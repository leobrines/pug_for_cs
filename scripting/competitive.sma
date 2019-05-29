#include <amxmodx>
#include <reapi>
#include <amxmisc>
#include <competitive/index>

#define PLUGIN "CS Pug Mod"
#define VERSION "3.83"
#define AUTHOR "Leopoldo Brines"

#define TASK_HUD_READY 552214
#define TASK_HUD_VOTE    996541
#define TASK_HUD_MONEY 3001

#define MAX_MAPS 7
#define MAX_BYTES 192

// Ready System
new g_iReadyCount
new bool:g_bReady[MAX_PLAYERS+1]
new g_iRound;
new g_iRoundCT;
new g_iRoundTT;

// Message
new gMsgTeamScore;
new gMsgScoreInfo;

// Damage - Hits - Frags - Deaths
new g_iDmg[MAX_PLAYERS+1][MAX_PLAYERS+1]
new g_iHits[MAX_PLAYERS+1][MAX_PLAYERS+1]
new g_iFrags[MAX_PLAYERS+1]
new g_iDeaths[MAX_PLAYERS+1]

// Votation System
new g_iVoteId;
new g_iVotesCount

// VoteMap
new g_mMap
new g_iMapCount
new g_sMapNames[MAX_MAPS][32]
new g_iMapVotes[MAX_MAPS]
new g_sLastMaps[2][32]
// new bool:vote_map

// VoteTeam
new g_mTeam
new g_iTeamCount
new g_sTeamNames[2][32]
new g_iTeamVotes[32]

// VotePause
new g_mPause
new g_iPauseCount
new g_sPauseOptions[2][32]
new g_iPauseVotes[32]

// Mute system
new g_bMuted[MAX_PLAYERS+1][MAX_PLAYERS+1]

// Hook
new HookChain:g_hDeadPlayerWeapons
new HookChain:g_hGiveC4
new HookChain:g_hPlayerPostThink
new HookChain:g_hRoundFreezeEnd
new HookChain:g_hPlayerSpawn
new HookChain:g_hPlayerKilled
new HookChain:g_hHasRestrictItem
new HookChain:g_hRoundEnd
new HookChain:g_hChooseTeam
// new HookChain:g_hTakeDamage

// Team names
new const g_szTeams[TeamName][MAX_NAME_LENGTH]
new const g_szTeams2[TeamName][MAX_NAME_LENGTH]

// AFK kicker
#define MIN_AFK_TIME 30		// I use this incase stupid admins accidentally set mp_afktime to something silly.
#define WARNING_TIME 15		// Start warning the user this many seconds before they are about to be kicked.
#define CHECK_FREQ 5		// This is also the warning message frequency.

new g_iOldAngles[33][3]
new g_iAfkTime[33]
new bool:g_bSpawned[33] = {true, ...}

#define TASK_DISPLAY_INFO 4563

public plugin_precache () {
	precache_showequip_sprites();
}

// --------------------- AMX Forwards ---------------------

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
	chooseteam_init();

	// AFK Kicker
	set_task(float(CHECK_FREQ), "fnCheckPlayers", _, _, _, "b")

	// Commands
	register_clcmd("say", "fnHookSay")
	register_clcmd("say_team", "fnHookSayTeam")

	// for admins
	registerCommand("start", "fnForceStart", PUG_CMD_LVL)
	registerCommand("cancel", "fnForceCancel", PUG_CMD_LVL)
	registerCommand("manual", "fnManualON", PUG_CMD_LVL)
	registerCommand("auto", "fnManualOFF", PUG_CMD_LVL)

	// for players
	registerCommand("dmg", "fnDamage", ADMIN_ALL)
	registerCommand("hp", "fnHp", ADMIN_ALL)
	registerCommand("mute", "fnMute", ADMIN_ALL)
	registerCommand("unmute", "fnUnmute", ADMIN_ALL)
	registerCommand("pause", "fnStartVotePause", ADMIN_ALL)
	registerCommand("money", "fnShowMoney", ADMIN_ALL)

	// Hooks
	g_hDeadPlayerWeapons = RegisterHookChain(RG_CSGameRules_DeadPlayerWeapons, "CSGameRules_DeadPlayerWeapons")
	g_hGiveC4 = RegisterHookChain(RG_CSGameRules_GiveC4, "CSGameRules_GiveC4");
	g_hRoundFreezeEnd = RegisterHookChain(RG_CSGameRules_OnRoundFreezeEnd, "CSGameRules_OnRoundFreezeEnd")
	g_hPlayerKilled = RegisterHookChain(RG_CSGameRules_PlayerKilled, "CSGameRules_PlayerKilled")
	//g_hTakeDamage = RegisterHookChain(RG_CBasePlayer_TakeDamage, "CBasePlayer_TakeDamage")

	g_hPlayerPostThink = RegisterHookChain(RG_CBasePlayer_PostThink, "CBasePlayer_PostThink")
	g_hPlayerSpawn = RegisterHookChain(RG_CSGameRules_PlayerSpawn, "CSGameRules_PlayerSpawn")
	g_hHasRestrictItem = RegisterHookChain(RG_CBasePlayer_HasRestrictItem, "CBasePlayer_HasRestrictItem")
	g_hRoundEnd = RegisterHookChain(RG_RoundEnd, "RoundEnd")
	//g_hChooseTeam = RegisterHookChain(RG_HandleMenu_ChooseTeam, "HandleMenu_ChooseTeam")


	DisableHookChain(g_hDeadPlayerWeapons)
	DisableHookChain(g_hGiveC4)
	DisableHookChain(g_hPlayerPostThink)

	// Server messages
	gMsgTeamScore 	= get_user_msgid("TeamScore")
	gMsgScoreInfo 	= get_user_msgid("ScoreInfo")

	register_message(gMsgTeamScore, "fnTeamScore")
	register_message(gMsgScoreInfo, "fnScoreInfo")

	// Events
	register_event("Money", "money_handler", "b")
	register_event("Damage", "damage_handler", "b", "2!0", "3=0", "4!0")

	// DISPLAY MONEY
	register_event("HLTV", "event_new_round", "a", "1=0", "2=0");

	set_task(5.0, "PugWarmup", _, _, _, "a", 1)
	set_task(3.0, "fnPostConfig", _, _, _, "a", 1)
}

public plugin_cfg()
{
	get_lastmaps(g_sLastMaps);

	server_cmd("exec pugconfig.cfg")

	update_game_description();

	// fnUpdateServerName();
}

public plugin_unpause()
{
	PugWarmup()
}

public plugin_pause()
{
	fnDisplayReady(0.0);

	remove_task();

	DisableHookChain(g_hDeadPlayerWeapons)
	DisableHookChain(g_hGiveC4)
	DisableHookChain(g_hPlayerPostThink)
	DisableHookChain(g_hRoundFreezeEnd)
	DisableHookChain(g_hPlayerSpawn)
	DisableHookChain(g_hPlayerKilled)
	DisableHookChain(g_hHasRestrictItem)
	DisableHookChain(g_hRoundEnd)
	DisableHookChain(g_hChooseTeam)
}

public client_putinserver(id)
{
	g_iFrags[id] = 0
	g_iDeaths[id] = 0
	arrayset(g_bMuted[id], false, sizeof(g_bMuted))
	g_iAfkTime[id] = 0

	#if AMXX_VERSION_NUM < 183
	set_task(0.2, "chatcolor_send_teaminfo", id);
	#endif
}

public client_disconnect(id)
{
	new TeamName:iTeam = TeamName:get_user_team(id)

	if (TEAM_TERRORIST <= iTeam <= TEAM_CT)
	{
		fnNotReady(id)

		if (game_is_live())
		{
			new iCount = getPlayersTeam(iTeam, false) - 1
			new iAbsencePlayers = get_max_absence_players();

			if (iAbsencePlayers && iCount <= iAbsencePlayers)
			{
				chat_print(0, "%L", LANG_SERVER, "PUG_GAME_CANCELED_ABSENCE", iTeam == TEAM_TERRORIST ? g_szTeams2[TEAM_TERRORIST] : g_szTeams2[TEAM_CT])

				if (g_iRoundCT > g_iRoundTT)
					PugFinished(WINSTATUS_CTS)

				else if (g_iRoundTT > g_iRoundCT)
					PugFinished(WINSTATUS_TERRORISTS)

				else if (g_iRoundTT == g_iRoundCT)
					PugFinished(WINSTATUS_DRAW)

			}
		}
	}
}

// --------------------- HookChains ---------------------

public CSGameRules_OnRoundFreezeEnd()
{	
	set_freezetime(15);
	fnResetDmg();
	return HC_CONTINUE;
}

public CSGameRules_DeadPlayerWeapons()
{
	//SetHookChainReturn(ATYPE_INTEGER, GR_PLR_DROP_GUN_NO);
	//return HC_SUPERCEDE;
}

public CSGameRules_GiveC4() return HC_SUPERCEDE;

public CSGameRules_PlayerKilled(const victim, const killer, const inflictor)
{
	if (1 <= victim <= g_iMaxClients)
	{
		g_iFrags[killer]++
		g_iDeaths[victim]++

		fnDamageAuto(victim)
	}
}

public CSGameRules_PlayerSpawn(const id)
{
	if (is_user_alive(id) && is_user_connected(id))
	{
		g_bSpawned[id] = false

		new sid[1]
		sid[0] = id
		set_task(0.75, "fnDelayedSpawn", _, sid, 1)     // Give the player time to drop to the floor when spawning
	}
}

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
	if (!is_user_connected(id) || !fnIsTeam(id))
		return;

	set_entvar(id, var_maxspeed, 0.1)

	client_cmd(id, "+showscores")
}

/*
public CBasePlayer_TakeDamage(const iVictim, iInflictor, iAttacker, Float:flDamage, bitsDamageType)
{
	if (iAttacker == iVictim
	|| !rg_is_player_can_takedamage(iAttacker, iVictim)
	|| flDamage < 1.0
	|| ( bitsDamageType != DMG_BULLET && bitsDamageType != DMG_GRENADE) )
		return HC_CONTINUE;

	g_iDmg[iVictim][iAttacker] += floatround(flDamage)
	g_iHits[iVictim][iAttacker]++

	return HC_CONTINUE;
}	
*/

public RoundEnd(WinStatus:status, ScenarioEventEndRound:event, Float:tmDelay)
{
	if (game_is_live() && event != ROUND_GAME_RESTART && event != ROUND_GAME_COMMENCE)
	{
		g_iRound++
		// fnUpdateServerName();

		if (status == WINSTATUS_CTS)
		{
			g_iRoundCT++
			emessage_begin(MSG_ALL, gMsgTeamScore)
			ewrite_string("CT")
			ewrite_short(g_iRoundCT)
			emessage_end()
		}
		else if (status == WINSTATUS_TERRORISTS)
		{
			g_iRoundTT++
			emessage_begin(MSG_ALL, gMsgTeamScore)
			ewrite_string("TT")
			ewrite_short(g_iRoundTT)
			emessage_end()
		}

		new iPlayers[MAX_PLAYERS], iNum;
		get_players(iPlayers, iNum, "ach");
		
		for (new i;i < iNum;i++) 
			fnDamageAuto(iPlayers[i])

		if (g_iStage == STAGE_FIRSTHALF)
		{
			if (g_iRound == get_maxrounds()/2)
				PugHalftime();

			else
				fnShowScore();

		}
		else if (g_iStage == STAGE_SECONDHALF)
		{
			new iMaxRounds = (get_maxrounds()/2)

			if (g_iRoundCT > iMaxRounds) {
				PugFinished(WINSTATUS_CTS)
			} else if (g_iRoundTT > iMaxRounds) {
				PugFinished(WINSTATUS_TERRORISTS)
			} else if (g_iRoundTT == iMaxRounds && g_iRoundCT == iMaxRounds) {
				if (is_tie_allowed())
					PugFinished(WINSTATUS_DRAW)
				else
					PugHalftime()
			}

			else
				fnShowScore();
		}
		else if (g_iStage == STAGE_OVERTIME)
		{
			new iMaxRounds = (get_maxrounds()/2)
			new iRoundsOT = (get_overtime_rounds()/2)

			if (g_iRoundCT > (iMaxRounds+iRoundsOT))
				PugFinished(WINSTATUS_CTS)

			else if (g_iRoundTT > (iMaxRounds+iRoundsOT))
				PugFinished(WINSTATUS_TERRORISTS)

			/*
			else if (g_iRoundTT == (iMaxRounds+iRoundsOT) && g_iRoundCT == (iMaxRounds+iRoundsOT))
			{
				PugHalftime()
			}
			*/
			else 
				fnShowScore();

		}
	}

	return HC_CONTINUE;
}

public HandleMenu_ChooseTeam(const id, MenuChooseTeam:iNewTeam)
{
	new TeamName:iOldTeam = client_get_team(id);

	if (STAGE_START <= g_iStage <= STAGE_FINISHED &&
			TEAM_TERRORIST <= iOldTeam <= TEAM_CT)
	{
		chat_print(id, "%L", LANG_SERVER, "PUG_TEAMS_NOT_CHANGE");

		SetHookChainReturn(ATYPE_INTEGER, 0)
		return HC_BREAK
	}
	
	if (_:iNewTeam == _:iOldTeam)
	{
		chat_print(id, "%L", LANG_SERVER, "PUG_TEAMS_SAMET");

		SetHookChainReturn(ATYPE_INTEGER, 0)
		return HC_BREAK
	}

	switch (iNewTeam)
	{
		case MenuChoose_Spec:
		{
			if (!is_spectator_allowed() && !access(id, PUG_CMD_LVL))
			{
				chat_print(id, "%L", LANG_SERVER, "PUG_TEAMS_SPECTATORS");
				fnNotReady(id)

				SetHookChainReturn(ATYPE_INTEGER, 0)
				return HC_BREAK
			}
		}
		case MenuChoose_AutoSelect:
		{
			chat_print(id, "%L", LANG_SERVER, "PUG_TEAMS_AUTO");

			SetHookChainReturn(ATYPE_INTEGER, 0)
			return HC_BREAK
		}
		case MenuChoose_T,  MenuChoose_CT:
		{
			if (getPlayersTeam(TeamName:iNewTeam) >= get_pug_maxplayers()/2 )
			{
				if (is_user_bot(id))
				{
					SetHookChainReturn(ATYPE_INTEGER, 0)
					return HC_BREAK
				}

				if (getPlayersTeam(TeamName:iNewTeam, false) >= get_pug_maxplayers()/2 )
				{
					chat_print(id, "%L", LANG_SERVER, "PUG_TEAMS_FULL", g_szTeams[TeamName:iNewTeam]);

					SetHookChainReturn(ATYPE_INTEGER, 0)
					return HC_BREAK
				}
			}

			fnReady(id)
		}
	}
			
	set_task(5.0, "show_owners", id, _, _, "a", 1) 
	
	return HC_CONTINUE;
}

// --------------------- Message in the engine ---------------------

public fnTeamScore(m, e, id)
{
	static _____team_score[2]
	get_msg_arg_string(1, _____team_score, charsmax(_____team_score))
	switch(_____team_score[0])
	{
		case 'T' : set_msg_arg_int(2, ARG_SHORT, g_iRoundTT)
		case 'C' : set_msg_arg_int(2, ARG_SHORT, g_iRoundCT)
	}
}

public fnScoreInfo(m, s, id)
{
	static _score_player_id
	_score_player_id = get_msg_arg_int(1)

	if (game_is_live())
	{
		set_msg_arg_int(2, ARG_SHORT, g_iFrags[_score_player_id])
		set_msg_arg_int(3, ARG_SHORT, g_iDeaths[_score_player_id])
	}
}

// --------------------- Game events ---------------------

public player_give_money (id, amount) set_member(id, m_iAccount, amount);

public money_handler (const id)
{
	if (!game_is_live())
	{
		player_give_money(id, 16000);
		return PLUGIN_HANDLED;
	}

	return PLUGIN_CONTINUE;
}

public damage_handler (iVictim)
{
	static iAttacker; iAttacker = get_user_attacker(iVictim)
	static iDamage; iDamage = read_data(2)

	if (iAttacker != iVictim && (1 <= iAttacker <= g_iMaxClients) && (1 <= iVictim <= g_iMaxClients))
	{
		g_iDmg[iVictim][iAttacker] += iDamage
		g_iHits[iVictim][iAttacker]++
	}

	return PLUGIN_CONTINUE;
}

// --------------------- Partes del PUG ---------------------

public PugWarmup ()
{
	g_iStage = STAGE_WARMUP

	g_iReadyCount = 0
	arrayset(g_bReady, false, sizeof(g_bReady))

	// fnUpdateServerName();

	new iPlayers[MAX_PLAYERS], iNum, iPlayer;
	get_players(iPlayers, iNum, "ch");
	
	for (new i; i < iNum; i++) 
	{	
		iPlayer = iPlayers[i]

		if (fnIsTeam(iPlayer))
			fnReady(iPlayer)
	}

	fnResetScores()
	fnResetFrags()

	if (g_iReadyCount < get_pug_maxplayers())
		set_task(1.0, "fnKeepReady", TASK_HUD_READY, _, _, "b")

	fnPregameHooks()

	client_cmd(0, "-showscores")
	DisableHookChain(g_hPlayerPostThink)

	fnRemoveHudMoney()

	exec_warmup();
}

public PugStart () {
	g_iStage = STAGE_START

	g_iVoteId = 0

	fnNextVote()
}

public PugFirstHalf(){
	g_iStage = STAGE_FIRSTHALF

	// fnTeamsRandomize()

	exec_pugmode();

	fnPugHooks()
	chat_print(0, "%L", LANG_SERVER, "PUG_STARTING_FIRSTHALF")
}

public PugHalftime(){
	g_iStage = STAGE_HALFTIME

	exec_halftime();

	EnableHookChain(g_hPlayerPostThink)
	fnPregameHooks()

	chat_print(0, "%L", LANG_SERVER, "PUG_GAME_INTERMISSION", get_halftime())

	if (g_iRound < get_maxrounds())
		set_task(float(get_halftime()), "PugSecondHalf", _, _, _, "a", 1);

	else
		set_task(float(get_halftime()), "PugOvertime", _, _, _, "a", 1);
}

public PugSecondHalf(){
	g_iStage = STAGE_SECONDHALF

	client_cmd(0, "-showscores")
	DisableHookChain(g_hPlayerPostThink)
	fnPugHooks()
	fnChangeTeams()

	chat_print(0, "%L", LANG_SERVER, "PUG_STARTING_SECONDHALF")
	exec_pugmode();
}

public PugOvertime()
{
	g_iStage = STAGE_OVERTIME

	client_cmd(0, "-showscores")
	DisableHookChain(g_hPlayerPostThink)
	fnPugHooks()
	fnChangeTeams()

	chat_print(0, "%L", LANG_SERVER, "PUG_STARTING_OVERTIME")
	exec_overtime();
}

public PugFinished(WinStatus:status)
{
	g_iStage = STAGE_FINISHED
	
	set_task(float(get_endtime()), "PugWarmup", _, _, _, "a", 1); 

	exec_finished();

	EnableHookChain(g_hPlayerPostThink)
	// fnUpdateLastMaps()

	switch (status)
	{
		case WINSTATUS_TERRORISTS:
			chat_print(0, "%L", LANG_SERVER, "PUG_GAMEOVER_WON", g_szTeams[TEAM_TERRORIST], g_iRoundTT, g_iRoundCT)

		case WINSTATUS_CTS:
			chat_print(0, "%L", LANG_SERVER, "PUG_GAMEOVER_WON", g_szTeams[TEAM_CT], g_iRoundCT, g_iRoundTT)

		case WINSTATUS_DRAW:
			chat_print(0, "%L", LANG_SERVER, "PUG_GAMEOVER_TIED", g_iRoundCT, g_iRoundTT)
	}
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
			fnStartingGame()
		}
	}
}

public fnStartingGame()
{
	switch(g_iStage)
	{
		case STAGE_START:
		{
			PugFirstHalf();
		}
		case STAGE_HALFTIME:
		{
			if (g_iRound < get_maxrounds())
				PugSecondHalf();
			else
				PugOvertime();
		}
	}
}

public fnStartVoteMap()
{
	arrayset(g_iMapVotes, 0, sizeof(g_iMapVotes))

	new iPlayers[MAX_PLAYERS], iNum, iPlayer;
	get_players(iPlayers, iNum, "ch");
	
	for (new i;i < iNum;i++) 
	{
		iPlayer = iPlayers[i];

		if (fnIsTeam(iPlayer))
			menu_display(iPlayer, g_mMap);

	}

	set_task(0.0, "fnVoteListMap", TASK_HUD_VOTE, _, _, "b")
	set_task(float(get_votedelay()), "fnVoteMapEnd", _, _, _, "a", 1)
}

public fnVoteListMap()
{
	new count, hud[512], temp

	for (new i = 0 ; i < g_iMapCount; i++)
	{
		temp = g_iMapVotes[i]
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

	fnMakeHudTitle(0, "%L", LANG_SERVER, "PUG_VOTING_MAP")
	fnMakeHudBody(0, hud)
}

public fnMapMenuHandle(const id, iMenu, iItem)
{
	if (iItem == MENU_EXIT)
		return PLUGIN_HANDLED;

	g_iMapVotes[iItem]++
	g_iVotesCount++
	fnVoteListMap()

	return PLUGIN_HANDLED;
}

public fnVoteMapEnd()
{
	set_votemap_ready(true);

	remove_task(TASK_HUD_VOTE)

	// Cancelar menu
	new iPlayers[MAX_PLAYERS], iNum, iPlayer;
	get_players(iPlayers, iNum, "ch");
	
	for (new i;i < iNum;i++) 
	{
		iPlayer = iPlayers[i];
		menu_cancel(iPlayer)
	}

	show_menu(0, 0, "^n", 1)

	// Obtener ganador
	new winner, temp
	for (new i = 0 ; i < sizeof (g_iMapVotes) ; i++)
	{
		if (temp < g_iMapVotes[i])
		{
			temp = g_iMapVotes[i]
			winner = i
		}
	}

	if (winner == 0)
	{
		chat_print(0, "%L", LANG_SERVER, "PUG_MAP_CURRENT")
		fnNextVote();
		return
	}

	else if (winner == 1)
		winner = random_num(1, 7)

	new sMapName[32]
	formatex(sMapName, charsmax(sMapName), "%s", g_sMapNames[winner])

	chat_print(0, "%L", LANG_SERVER, "PUG_MAP_CHANGE", sMapName)
	set_task(4.0, "fnChangeLevel", _, sMapName, charsmax(sMapName), "a", 1)
}

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

		if (fnIsTeam(iPlayer))
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

	fnMakeHudTitle(0, "%L", LANG_SERVER, "PUG_VOTING_TEAM")
	fnMakeHudBody(0, hud)
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

	// Cancelar menu
	new iPlayers[MAX_PLAYERS], iNum, iPlayer;
	get_players(iPlayers, iNum, "ch");
	
	for (new i;i < iNum;i++) 
	{
		iPlayer = iPlayers[i];
		menu_cancel(iPlayer)
	}

	show_menu(0, 0, "^n", 1)

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

		case 1:
		{
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

// --------------------- AFK kicker ---------------------

public fnCheckPlayers() { 
	for (new i = 1; i <= g_iMaxClients; i++) { 
		if (is_user_alive(i) && is_user_connected(i) && !is_user_bot(i) && !is_user_hltv(i) && g_bSpawned[i])
		{ 
			new newangle[3] 
			get_user_origin(i, newangle) 

			if ( newangle[0] == g_iOldAngles[i][0] && newangle[1] == g_iOldAngles[i][1] && newangle[2] == g_iOldAngles[i][2] )
			{ 
				g_iAfkTime[i] += CHECK_FREQ
				fnCheckAfkTime(i)
			}
			else
			{
				g_iOldAngles[i][0] = newangle[0]
				g_iOldAngles[i][1] = newangle[1]
				g_iOldAngles[i][2] = newangle[2]
				g_iAfkTime[i] = 0
			}
		}
	}
}

public fnCheckAfkTime(id)
{
	if (!game_is_live())
		return;

	new iMaxAfkTime = get_afktime();

	if (iMaxAfkTime < MIN_AFK_TIME)
	{
		log_amx("%L", LANG_SERVER, "PUG_AFKKICKER_CVAR", iMaxAfkTime, MIN_AFK_TIME)
		iMaxAfkTime = MIN_AFK_TIME
		set_afktime(MIN_AFK_TIME);
	}

	if (iMaxAfkTime-WARNING_TIME <= g_iAfkTime[id] < iMaxAfkTime)
	{
		new timeleft = iMaxAfkTime - g_iAfkTime[id]
		chat_print(id, "%L", LANG_SERVER, "PUG_AFKKICKER_WARN", timeleft)
	}
	else if (g_iAfkTime[id] > iMaxAfkTime)
	{
		new szName[32]
		get_user_name(id, szName, 31)
		chat_print(0, "%L", LANG_SERVER, "PUG_AFKKICKER_KICK", szName, iMaxAfkTime)
		log_amx("%s was kicked for being AFK longer than %i seconds", szName, iMaxAfkTime)
		server_cmd("kick #%d ^"%L^"", get_user_userid(id), LANG_SERVER, "PUG_AFKKICKER_KICK2", iMaxAfkTime)
	}
}

public fnDelayedSpawn(sid[])
{
	new id = sid[0];

	get_user_origin(id, g_iOldAngles[id]);
	g_bSpawned[id] = true;
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

		if (!fnIsTeam(iPlayer))
			continue;

		iLen = 0
		sMessage[0] = '^0'

		fnMakeHudTitle(iPlayer, sTitle);
		get_players(iPlayersTeam, iNumTeam, "eh", client_get_team(iPlayer) == TEAM_TERRORIST ? "TERRORIST" : "CT");

		for (new e; e < iNumTeam; e++)
		{
			iPlayerTeam = iPlayersTeam[e]

			iMoney = get_member(iPlayerTeam, m_iAccount);
			get_user_name(iPlayerTeam, sName, charsmax(sName));
			iLen += format(sMessage[iLen], charsmax(sMessage) - iLen, "%s: $%d^n", sName, iMoney);
		}

		fnMakeHudBody(iPlayer, sMessage);
	}
}

public fnRemoveHudMoney()
{
	if (task_exists(TASK_DISPLAY_INFO))
		remove_task(TASK_DISPLAY_INFO)
}

// --------------------- Utilidades ---------------------

public fnLoadMaps(const sPatch[])
{	
	g_mMap = menu_create("\gVotacion de mapa", "fnMapMenuHandle")

	if (file_exists(sPatch))
	{
		new iFile = fopen(sPatch, "rb");
		
		new sMap[32], iNum[10];

		// Mapa actual
		formatex(g_sMapNames[g_iMapCount], charsmax(g_sMapNames[]), "%L", LANG_SERVER, "PUG_VOTING_MAPCURRENT");
			
		num_to_str(g_iMapCount, iNum, charsmax(iNum));
		menu_additem(g_mMap, g_sMapNames[g_iMapCount], iNum);
	
		g_iMapCount++;
	
		// Mapa aleatorio
		formatex(g_sMapNames[g_iMapCount], charsmax(g_sMapNames[]), "%L", LANG_SERVER, "PUG_VOTING_RANDOM");
			
		num_to_str(g_iMapCount, iNum, charsmax(iNum));
		menu_additem(g_mMap, g_sMapNames[g_iMapCount], iNum);
	
		g_iMapCount++;

		while(!feof(iFile) && (g_iMapCount < MAX_MAPS))
		{
			fgets(iFile, sMap, charsmax(sMap));
			trim(sMap);
			
			if ((sMap[0] != ';') && is_map_valid(sMap) && !equali(sMap, g_sCurrentMap))
			{
				copy(g_sMapNames[g_iMapCount], charsmax(g_sMapNames[]), sMap);
				num_to_str(g_iMapCount, iNum, charsmax(iNum));

				if ( is_lastmaps_blocked() && (equali(sMap, g_sLastMaps[0]) || equali(sMap, g_sLastMaps[1])) )
				{
					new text[32]
					formatex(text, charsmax(text), "\d%i. %s", g_iMapCount+1, sMap)

					#if AMXX_VERSION_NUM >= 183
					menu_addtext2(g_mMap, text)
					#else
					menu_addtext(g_mMap, text)
					#endif
				}

				else
					menu_additem(g_mMap, sMap, iNum);
			
				g_iMapCount++;
			}
		}
		
		fclose(iFile);
		
		return g_iMapCount;
	}
	
	return 0;
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

	if ( equali(szArgs, "") )
		return true; 
	
	return false;
}

public fnSendMessage(id, Colors:color, msg[192])
{
	if (is_msgsound_allowed())
		client_cmd(id, "spk buttons/lightswitch2")

	message_begin(MSG_ONE_UNRELIABLE, get_user_msgid("SayText"), _, id);
	write_byte(color ? (_:color) : 33);
	write_string(msg);
	message_end();
}

public fnReady(const id)
{
	if (g_bReady[id] || is_user_bot(id))
		return;

	g_bReady[id] = true;
	g_iReadyCount++;

	fnCheckReady();
}

public fnNotReady(const id)
{
	if (!g_bReady[id])
		return;

	g_bReady[id] = false;
	g_iReadyCount--;

	fnCheckReady();
}

public fnForceStart(const id)
{
	if (!is_user_admin(id))
	{
		chat_print(id, "%L", LANG_SERVER, "PUG_ACTION_NOTACCESS")
		return PLUGIN_HANDLED;
	}

	if (game_is_live())
	{
		chat_print(id, "%L", LANG_SERVER, "PUG_ACTION_NOTALLOWED")
		return PLUGIN_HANDLED;
	}

	new sName[MAX_NAME_LENGTH]
	get_user_name(id, sName, charsmax(sName))
	chat_print(0, "%L", LANG_SERVER, "PUG_FORCE_GAME", sName)

	fnDisplayReady(0.0)		
	remove_task(TASK_HUD_READY)

	switch(g_iStage)
	{
		case STAGE_WARMUP:
			PugStart();

	}

	return PLUGIN_HANDLED;
}

public fnForceCancel(const id)
{
	if (!is_user_admin(id))
	{
		chat_print(id, "%L", LANG_SERVER, "PUG_ACTION_NOTACCESS");
		return PLUGIN_HANDLED;
	}
	else if (!game_is_live())
	{
		chat_print(id, "%L", LANG_SERVER, "PUG_ACTION_NOTALLOWED");
		return PLUGIN_HANDLED;
	}

	new sName[MAX_NAME_LENGTH]
	get_user_name(id, sName, charsmax(sName))
	chat_print(0, "%L", LANG_SERVER, "PUG_FORCE_CANCEL", sName)

	PugWarmup();

	return PLUGIN_HANDLED;
}

public fnManualON (id) {
	if (!is_user_admin(id))
	{
		chat_print(id, "%L", LANG_SERVER, "PUG_ACTION_NOTACCESS");
		return PLUGIN_HANDLED;
	}

	new sName[MAX_NAME_LENGTH]
	get_user_name(id, sName, charsmax(sName))
	chat_print(0, "%L", LANG_SERVER, "PUG_CHANGE_TO_MANUAL", sName);

	set_server_manual();
	
	return PLUGIN_HANDLED;
}

public fnManualOFF (id) {
	if (!is_user_admin(id))
	{
		chat_print(id, "%L", LANG_SERVER, "PUG_ACTION_NOTACCESS");
		return PLUGIN_HANDLED;
	}

	new sName[MAX_NAME_LENGTH]
	get_user_name(id, sName, charsmax(sName))
	chat_print(0, "%L", LANG_SERVER, "PUG_CHANGE_TO_AUTO", sName);

	set_server_auto();
	
	return PLUGIN_HANDLED;
}

public fnMode (id) 
{
	if (!is_user_admin(id))
	{
		chat_print(id, "%L", LANG_SERVER, "PUG_ACTION_NOTACCESS");
		return;
	}

	if (is_server_manual())
		chat_print(0, "%L", LANG_SERVER, "PUG_MODE_MANUAL");
	else
		chat_print(0, "%L", LANG_SERVER, "PUG_MODE_AUTO");
}

public fnCheckReady()
{
	if (g_iStage == STAGE_WARMUP)
	{
		if (g_iReadyCount == get_pug_maxplayers() && !is_server_manual() )
		{
			fnDisplayReady(0.0)		
			remove_task(TASK_HUD_READY)
			
			PugStart();
		}

		else
			set_task(1.0, "fnKeepReady", TASK_HUD_READY, _, _, "b")

	}
}

public fnDisplayReady(Float:fHoldTime)
{
	if ( !is_server_manual() )
	{
		set_hudmessage(0, 255, 0, 0.8, 0.07, 0, 0.0, fHoldTime, 0.0, 0.0, 1)

		new iNeed = get_pug_maxplayers() - g_iReadyCount

		if (iNeed > 1)
			show_hudmessage(0, "%L", LANG_SERVER, "PUG_PLAYERS_MISSING", iNeed)
		else
			show_hudmessage(0, "%L", LANG_SERVER, "PUG_PLAYERS_MISSING2", iNeed)
	}
	else 
	{
			show_hudmessage(0, "");
	}
}

public fnKeepReady()
{
	fnDisplayReady(999.9)
}

public fnMakeHudTitle(const id, msg[], any:...)
{
	new fmt[50]
	vformat(fmt, charsmax(fmt), msg, 3)
	set_hudmessage(0, 255, 0, 0.23, 0.02, 0, 1.0, 1.1, 0.0, 0.0, 1)
	show_hudmessage(id, fmt);
}
public fnMakeHudBody(const id, msg[], any:...)
{
	new fmt[512]
	vformat(fmt, charsmax(fmt), msg, 3)
	set_hudmessage(255, 255, 255, 0.23, 0.05, 0, 1.0, 1.1, 0.0, 0.0, 2)
	show_hudmessage(id, fmt);
}

public fnChangeLevel(sMap[])
{
	#if AMXX_VERSION_NUM >= 183
	engine_changelevel(sMap);
	#else
	server_cmd("changelevel %s", sMap);
	#endif
}

public bool:fnIsTeam(id)
{
	if (TEAM_TERRORIST <= client_get_team(id) <= TEAM_CT)
		return true;

	return false;
}

public fnDamage(const id)
{
	new TeamName:iTeam = client_get_team(id);

	if (!game_is_live() || is_user_alive(id) || iTeam == TEAM_SPECTATOR)
	{
		chat_print(id, "%L", LANG_SERVER, "PUG_ACTION_NOTALLOWED")
		return PLUGIN_HANDLED;
	}

	chat_print(id, "%L", LANG_SERVER, "PUG_DMG_TITLE")

	new iPlayers[MAX_PLAYERS], iNum, iVictim, szName[10];
	get_players(iPlayers, iNum, "h");
	
	for (new i;i < iNum;i++)
	{
		iVictim = iPlayers[i];
		
		if (g_iDmg[iVictim][id] > 0)
		{
			get_user_name(iVictim, szName, charsmax(szName));
			chat_print(id, "%L", LANG_SERVER, "PUG_DMG_INFO", szName, g_iDmg[iVictim][id], g_iHits[iVictim][id])
		}
	}
	
	if (!szName[0])
		chat_print(id, "%L", LANG_SERVER, "PUG_DMG_DIDNTHURT")
	
	return PLUGIN_HANDLED;
}

public fnDamageAuto(const id)
{
	console_print(id, "%L", LANG_SERVER, "PUG_DMG_TITLE")

	new iPlayers[MAX_PLAYERS], iNum, iVictim;
	get_players(iPlayers, iNum, "h");
	
	new szName[MAX_NAME_LENGTH];
	
	for (new i;i < iNum;i++)
	{
		iVictim = iPlayers[i];
		
		if (g_iDmg[iVictim][id] > 0)
		{
			get_user_name(iVictim, szName, charsmax(szName));
			console_print(id, "%L", LANG_SERVER, "PUG_DMG_INFO", szName, g_iDmg[iVictim][id], g_iHits[iVictim][id])
		}
	}
	
	if (!szName[0])
		console_print(id, "%L", LANG_SERVER, "PUG_DMG_DIDNTHURT")

	console_print(id, "%L", LANG_SERVER, "PUG_DMG_TITLE")

}

public fnResetDmg()
{
	for (new i; i < g_iMaxClients+1; i++)
	{
		arrayset(g_iDmg[i], 0, sizeof(g_iDmg))
		arrayset(g_iHits[i], 0, sizeof(g_iHits))
	}
}

public fnHp(id) 
{
	if (game_is_live() && !is_user_alive(id) && fnIsTeam(id))
	{
		chat_print(id, "%L", LANG_SERVER, "PUG_HP_TITLE")

		new iPlayers[MAX_PLAYERS], iNum, iPlayer;
		get_players(iPlayers, iNum, "ah");
		
		new szName[MAX_NAME_LENGTH];
		
		for (new i;i < iNum;i++)
		{
			iPlayer = iPlayers[i];

			if (is_user_alive(iPlayer))
			{
				get_user_name(iPlayer, szName, charsmax(szName))
				chat_print(id, "%L", LANG_SERVER, "PUG_HP_INFO", szName, get_user_health(iPlayer))
			}
		}
		
		if (!szName[0])
			chat_print(id, "%L", LANG_SERVER, "PUG_HP_DIDNTLIVE")

	}
	else
		chat_print(id, "%L", LANG_SERVER, "PUG_ACTION_NOTALLOWED")

}

public fnShowMoney(id) 
{
	if (game_is_live() && fnIsTeam(id))
	{
		chat_print(id, "%L", LANG_SERVER, "PUG_MONEY_TITLE")

		new szName[MAX_NAME_LENGTH];
		new iPlayers[MAX_PLAYERS], iNum, iPlayer;
		get_players(iPlayers, iNum, "eh", client_get_team(id) == TEAM_TERRORIST ? "TERRORIST" : "CT");
		
		for (new i;i < iNum;i++)
		{
			iPlayer = iPlayers[i];
			
			if (iPlayer == id)
				continue;

			get_user_name(iPlayer, szName, charsmax(szName))
			chat_print(id, "%L", LANG_SERVER, "PUG_MONEY_INFO", get_member(iPlayer, m_iAccount), szName)
		}
	}

	else
		chat_print(id, "%L", LANG_SERVER, "PUG_ACTION_NOTALLOWED")

}

public fnChangeTeams()
{
	chat_print(0, "%L", LANG_SERVER, "PUG_TEAM_CHANGING")

	new iPlayersTT[MAX_PLAYERS], iNumTT;
	new iPlayersCT[MAX_PLAYERS], iNumCT;

	// Get actual teams
	get_players(iPlayersTT, iNumTT, "eh", "TERRORIST");
	get_players(iPlayersCT, iNumCT, "eh", "CT");

	// Terrorist -> CT

	for (new i; i < iNumTT; i++)
	{
		rg_set_user_team(iPlayersTT[i], TEAM_CT);
	}

	// CT -> Terrorist

	for (new i; i < iNumCT; i++)
	{
		rg_set_user_team(iPlayersCT[i], TEAM_TERRORIST);
	}

	// Change Rounds
	new temp = g_iRoundCT
	g_iRoundCT = g_iRoundTT
	g_iRoundTT = temp
}

public fnResetScores()
{
	g_iRound = 0
	g_iRoundCT = 0
	g_iRoundTT = 0
}

public fnResetFrags()
{
	arrayset(g_iFrags, 0, sizeof(g_iFrags))
	arrayset(g_iDeaths, 0, sizeof(g_iDeaths))
}

public fnShowScore()
{
	if (!game_is_live())
		return;

	if (g_iRoundCT == g_iRoundTT)
		chat_print(0, "%L", LANG_SERVER, "PUG_SCORE_TIED", g_iRoundCT, g_iRoundTT)

	else if (g_iRoundCT > g_iRoundTT)
		chat_print(0, "%L", LANG_SERVER, "PUG_SCORE_WINNING", g_szTeams[TEAM_CT], g_iRoundCT, g_iRoundTT)

	else
		chat_print(0, "%L", LANG_SERVER, "PUG_SCORE_WINNING", g_szTeams[TEAM_TERRORIST], g_iRoundTT, g_iRoundCT)

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
	
	formatex(g_sPauseOptions[0], charsmax(g_sPauseOptions[]), "%L", LANG_SERVER, "PUG_VOTING_YES");
	formatex(g_sPauseOptions[1], charsmax(g_sPauseOptions[]), "%L", LANG_SERVER, "PUG_VOTING_NO");

	// Get maps allowed
	new sPatch[40];
	get_configdir(sPatch, charsmax(sPatch));
	format(sPatch, charsmax(sPatch), "%s/maps.ini", sPatch);

	if (!fnLoadMaps(sPatch))
	{
		get_mapcycle_file(sPatch, charsmax(sPatch));
		fnLoadMaps(sPatch);
	}
}

public show_owners(const id)
{
	new sOwner[256]
	get_owner(sOwner, charsmax(sOwner));

	if (!equali(sOwner, "")) {
		if (equali(g_sLang, "es"))
			chat_print(id, "Lider del servidor: !t%s", sOwner);
		else
			chat_print(id, "Server leader: !t%s", sOwner);
	}
}

// PAUSE VOTE

public fnStartVotePause(id)
{
	new TeamName:iTeam = client_get_team(id);

	if ( !(TEAM_TERRORIST <= iTeam <= TEAM_CT) )
	{
		chat_print(id, "%L", LANG_SERVER, "PUG_PAUSE_SPECTATORS");
		return;
	}

	chat_print(0, "%L", LANG_SERVER, "PUG_PAUSE_START", LANG_SERVER, iTeam == TEAM_TERRORIST ? "TERRORIST" :  "CT")

	arrayset(g_iPauseVotes, 0, sizeof(g_iPauseVotes))
	g_iPauseCount = 0
	g_mPause = menu_create("\gPausar proxima ronda?", "fnPauseMenuHandle")

	new sNum[10]

	for (new i; i < sizeof(g_sPauseOptions); i++)
	{
		num_to_str(g_iPauseCount, sNum, charsmax(sNum));
		menu_additem(g_mPause, g_sPauseOptions[i], sNum);
		g_iPauseCount++
	}

	menu_setprop(g_mPause, MPROP_EXIT, MEXIT_NEVER);

	new iPlayers[MAX_PLAYERS], iNum, iPlayer;
	get_players(iPlayers, iNum, "ch");

	for (new i; i < iNum;i++) 
	{
		iPlayer = iPlayers[i];

		if (client_get_team(iPlayer) == iTeam)
			menu_display(iPlayer, g_mPause);

	}

	set_task(float(get_votedelay()), "fnVotePauseEnd", _:iTeam, _, _, "a", 1)
}

public fnPauseMenuHandle(const id, iMenu, iItem)
{
	if (iItem == MENU_EXIT)
		return PLUGIN_HANDLED;

	g_iPauseVotes[iItem]++
	g_iVotesCount++

	return PLUGIN_HANDLED;
}

public fnVotePauseEnd(TeamName:iTeam)
{
	remove_task(TASK_HUD_VOTE)

	// Cancelar menu
	new iPlayers[MAX_PLAYERS], iNum, iPlayer;
	get_players(iPlayers, iNum, "ceh", iTeam == TEAM_TERRORIST ? "TERRORIRST" :  "CT");
	
	for (new i;i < iNum;i++) 
	{
		iPlayer = iPlayers[i];
		menu_cancel(iPlayer)
	}

	show_menu(0, 0, "^n", 1)

	// Obtener ganador
	new winner, temp

	for (new i = 0 ; i < sizeof (g_iPauseVotes) ; i++)
	{
		if (temp < g_iPauseVotes[i])
		{
			temp = g_iPauseVotes[i]
			winner = i
		}
	}

	switch (winner)
	{
		case 0:
		{
			chat_print(0, "%L", LANG_SERVER, "PUG_PAUSE_NEXT_ROUND")
			set_freezetime(45);
		}

		case 1:
		{
			for (new i;i < iNum;i++) 
			{
				iPlayer = iPlayers[i];
				chat_print(iPlayer, "%L", LANG_SERVER, "PUG_PAUSE_FAILED")
			}
		}
	}
}

public fnUpdateLastMaps()
{
	// Update last maps
	formatex(g_sLastMaps[1], charsmax(g_sLastMaps[]), "%s", g_sLastMaps[0])
	formatex(g_sLastMaps[0], charsmax(g_sLastMaps[]), "%s", g_sCurrentMap)

	set_lastmaps(g_sLastMaps);
}	

public fnMute(const id, level, cid)
{
	if (!cmd_access(id, level, cid, 1))
	{
		chat_print(id, "%L", LANG_SERVER, "PUG_VOTEKICK_SPECIFY");
		return PLUGIN_HANDLED;
	}

	new target, targetName[32];
	read_argv(1, targetName, 31);
	target = cmd_target(id, targetName, CMDTARGET_OBEY_IMMUNITY | CMDTARGET_NO_BOTS | CMDTARGET_ALLOW_SELF);

	if (!target)
	{
		chat_print(id, "%L", LANG_SERVER, "PUG_VOTEKICK_UNAVAILABLE")
		return PLUGIN_HANDLED;
	}

	g_bMuted[id][target] = true	

	return PLUGIN_HANDLED
}

public fnUnmute(const id, level, cid)
{
	if (!cmd_access(id, level, cid, 1))
	{
		chat_print(id, "%L", LANG_SERVER, "PUG_VOTEKICK_SPECIFY");
		return PLUGIN_HANDLED
	}

	new target, targetName[32];
	read_argv(1, targetName, 31);
	target = cmd_target(id, targetName, CMDTARGET_OBEY_IMMUNITY | CMDTARGET_NO_BOTS | CMDTARGET_ALLOW_SELF);

	if (!target)
	{
		chat_print(id, "%L", LANG_SERVER, "PUG_VOTEKICK_UNAVAILABLE")
		return PLUGIN_HANDLED
	}

	g_bMuted[id][target] = false

	return PLUGIN_HANDLED
}

public fnPregameHooks()
{
	EnableHookChain(g_hDeadPlayerWeapons)
	EnableHookChain(g_hGiveC4)
	DisableHookChain(g_hRoundFreezeEnd)
	DisableHookChain(g_hPlayerKilled)
	//DisableHookChain(g_hTakeDamage)
}

public fnPugHooks()
{
	EnableHookChain(g_hRoundFreezeEnd)
	EnableHookChain(g_hPlayerKilled)
	//EnableHookChain(g_hTakeDamage)
	DisableHookChain(g_hDeadPlayerWeapons)
	DisableHookChain(g_hGiveC4)
}

/*
public fnUpdateServerName()
{
	new szFmt[32];

	if (game_is_live())
	{
		if (g_iRoundCT == g_iRoundTT)
			formatex(szFmt, charsmax(szFmt), "Ronda: %i | Empate: %i - %i", g_iRound, g_iRoundCT, g_iRoundTT)

		else if (g_iRoundCT > g_iRoundTT)
			formatex(szFmt, charsmax(szFmt), "Ronda: %i | CT: %i | TT: %i", g_iRound, g_iRoundCT, g_iRoundTT)

		else
			formatex(szFmt, charsmax(szFmt), "Ronda: %i | TT: %i | CT: %i", g_iRound, g_iRoundTT, g_iRoundCT)

	}
	else 
	{
		formatex(szFmt, charsmax(szFmt), "Funny Hosting");
	}

	set_member_game(m_GameDesc, szFmt);
}
*/

// DISPLAY NAME

public event_new_round()
{
	if (game_is_live())
	{
		new showMoneyMode = get_showmoney_mode();

		switch (showMoneyMode)
		{
			case 1:
			{
				new iPlayers[MAX_PLAYERS], iNum, iPlayer
				get_players(iPlayers, iNum, "h");

				for (new i; i < iNum; i++)
				{
					iPlayer = iPlayers[i]

					if (!fnIsTeam(iPlayer))
						continue;
				
					client_cmd(iPlayer, "say_team $%i", get_member(iPlayer, m_iAccount))
				}
			}
			case 2:
			{
				remove_task(TASK_DISPLAY_INFO);

				set_task(0.2, "fnHudMoney", TASK_DISPLAY_INFO, _, _, "b")
				set_task(float(get_freezetime()), "fnRemoveHudMoney", _, _, _, "a", 1)     // Give the player time to drop to the floor when spawning
			}
			case 3:
			{
				show_team_equipment();
			}
		}
	}
}
