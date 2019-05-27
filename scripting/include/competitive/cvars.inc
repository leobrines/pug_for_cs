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

stock init_cvars () {
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

stock update_game_description () {
	new gameName[32];
	get_pcvar_string(g_pGameName, gameName, charsmax(gameName));                                                                             
	set_member_game(m_GameDesc, gameName);
}

stock get_mapcycle_file (file[], size)
	get_pcvar_string(g_pMapCycle, file, size);

stock get_pug_maxplayers ()
	return get_pcvar_num(g_pPlayers);

stock get_owner (owner[], size)
	get_pcvar_string(g_pPugOwner, owner, size);

stock get_afktime ()
	return get_pcvar_num(g_pAfkTime);

stock set_afktime (time)
	return set_pcvar_num(g_pAfkTime, time);

stock get_votekick_time ()
	return get_pcvar_num(g_pVoteTime);

stock get_votekick_percentage ()
	return get_pcvar_num(g_pVoteKickPerc);

stock get_max_absence_players ()
	return get_pcvar_num(g_pAbsencePlayers);

stock is_shield_blocked ()
	return get_pcvar_num(g_pBlockShield) ? true : false;

stock is_nvg_blocked ()
	return get_pcvar_num(g_pBlockNvgs) ? true : false;

stock are_grenades_blocked ()
	return get_pcvar_num(g_pBlockGrenades) ? true : false;

stock get_maxrounds () 
	return get_pcvar_num(g_pRoundsMax);

stock get_overtime_rounds () 
	return get_pcvar_num(g_pRoundsOT);

stock is_tie_allowed ()
	return get_pcvar_num(g_pAllowTie) ? true : false;

stock is_spectator_allowed ()
	return get_pcvar_num(g_pAllowSpec) ? true : false;

stock get_halftime ()
	return get_pcvar_num(g_pIntermissionTime);

stock get_endtime ()
	return get_pcvar_num(g_pDelayEnd);

stock is_votemap_allowed ()
	return get_pcvar_num(g_pVoteMap) ? true : false;

stock is_votemap_ready ()
	return get_pcvar_num(g_pVoteMapReady) ? true : false;

stock set_votemap_ready (bool:isReady) {
	if (isReady)
		set_pcvar_num(g_pVoteMapReady, 1)
	else
		set_pcvar_num(g_pVoteMapReady, 0)
}

stock is_voteteam_allowed ()
	return get_pcvar_num(g_pVoteTeam) ? true : false;

stock get_votedelay ()
	return get_pcvar_num(g_pVoteDelay);

stock is_lastmaps_blocked ()
	return get_pcvar_num(g_pBlockLastMaps) ? true : false;

stock is_msgsound_allowed ()
	return get_pcvar_num(g_pAllowSoundMsg) ? true : false;

stock exec_warmup ()
	exec_file(g_pWarmup);

stock exec_pugmode ()
	exec_file(g_pPugMode);

stock exec_halftime ()
	exec_file(g_pHalftime);

stock exec_overtime ()
	exec_file(g_pOvertime);

stock exec_finished ()
	exec_file(g_pFinished);
                                                                                                                                             
stock exec_file (hConvar) {
	new szFile[MAX_NAME_LENGTH];
	get_pcvar_string(hConvar, szFile, charsmax(szFile));

	if (szFile[0] != '^0') {
		new szDir[128];
		get_configdir(szDir, charsmax(szDir));
		format(szDir, charsmax(szDir), "%s/%s", szDir, szFile);

		server_cmd("exec %s", szDir);
	}
}

stock get_configdir (dir[], size) {
	get_localinfo("amxx_configsdir", dir, size);
	format(dir, size, "%s/pug", dir);
}

stock set_server_manual ()
	set_pcvar_num(g_pManual, 1);

stock set_server_auto ()
	set_pcvar_num(g_pManual, 0);

stock is_server_manual ()
	return get_pcvar_num(g_pManual) ? true : false;

stock get_prefix (prefix[], size)
	get_pcvar_string(g_pPugTag, prefix, size);

stock get_currentmap (map[], size) 
    get_mapname(map, size);

stock get_serverlang (lang[], size)
    get_cvar_string("amx_language", lang, size);

stock get_lastmaps (maps[2][32]) {
	get_pcvar_string(g_pLastMap, maps[0], charsmax(maps[]))
	get_pcvar_string(g_pLastMap2, maps[1], charsmax(maps[]))
}

stock set_lastmaps (maps[2][32]) {
	set_pcvar_string(g_pLastMap, maps[0]);
	set_pcvar_string(g_pLastMap2, maps[1]);
}

stock get_showmoney_mode () 
	return get_pcvar_num(g_pShowMoney);

stock get_freezetime () 
	return get_pcvar_num(g_pFreezeTime);

stock set_freezetime (num) 
	return set_pcvar_num(g_pFreezeTime, num);
