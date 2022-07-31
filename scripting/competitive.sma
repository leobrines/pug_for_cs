#include <competitive/index>

#define PLUGIN "Competitive"
#define VERSION "0.12.4"
#define AUTHOR "Leopoldo Brines"

public plugin_init()
{
	register_plugin(PLUGIN, VERSION, AUTHOR);
	register_dictionary("competitive.txt");

	g_iStage = STAGE_DEAD

	#if AMXX_VERSION_NUM >= 183
	g_iMaxClients = MaxClients
	#else
	g_iMaxClients = get_maxplayers()
	#endif
	
	utils_init();
	cvars_init();

	votekick_init();
	votepause_init();
	mute_init();
	afkicker_start();
	stats_init();
	chooseteam_init();
	clrates_init();

	cmd_init();
	event_init();

	// Commands
	register_clcmd("say", "fnHookSay")
	register_clcmd("say_team", "fnHookSayTeam")

	set_task(5.0, "PugWarmup", _, _, _, "a", 1)
	set_task(3.0, "fnPostConfig", _, _, _, "a", 1)
}

#if AMXX_VERSION_NUM < 183
public plugin_cfg()
#else
public OnConfigsExecuted()
#endif
{
	static configfile[40];

	get_lastmaps(g_sLastMaps);
	get_configfile(configfile, charsmax(configfile));

	server_cmd("exec %s", configfile);
}

public plugin_unpause()
{
	PugWarmup()
}

public plugin_pause()
{
	remove_task();
	autoready_hide();
}

public client_connect (id) {
	if (user_iskicked(id)) {
		server_cmd("kick #%i ^"Votekick^"", get_user_userid(id));
		return
	}

	client_check_rates(id)
}

public client_putinserver (id) {
	if (!is_user_connected(id))
		return

	client_purge_data(id)
}

public client_purge_data (id) {
	client_reset_score(id);
	client_mute_reset(id);
	dmg_reset_user(id);
}

#if AMXX_VERSION_NUM < 183
public client_disconnect (id) {
#else
public client_disconnected (id) {
#endif
	if (!is_user_connected(id))
		return

	client_purge_data(id)

	if (!client_is_player(id))
		return;
	if (!game_is_started())
		autoready_check();
	if (!game_is_live())
		return;

	new TeamName:iTeam = TeamName:get_user_team(id)

	new iCount = get_teamplayers_count(iTeam) - 1
	new iAbsencePlayers = get_max_absence_players();

	if (iAbsencePlayers && iCount <= iAbsencePlayers) {
		chat_print(0, "%L", LANG_SERVER, "PUG_GAME_CANCELED_ABSENCE", iTeam == TEAM_TERRORIST ? g_szTeams2[TEAM_TERRORIST] : g_szTeams2[TEAM_CT])

		if (teamct_is_winning())
			game_finish(WINSTATUS_CTS)
		else if (teamtt_is_winning())
			game_finish(WINSTATUS_TERRORISTS)
		else
			game_finish(WINSTATUS_DRAW)
	}
}

// --------------------- Partes del PUG ---------------------

public PugWarmup ()
{
	g_iStage = STAGE_WARMUP

	votekick_reset();
	votepause_reset();

	g_iRound = 0;

	teams_reset_scores();
	clients_reset_scores();
	autoready_check();
	remove_break();

	fnRemoveHudMoney()

	exec_warmup();
}

public PugStart () {
	g_iStage = STAGE_START
	g_iVoteId = 0

	autoready_hide();

	fnNextVote()
}

// --------------------- Votaciones ---------------------

public fnNextVote()
{
	g_iVoteId++
	switch(g_iVoteId)
	{
		case 1:
		{
			if (is_votemap_allowed() && !is_votemap_ready()) {
				fnStartVoteMap()
			} else {
				set_votemap_ready(false);
				fnNextVote();
			}
		}
		case 2:
		{
			if (is_voteteam_allowed())
				fnStartVoteTeam()
			else
				fnNextVote();
		}
		default:
		{
			show_menu(0, 0, "^n", 1);
			set_votemap_ready(false);
			firsthalf();
		}
	}
}

public fnStartVoteMap () {
	g_mMap = maps_create_menu();

	new iPlayers[MAX_PLAYERS], iNum, iPlayer;
	get_players(iPlayers, iNum, "ch");
	
	for (new i;i < iNum;i++) 
	{
		iPlayer = iPlayers[i];

		if (client_is_player(iPlayer))
			menu_display(iPlayer, g_mMap);
	}

	set_task(0.1, "fnVoteListMap", TASK_HUD_VOTE, _, _, "b")
	set_task(float(get_votedelay()), "fnVoteMapEnd", _, _, _, "a", 1)
}

public fnVoteListMap()
{
	new count, hud[512], temp

	for (new i = 0 ; i < g_iMapCount; i++)
	{
		temp = g_iMapVotes[i];
		if (temp)
		{
			count++
			format(hud, charsmax(hud), "%s[%i] %s^n", hud, temp, g_sMapNames[i])
		}
	}

	if (!count)
	{	
		formatex(hud, charsmax(hud), "%L", LANG_SERVER, "PUG_NOVOTES")
	}

	show_hudtitle(0, "%L", LANG_SERVER, "PUG_VOTING_MAP")
	show_hudbody(0, hud)
}

public fnMapMenuHandle(const id, iMenu, iItem)
{
	if (iItem == MENU_EXIT)
		return PLUGIN_HANDLED;

	g_iMapVotes[iItem]++;
	g_iVotesCount++
	fnVoteListMap()

	return PLUGIN_HANDLED;
}

public fnVoteMapEnd()
{
	set_votemap_ready(true);

	remove_task(TASK_HUD_VOTE)
	menu_destroy(g_mMap);

	// Obtener ganador
	new winner, temp
	for (new i = 0 ; i < sizeof(g_iMapVotes) ; i++) {
		if (temp < g_iMapVotes[i]) {
			temp = g_iMapVotes[i];
			winner = i
		}
	}

	if (!winner) {
		chat_print(0, "%L", LANG_SERVER, "PUG_MAP_CURRENT")
		fnNextVote();
		return
	}

	new sMapName[32]
	formatex(sMapName, charsmax(sMapName), "%s", g_sMapNames[winner])

	chat_print(0, "%L", LANG_SERVER, "PUG_MAP_CHANGE", sMapName)
	set_task(4.0, "votemap_changemap", _, sMapName, charsmax(sMapName), "a", 1)
}

public votemap_changemap (map[])
	server_changemap(map);

public fnStartVoteTeam()
{
	arrayset(g_iTeamVotes, 0, sizeof(g_iTeamVotes))
	g_iTeamCount = 0
	g_mTeam = menu_create("\gVotacion de equipos", "fnTeamMenuHandle")

	new sNum[10]

	for (new i; i < sizeof(g_sTeamNames); i++)
	{
		num_to_str(g_iTeamCount, sNum, charsmax(sNum));
		menu_additem(g_mTeam, g_sTeamNames[i], sNum);
		g_iTeamCount++
	}

	menu_setprop(g_mTeam, MPROP_EXIT, MEXIT_NEVER);

	new iPlayers[MAX_PLAYERS], iNum, iPlayer;
	get_players(iPlayers, iNum, "ch");

	for (new i;i < iNum;i++) 
	{
		iPlayer = iPlayers[i];

		if (client_is_player(iPlayer))
		{
			menu_display(iPlayer, g_mTeam);
		}
	}

	set_task(0.2, "fnVoteListTeam", TASK_HUD_VOTE, _, _, "b")
	set_task(float(get_votedelay()), "fnVoteTeamEnd", _, _, _, "a", 1)
}

public fnVoteListTeam()
{
	new count, hud[512], temp

	for (new i = 0 ; i < g_iTeamCount; i++)
	{
		temp = g_iTeamVotes[i]
		if (temp)
		{
			count++
			format(hud, charsmax(hud), "%s[%i] %s^n", hud, temp, g_sTeamNames[i])
		}
	}

	if (!count)
	{	
		formatex(hud, charsmax(hud), "%L", LANG_SERVER, "PUG_NOVOTES")
	}

	show_hudtitle(0, "%L", LANG_SERVER, "PUG_VOTING_TEAM")
	show_hudbody(0, hud)
}

public fnTeamMenuHandle(const id, iMenu, iItem)
{
	if (iItem == MENU_EXIT)
		return PLUGIN_HANDLED;

	g_iTeamVotes[iItem]++
	g_iVotesCount++

	fnVoteListTeam()

	return PLUGIN_HANDLED;
}

public fnVoteTeamEnd()
{
	remove_task(TASK_HUD_VOTE)
	menu_destroy(g_mTeam);

	// Obtener ganador
	new winner, temp
	for (new i = 0 ; i < sizeof (g_iTeamVotes) ; i++)
	{
		if (temp < g_iTeamVotes[i])
		{
			temp = g_iTeamVotes[i]
			winner = i
		}
	}

	chat_print(0, "%L", LANG_SERVER, "PUG_WINNER_CHOOSEN", g_sTeamNames[winner])

	switch (winner)
	{
		case 0:
			fnNextVote(); // Dejar los equipos iguales

		case 1: {
			fnTeamsRandomize(); fnNextVote();
		}

	}
}

public fnTeamsRandomize()
{
	chat_print(0, "%L", LANG_SERVER, "PUG_RANDOMIZING_TEAMS")

	static Players[32]
	new playerCount, i, player
	get_players(Players, playerCount, "ch")
	
	for (i=0; i<playerCount; i++)
	{
		player = Players[i]
		
		switch ( client_get_team(player) )
		{
			case TEAM_TERRORIST, TEAM_CT: 
			{
				new TeamName:random_team = TeamName:random_num(1, 2)

				if (team_is_full(random_team)) {
					if (random_team == TEAM_TERRORIST)
						client_set_team(player, TEAM_TERRORIST)
					else
						client_set_team(player, TEAM_CT)
				}else {
					client_set_team(player, random_team)
				}
				
			}
		}
	}
}

public fnHudMoney()
{
	new sMessage[1024], sName[32], iLen, iMoney;
	new iPlayersTeam[MAX_PLAYERS], iNumTeam, iPlayerTeam;

	new sTitle[512];
	format(sTitle, charsmax(sTitle), "%L", LANG_SERVER, "PUG_TEAM_MONEY");

	new iPlayers[MAX_PLAYERS], iNum, iPlayer;
	get_players(iPlayers, iNum, "ch");

	for (new i; i < iNum; i++)
	{
		iPlayer = iPlayers[i]

		if (!client_is_player(iPlayer))
			continue;

		iLen = 0
		sMessage[0] = '^0'

		show_hudtitle(iPlayer, sTitle);
		get_players(iPlayersTeam, iNumTeam, "eh", client_get_team(iPlayer) == TEAM_TERRORIST ? "TERRORIST" : "CT");

		for (new e; e < iNumTeam; e++)
		{
			iPlayerTeam = iPlayersTeam[e]

			iMoney = client_get_money(iPlayerTeam);
			get_user_name(iPlayerTeam, sName, charsmax(sName));
			iLen += format(sMessage[iLen], charsmax(sMessage) - iLen, "%s: $%d^n", sName, iMoney);
		}

		show_hudbody(iPlayer, sMessage);
	}
}

public fnRemoveHudMoney()
{
	if (task_exists(TASK_DISPLAY_INFO))
		remove_task(TASK_DISPLAY_INFO)
}

// --------------------- Utilidades ---------------------

public maps_create_menu () {
	new menu, patch[40];
	new map[32], mapfile;

	menu = menu_create("\gVotacion de mapa", "fnMapMenuHandle")
	g_iMapCount = 0;
	arrayset(g_iMapVotes, 0, sizeof(g_iMapVotes));

	// Mapa actual
	formatex(g_sMapNames[g_iMapCount], charsmax(g_sMapNames[]), "%L", LANG_SERVER, "PUG_VOTING_MAPCURRENT");
		
	menu_additem(menu, g_sMapNames[g_iMapCount]);

	g_iMapCount++;

	get_mapfile(patch, charsmax(patch));
	mapfile = fopen(patch, "rb");

	while (!feof(mapfile)) {
		fgets(mapfile, map, charsmax(map));
		trim(map);
		
		if (!is_map_valid(map) || equali(map, g_sCurrentMap))
			continue;

		copy(g_sMapNames[g_iMapCount], charsmax(g_sMapNames[]), map);

		if ( is_lastmaps_blocked() && (equali(map, g_sLastMaps[0]) || equali(map, g_sLastMaps[1])) ) {
			new text[32]
			formatex(text, charsmax(text), "\d%i. %s", g_iMapCount+1, map)

			#if AMXX_VERSION_NUM >= 183
			menu_addtext2(menu, text)
			#else
			menu_addtext(menu, text)
			#endif
		} else {
			menu_additem(menu, map);
		}
	
		g_iMapCount++;
	}
	
	fclose(mapfile);
	
	return menu;
}

public fnHookSay(id)
{
	new szArgs[192];
	read_args(szArgs, charsmax(szArgs));
	remove_quotes(szArgs); 
	    
	if (fnCheckCommand(id, szArgs)) // If argument is empty or is command
		return PLUGIN_HANDLED;

	if (!is_globalsay_allowed()) {
		chat_print(id, "%L", LANG_SERVER, "PUG_GLOBALSAY_DISABLED");
		return PLUGIN_HANDLED;
	}

	new TeamName:iTeam, szMessage[192], sName[32];

	iTeam = client_get_team(id);
	get_user_name(id, sName, charsmax(sName));

	switch (iTeam)
	{
		case TEAM_TERRORIST, TEAM_CT:
			formatex(szMessage, charsmax(szMessage), "^1%s^3%s^1 : %s", is_user_alive(id) ? "" : "*DEAD* ", sName, szArgs)

		default:
		{
			if (!is_user_admin(id))
			{
				chat_print(id, "%L", LANG_SERVER, "PUG_SPEC_DONT_SAY");
				return PLUGIN_HANDLED;
			}

			formatex(szMessage, charsmax(szMessage), "^1*ADMIN* ^3%s^1 : %s", sName, szArgs)
		}
	}

	new iPlayers[MAX_PLAYERS], iNum, iPlayer;
	get_players(iPlayers, iNum, "ch");
	
	for (new i;i < iNum;i++)
	{
		iPlayer = iPlayers[i];
		fnSendMessage(iPlayer, iTeam == TEAM_TERRORIST ? print_team_red : iTeam == TEAM_CT ? print_team_blue : print_team_grey, szMessage)
	}
	
	return PLUGIN_HANDLED;
}

public fnHookSayTeam(id)
{
	new szArgs[192];
	read_args(szArgs, charsmax(szArgs));
	remove_quotes(szArgs); 
	    
	if (fnCheckCommand(id, szArgs)) // If argument is empty or is command
		return PLUGIN_HANDLED;

	new TeamName:iTeam, szMessage[192], sName[32];

	iTeam = client_get_team(id);
	get_user_name(id, sName, charsmax(sName));

	switch (iTeam)
	{
		case TEAM_TERRORIST:
			formatex(szMessage, charsmax(szMessage), "^1%s(Terrorista) ^3%s^1 : %s", is_user_alive(id) ? "" : "*DEAD* ", sName, szArgs)

		case TEAM_CT:
			formatex(szMessage, charsmax(szMessage), "^1%s(Antiterrorista) ^3%s^1 : %s", is_user_alive(id) ? "" : "*DEAD* ", sName, szArgs)

		default:
			formatex(szMessage, charsmax(szMessage), "^1(Espectador) ^3%s^1 : %s", sName, szArgs)

	}

	new iPlayers[MAX_PLAYERS], iNum, iPlayer;
	get_players(iPlayers, iNum, "ch");
	
	for (new i;i < iNum; i++)
	{
		iPlayer = iPlayers[i];

		if (client_get_team(iPlayer) == iTeam)
			fnSendMessage(iPlayer, iTeam == TEAM_TERRORIST ? print_team_red : iTeam == TEAM_CT ? print_team_blue : print_team_grey, szMessage)

	}
	
	return PLUGIN_HANDLED;
}

public bool:fnCheckCommand (const id, szArgs[192])
{
	if ((szArgs[0] == '.') || (szArgs[0] == '!')) 
	{
		client_cmd(id, szArgs, charsmax(szArgs));
		return true; 
	}

	if (equali(szArgs, ""))
		return true; 
	
	return false;
}

public fnSendMessage(id, color, msg[192])
{
	if (is_msgsound_allowed())
		client_cmd(id, "spk buttons/lightswitch2")

	client_print_color2(id, color, msg);
}

public fnPostConfig()
{
	// Format some string
	get_prefix(TAG, charsmax(TAG));
	get_currentmap(g_sCurrentMap, charsmax(g_sCurrentMap));
	get_serverlang(g_sLang, charsmax(g_sLang));

	format(TAG, charsmax(TAG), "!t%s!y", TAG)

	formatex(g_szTeams[TEAM_UNASSIGNED], charsmax(g_szTeams[]), "%L", LANG_SERVER, "UNASSIGNED")

	formatex(g_szTeams[TEAM_TERRORIST], charsmax(g_szTeams[]), "%L", LANG_SERVER, "TERRORISTS")
	formatex(g_szTeams[TEAM_CT], charsmax(g_szTeams[]), "%L", LANG_SERVER, "CTS")
	formatex(g_szTeams[TEAM_SPECTATOR], charsmax(g_szTeams[]), "%L", LANG_SERVER, "SPECTATORS")

	formatex(g_szTeams2[TEAM_TERRORIST], charsmax(g_szTeams[]), "%L", LANG_SERVER, "TERRORIST")
	formatex(g_szTeams2[TEAM_CT], charsmax(g_szTeams[]), "%L", LANG_SERVER, "CT")
	formatex(g_szTeams2[TEAM_SPECTATOR], charsmax(g_szTeams), "%L", LANG_SERVER, "SPECTATOR")

	formatex(g_sTeamNames[0], charsmax(g_sTeamNames[]), "%L", LANG_SERVER, "PUG_VOTING_NOTSORTED");
	formatex(g_sTeamNames[1], charsmax(g_sTeamNames[]), "%L", LANG_SERVER, "PUG_VOTING_RANDOM");
}

public fnUpdateLastMaps()
{
	// Update last maps
	formatex(g_sLastMaps[1], charsmax(g_sLastMaps[]), "%s", g_sLastMaps[0])
	formatex(g_sLastMaps[0], charsmax(g_sLastMaps[]), "%s", g_sCurrentMap)

	set_lastmaps(g_sLastMaps);
}

// Commands

public cmd_init () {
	register_concmd(".auto", "cmd_auto", PUG_CMD_LVL)
	register_concmd(".manual", "cmd_manual", PUG_CMD_LVL)
	register_concmd(".restart", "cmd_restart", PUG_CMD_LVL);
	register_concmd(".start", "cmd_start", PUG_CMD_LVL);
	register_concmd(".cancel", "cmd_cancel", PUG_CMD_LVL);
	register_concmd(".test", "cmd_test", PUG_CMD_LVL);
	register_concmd(".globalsay", "cmd_globalsay", PUG_CMD_LVL);

	register_concmd(".menu", "cmd_menu", ADMIN_ALL);
	register_concmd(".mute", "cmd_mute", ADMIN_ALL);
	register_concmd(".unmute", "cmd_unmute", ADMIN_ALL);
	register_concmd(".votekick", "cmd_votekick", ADMIN_ALL);
	register_concmd(".votepause", "cmd_votepause", ADMIN_ALL);

	register_clcmd("chooseteam", "cmd_chooseteam");
	register_clcmd("nightvision", "cmd_menu");

	register_concmd("kill", "block_kill")
}

public cmd_chooseteam (id) {
	// Unlocks the selection of a single team
	set_pdata_int(id, 125, get_pdata_int(id, 125, 5) & ~(1<<8), 5);

	return PLUGIN_CONTINUE;
}

public cmd_restart (id, level, cid) {
	if (!cmd_access(id, level, cid, 1))
		return PLUGIN_CONTINUE;

	server_cmd("sv_restart 3");

	return PLUGIN_HANDLED;
}

public cmd_start (id, level, cid) {
	if (!cmd_access(id, level, cid, 1))
		return PLUGIN_CONTINUE;

	if (game_is_started()) {
		chat_print(id, "%L", LANG_SERVER, "PUG_ACTION_NOTALLOWED")
		return PLUGIN_HANDLED;
	}

	new sName[MAX_NAME_LENGTH]
	get_user_name(id, sName, charsmax(sName))
	chat_print(0, "%L", LANG_SERVER, "PUG_FORCE_GAME", sName)

	PugStart();

	return PLUGIN_HANDLED;
}

public cmd_cancel (id, level, cid) {
	if (!cmd_access(id, level, cid, 1))
		return PLUGIN_CONTINUE;

	if (!game_is_live()) {
		chat_print(id, "%L", LANG_SERVER, "PUG_ACTION_NOTALLOWED");
		return PLUGIN_HANDLED;
	}

	new sName[MAX_NAME_LENGTH]
	get_user_name(id, sName, charsmax(sName))
	chat_print(0, "%L", LANG_SERVER, "PUG_FORCE_CANCEL", sName)

	PugWarmup();

	return PLUGIN_HANDLED;
}

public cmd_manual (id, level, cid) {
	if (!cmd_access(id, level, cid, 1))
		return PLUGIN_CONTINUE;

	new sName[MAX_NAME_LENGTH]
	get_user_name(id, sName, charsmax(sName))
	chat_print(0, "%L", LANG_SERVER, "PUG_CHANGE_TO_MANUAL", sName);

	set_server_manual();
	
	return PLUGIN_HANDLED;
}

public cmd_auto (id, level, cid) {
	if (!cmd_access(id, level, cid, 1))
		return PLUGIN_CONTINUE;

	new sName[MAX_NAME_LENGTH]
	get_user_name(id, sName, charsmax(sName))
	chat_print(0, "%L", LANG_SERVER, "PUG_CHANGE_TO_AUTO", sName);

	set_server_auto();
	
	return PLUGIN_HANDLED;
}

public cmd_test (id, level, cid) {
	if (!cmd_access(id, level, cid, 1))
		return PLUGIN_CONTINUE;

	if (!game_is_started())
		firsthalf();

	return PLUGIN_HANDLED;
}

public cmd_menu (id) {
	pugmenu_start(id);
	return PLUGIN_HANDLED
}

public cmd_mute (const id, level, cid) {
	new const target = find_target(id, level, cid);

	if (target)
		client_mute(id, target);

	return PLUGIN_HANDLED
}

public cmd_unmute (const id, level, cid) {
	new const target = find_target(id, level, cid);

	if (target)
		client_unmute(id, target);

	return PLUGIN_HANDLED
}

public cmd_votekick (id, level, cid) {
	if (!client_is_player(id) || !game_is_live())
		return PLUGIN_CONTINUE;
		
	new const target = find_target(id, level, cid);

	if (target)
		votekick_start(id, target);

	return PLUGIN_HANDLED
}

public cmd_votepause (id, level, cid) {
	if (!client_is_player(id) || !game_is_live())
		return PLUGIN_CONTINUE;

	votepause_start(id);

	return PLUGIN_HANDLED
}

public cmd_globalsay(id) {
	switch_globalsay()

	if (is_globalsay_allowed()) {
		chat_print(id, "%L", LANG_SERVER, "PUG_GLOBALSAY_ENABLED");
		return
	}

	chat_print(id, "%L", LANG_SERVER, "PUG_GLOBALSAY_DISABLED");
}

public block_kill () {
	if (game_is_live())
		return PLUGIN_HANDLED;

	return PLUGIN_CONTINUE; 
}

static find_target (id, level, cid) {
	if (!cmd_access(id, level, cid, 2)) {
		chat_print(id, "%L", LANG_SERVER, "PUG_CMD_SPECIFY");
		return 0;
	}

	new target, targetName[32];

	read_argv(1, targetName, charsmax(targetName));
	target = cmd_target(id, targetName, CMDTARGET_OBEY_IMMUNITY | 
										CMDTARGET_NO_BOTS |
										CMDTARGET_ALLOW_SELF);

	if (!target) {
		chat_print(id, "%L", LANG_SERVER, "PUG_CMD_UNAVAILABLE")
		return 0;
	}

	return target;
}
