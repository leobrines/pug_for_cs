#include <competitive/index>

#define PLUGIN "Competitive"
#define VERSION "0.10.2"
#define AUTHOR "Leopoldo Brines"

#define TASK_HUD_VOTE    996541
#define TASK_HUD_MONEY 3001

#define MAX_BYTES 192

new g_iRound;

// Votation System
new g_iVoteId;
new g_iVotesCount

// VoteMap
new g_mMap
new g_iMapCount
new g_sMapNames[15][32]
new g_iMapVotes[15];
new g_sLastMaps[2][32]

// VoteTeam
new g_mTeam
new g_iTeamCount
new g_sTeamNames[2][32]
new g_iTeamVotes[32]

// Team names
new const g_szTeams[TeamName][MAX_NAME_LENGTH]
new const g_szTeams2[TeamName][MAX_NAME_LENGTH]

#define TASK_DISPLAY_INFO 4563

public plugin_precache () {
	showequip_precache();
}

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

	cmd_init();
	event_init();

	// Commands
	register_clcmd("say", "fnHookSay")
	register_clcmd("say_team", "fnHookSayTeam")

	// Hooks
	g_hGiveC4 = RegisterHookChain(RG_CSGameRules_GiveC4, "CSGameRules_GiveC4");
	g_hRoundFreezeEnd = RegisterHookChain(RG_CSGameRules_OnRoundFreezeEnd, "CSGameRules_OnRoundFreezeEnd")
	g_hPlayerPostThink = RegisterHookChain(RG_CBasePlayer_PostThink, "CBasePlayer_PostThink")
	g_hHasRestrictItem = RegisterHookChain(RG_CBasePlayer_HasRestrictItem, "CBasePlayer_HasRestrictItem")
	g_hRoundEnd = RegisterHookChain(RG_RoundEnd, "RoundEnd")

	DisableHookChain(g_hGiveC4)
	DisableHookChain(g_hPlayerPostThink)

	// Events
	register_event("Money", "event_money", "b")
	register_event("Damage", "event_damage", "b", "2!0", "3=0", "4!0")
	register_logevent("event_new_round", 3, "2=Spawned_With_The_Bomb");
	register_event("DeathMsg", "event_death_player", "a", "1!0", "2!0");

	set_task(5.0, "PugWarmup", _, _, _, "a", 1)
	set_task(3.0, "fnPostConfig", _, _, _, "a", 1)
}

public plugin_cfg()
{
	static configfile[40];

	get_lastmaps(g_sLastMaps);
	get_configfile(configfile, charsmax(configfile));

	set_default_gamedesc();

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

	DisableHookChain(g_hGiveC4)
	DisableHookChain(g_hPlayerPostThink)
	DisableHookChain(g_hRoundFreezeEnd)
	DisableHookChain(g_hHasRestrictItem)
	DisableHookChain(g_hRoundEnd)
}

public client_connect (id) {
	if (user_iskicked(id))
		server_cmd("kick #%i ^"Votekick^"", get_user_userid(id));
}

public client_putinserver (id) {
	client_reset_score(id);
}

public client_disconnect (id) {
	client_mute_reset(id);

	if (!client_is_player(id))
		return;
	if (!game_is_started())
		autoready_check();
	if (!game_is_live())
		return;

	new TeamName:iTeam = TeamName:get_user_team(id)

	new iCount = get_teamplayers(iTeam, false) - 1
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

// --------------------- HookChains ---------------------

public CSGameRules_OnRoundFreezeEnd()
{
	votepause_check(true);
	dmg_reset();
	set_task(0.8, "afkicker_newround_task");

	return HC_CONTINUE;
}

public CSGameRules_GiveC4()
	return HC_SUPERCEDE;

public CBasePlayer_HasRestrictItem(const id, const ItemID:item, const ItemRestType:type)
{
	if (item == ITEM_SHIELDGUN && type == ITEM_TYPE_BUYING && is_shield_blocked())
	{
		chat_print(id, "%L", LANG_SERVER, "PUG_WEAPON_NOTALLOWED")
		SetHookChainReturn(ATYPE_INTEGER, 1);
		return HC_SUPERCEDE;
	}
	else if (item == ITEM_NVG && type == ITEM_TYPE_BUYING  && is_nvg_blocked())
	{
		chat_print(id, "%L", LANG_SERVER, "PUG_WEAPON_NOTALLOWED")
		SetHookChainReturn(ATYPE_INTEGER, 1);
		return HC_SUPERCEDE;
	}
	else if ((item == ITEM_FLASHBANG || item == ITEM_HEGRENADE || item == ITEM_SMOKEGRENADE) && !game_is_live() && are_grenades_blocked())
	{	
		SetHookChainReturn(ATYPE_INTEGER, 1);
		return HC_SUPERCEDE;
	}
	else if (item == ITEM_DEFUSEKIT && !game_is_live())
	{
		SetHookChainReturn(ATYPE_INTEGER, 1);
		return HC_SUPERCEDE;
	}

	return HC_CONTINUE;
}

public CBasePlayer_PostThink(const id)
{
	if (!is_user_connected(id) || !client_is_player(id))
		return;

	set_entvar(id, var_maxspeed, 0.1)

	client_cmd(id, "+showscores")
}

public RoundEnd (WinStatus:status, ScenarioEventEndRound:event, Float:tmDelay) {
	if (!game_is_live())
		return HC_CONTINUE;

	switch (event) {
		case ROUND_GAME_RESTART: {
			if (!is_firstround())
				round_restarted();

			return HC_CONTINUE;
		}
		case ROUND_GAME_COMMENCE: {
			return HC_CONTINUE;
		}
	}

	switch (status) {
		case WINSTATUS_CTS:
			teamct_add_score();
		case WINSTATUS_TERRORISTS, WINSTATUS_DRAW:
			teamtt_add_score();
		default:
			return HC_CONTINUE;
	}

	g_iRound++;

	if (get_cvar_num("pug_dmgmode")) {
		new iPlayers[MAX_PLAYERS], iNum;
		get_players(iPlayers, iNum, "ach");
		
		for (new i;i < iNum;i++)  {
			new args[2];
			args[0] = iPlayers[i];
			set_task(1.0, "printdmg_task", _, args, charsmax(args), "a", 1);
		}
	}

	check_halfend();

	if (game_is_live())
		votepause_check(false);

	return HC_CONTINUE;
}

// --------------------- Partes del PUG ---------------------

public PugWarmup ()
{
	g_iStage = STAGE_WARMUP

	set_default_gamedesc();

	votekick_reset();
	votepause_reset();

	g_iRound = 0;

	teams_reset_scores();
	clients_reset_scores();
	autoready_check();

	fnPregameHooks()

	client_cmd(0, "-showscores")
	DisableHookChain(g_hPlayerPostThink)

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
	
	new type = 0;
	for (i=0; i<playerCount; i++)
	{
		player = Players[i]
		
		switch ( client_get_team(player) )
		{
			case 1: 
			{
				if (type == 0)
				{
					type = random_num(1, 2)
					rg_set_user_team(player, _:type)
					rg_join_team(player, TeamName:type)
				}
				else
				{
					rg_set_user_team(player, (type == 1) ? 2 : 1)
					rg_join_team(player, (type == 1) ? (TeamName:2) : (TeamName:1) )
					type = 0
				}
			}
			case 2:
			{
				if (type == 0)
				{
					type = random_num(1, 2)
					rg_set_user_team(player, _:type)
					rg_join_team(player, TeamName:type)
				}
				else
				{
					rg_set_user_team(player, (type == 1) ? 2 : 1)
					rg_join_team(player, (type == 1) ? (TeamName:2) : (TeamName:1) )
					type = 0
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

	message_begin(MSG_ONE_UNRELIABLE, get_user_msgid("SayText"), _, id);
	write_byte(color ? color : 33);
	write_string(msg);
	message_end();
}

public print_dmgrdmg(const id) {
	new const mode = get_cvar_num("pug_dmgmode");

	console_print(id, "---------------------------------");
	dmgprint(id, (mode > 1));
	rdmgprint(id);
	console_print(id, "---------------------------------");
}

public fnShowScore()
{
	if (!game_is_live())
		return;
	
	static ttscore, ctscore;

	ctscore = teamct_get_score();
	ttscore = teamtt_get_score();

	if (teamct_is_winning())
		chat_print(0, "%L", LANG_SERVER, "PUG_SCORE_WINNING", g_szTeams[TEAM_CT], ctscore, ttscore)
	else if (teamtt_is_winning())
		chat_print(0, "%L", LANG_SERVER, "PUG_SCORE_WINNING", g_szTeams[TEAM_TERRORIST], ttscore, ctscore)
	else
		chat_print(0, "%L", LANG_SERVER, "PUG_SCORE_TIED", ctscore, ttscore)
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

public fnPregameHooks()
{
	EnableHookChain(g_hGiveC4)
	DisableHookChain(g_hRoundFreezeEnd)
}

public fnPugHooks()
{
	EnableHookChain(g_hRoundFreezeEnd)
	DisableHookChain(g_hGiveC4)
}

// Events

public event_money (id) {
	if (game_is_started())
		return PLUGIN_CONTINUE;

	client_give_money(id, 16000);

	return PLUGIN_HANDLED;
}

public event_damage (victim) {
	new attacker, damage;
	attacker = get_user_attacker(victim);
	damage = read_data(2);

	if (attacker != victim &&
		is_player_id(attacker) &&
		is_player_id(victim))
	{
		dmg_addattack(victim, attacker, damage);
	}
}

public event_death_player () {
	if (!game_is_live() || is_restarting())
		return; 
	
	new const killer = read_data(1);
	new const victim = read_data(2);

	client_add_frag(killer);
	client_add_death(victim);

	new args[2];
	args[0] = victim;
	set_task(1.0, "printdmg_task", _, args, charsmax(args), "a", 1);
}

public printdmg_task (args[])
	print_dmgrdmg(args[0]);

public event_new_round () {
	if (!game_is_live())
		return PLUGIN_CONTINUE;

	round_start();

	if (!g_iRound) {
		teams_reset_scores();
		clients_reset_scores();
	}

	new showMoneyMode = get_showmoney_mode();

	switch (showMoneyMode) {
		case 1: {
			clients_print_money();
		}
		case 2: {
			set_task(0.2, "fnHudMoney", TASK_DISPLAY_INFO, _, _, "b")
			set_task(float(get_freezetime()), "fnRemoveHudMoney", _, _, _, "a", 1)
		}
		case 3: {
			show_team_equipment();
		}
	}

	return PLUGIN_HANDLED;
}
