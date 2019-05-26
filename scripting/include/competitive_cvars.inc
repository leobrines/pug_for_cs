#include <amxmodx>

// Cfg files
new g_pWarmup
new g_pPugMode
new g_pHalftime
new g_pOvertime
new g_pFinished

// Pug configuration
new g_pPugTag
new g_pPugOwner
new g_pGameName
new g_pPlayers
new g_pBlockShield
new g_pBlockNvgs
new g_pBlockGrenades
new g_pBlockLastMaps
new g_pRoundsMax
new g_pRoundsOT
new g_pVoteDelay
new g_pVoteMap
new g_pVoteMapReady
new g_pVoteTeam
new g_pAllowTie
new g_pDelayEnd
new g_pLastMap
new g_pLastMap2
new g_pIntermissionTime
new g_pAbsencePlayers
new g_pVoteTime
new g_pVoteKickPerc
new g_pAfkTime
new g_pManual
new g_pShowMoney
new g_pAllowSoundMsg

new g_pAllowSpec
new g_pMapCycle
new g_pFreezeTime

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
