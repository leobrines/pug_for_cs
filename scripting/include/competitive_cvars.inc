// Cfg files
stock g_pWarmup
stock g_pPugMode
stock g_pHalftime
stock g_pOvertime
stock g_pFinished

// Pug configuration
stock g_pPugTag
stock g_pPugOwner
stock g_pGameName
stock g_pPlayers
stock g_pBlockShield
stock g_pBlockNvgs
stock g_pBlockGrenades
stock g_pBlockLastMaps
stock g_pRoundsMax
stock g_pRoundsOT
stock g_pVoteDelay
stock g_pVoteMap
stock g_pVoteMapReady
stock g_pVoteTeam
stock g_pAllowTie
stock g_pDelayEnd
stock g_pLastMap
stock g_pLastMap2
stock g_pIntermissionTime
stock g_pAbsencePlayers
stock g_pVoteTime
stock g_pVoteKickPerc
stock g_pAfkTime
stock g_pManual
stock g_pShowMoney
stock g_pAllowSoundMsg

stock g_pAllowSpec
stock g_pMapCycle
stock g_pFreezeTime

public create_pcvars () {
	g_pWarmup	= register_cvar("pug_config_warmup", "warmup.rc");
	g_pPugMode = register_cvar("pug_config_pugmode", "pugmode.rc");
	g_pHalftime = register_cvar("pug_config_halftime", "halftime.rc");
	g_pOvertime = register_cvar("pug_config_overtime", "overtime.rc");
	g_pFinished = register_cvar("pug_config_end", "end.rc");

	g_pPugTag = register_cvar("pug_tag", "[Server]")
	g_pPugOwner = register_cvar("pug_owner", "")
	g_pGameName = register_cvar("pug_gamename", "Competitivo")
	g_pPlayers = register_cvar("pug_players", "10");
	g_pBlockShield = register_cvar("pug_block_shield", "1");
	g_pBlockNvgs = register_cvar("pug_block_nvgs", "1");
	g_pBlockGrenades = register_cvar("pug_block_grenades", "1");
	g_pBlockLastMaps = register_cvar("pug_block_last_maps", "0");
	g_pRoundsMax = register_cvar("pug_rounds_max", "30");
	g_pRoundsOT = register_cvar("pug_rounds_ot", "6");
	g_pVoteMap = register_cvar("pug_vote_map", "1")
	g_pVoteMapReady = register_cvar("pug_vote_map_ready", "0"); // Esta cvar solo la controla el plugin, no los .rc
	g_pVoteDelay = register_cvar("pug_vote_delay", "15")
	g_pVoteTeam = register_cvar("pug_vote_team", "1")
	g_pAllowTie = register_cvar("pug_allow_tie", "0")
	g_pDelayEnd = register_cvar("pug_delay_end", "5.0")
	g_pLastMap = register_cvar("pug_last_map", ""); // Esta cvar solo la controla el plugin, no los .rc
	g_pLastMap2 = register_cvar("pug_last_map2", ""); // Esta cvar solo la controla el plugin, no los .rc
	g_pIntermissionTime = register_cvar("pug_intermission_time", "15")
	g_pAbsencePlayers = register_cvar("pug_absence_players", "2")
	g_pVoteTime = register_cvar("pug_vote_delay", "10");
	g_pVoteKickPerc = register_cvar("pug_votekick_percent", "0.7");
	g_pAfkTime = register_cvar("pug_afktime", "90");
	g_pManual = register_cvar("pug_manual", "0");
	g_pShowMoney = register_cvar("pug_show_money", "3");
	g_pAllowSoundMsg = register_cvar("pug_allow_soundmsg", "1");

	g_pAllowSpec = get_cvar_pointer("allow_spectators")
	g_pMapCycle = get_cvar_pointer("mapcyclefile");
	g_pFreezeTime = get_cvar_pointer("mp_freezetime");
}
