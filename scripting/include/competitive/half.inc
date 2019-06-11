static maxrounds;
static bool:firstround;

stock firsthalf () {
	g_iStage = STAGE_FIRSTHALF
	autoready_hide();
	maxrounds = get_maxrounds();

	chat_print(0, "%L", LANG_SERVER, "PUG_STARTING_FIRSTHALF")

	halfstart();
	exec_pugmode();
}

static secondhalf () {
	g_iStage = STAGE_SECONDHALF

	chat_print(0, "%L", LANG_SERVER, "PUG_STARTING_SECONDHALF")

	halfstart(true);
	exec_pugmode();
}

static firsthalf_ot () {
	g_iStage = STAGE_FIRSTOT
	maxrounds += get_overtime_rounds();

	chat_print(0, "%L", LANG_SERVER, "PUG_STARTING_OVERTIME")

	halfstart(true);
	exec_overtime();
}

static secondhalf_ot () {
	g_iStage = STAGE_SECONDOT

	chat_print(0, "%L", LANG_SERVER, "PUG_STARTING_OVERTIME")

	halfstart(true);
	exec_overtime();
}

stock game_finish (WinStatus:status) {
	static ttscore, ctscore;

	ctscore = teamct_get_score();
	ttscore = teamtt_get_score();

	set_task(float(get_endtime()), "PugWarmup", _, _, _, "a", 1); 

	exec_finished();

	EnableHookChain(g_hPlayerPostThink)
	// fnUpdateLastMaps()

	switch (status) {
		case WINSTATUS_TERRORISTS:
			chat_print(0, "%L", LANG_SERVER, "PUG_GAMEOVER_WON", team_name[TEAM_TERRORIST], ttscore, ctscore)
		case WINSTATUS_CTS:
			chat_print(0, "%L", LANG_SERVER, "PUG_GAMEOVER_WON", team_name[TEAM_CT], ctscore, ttscore)
		default:
			chat_print(0, "%L", LANG_SERVER, "PUG_GAMEOVER_TIED", ctscore, ttscore)
	}
}

// Round end checks

static halfstart (bool:remove_break=false) {
	fnPugHooks();
	firstround = true;

	if (remove_break) {
		client_cmd(0, "-showscores");
		DisableHookChain(g_hPlayerPostThink);
		teams_switch();
	}
}

static gamebreak () {
	EnableHookChain(g_hPlayerPostThink)
	fnPregameHooks()

	chat_print(0, "%L", LANG_SERVER, "PUG_GAME_INTERMISSION", get_halftime())

	set_task(float(get_halftime()), "gamebreak_end", _, _, _, "a", 1);

	exec_halftime();
}

public gamebreak_end () {
	switch (g_iStage) {
		case STAGE_FIRSTHALF:
			secondhalf();
		case STAGE_SECONDHALF:
			firsthalf_ot();
		case STAGE_FIRSTOT:
			secondhalf_ot();
		case STAGE_SECONDOT:
			firsthalf_ot();
	}
}
			
static game_try_end () {
	if (teamtt_get_score() > maxrounds/2) {
		game_finish(WINSTATUS_TERRORISTS);
		return;
	} else if (teamct_get_score() > maxrounds/2) {
		game_finish(WINSTATUS_CTS);
		return;
	} else if (is_score_tie() && is_tie_allowed()) {
		game_finish(WINSTATUS_DRAW);
		return;
	}

	fnShowScore();
	
	if (is_score_tie() && !is_tie_allowed() && get_overtime_rounds())
		gamebreak();
}

stock check_halfend () {
	firstround = false;

	switch (g_iStage) {
		case STAGE_FIRSTHALF, STAGE_FIRSTOT: {
			fnShowScore();

			if (is_half_end())
				gamebreak();
		}
		case STAGE_SECONDHALF, STAGE_SECONDOT: {
			game_try_end();
		}
	}
}

static bool:is_score_tie ()
	return (teamtt_get_score() == maxrounds/2 &&
			teamct_get_score() == maxrounds/2);

static is_half_end () {
	new const total_score = teamtt_get_score() + teamct_get_score();

	if (total_score == maxrounds/2 || total_score == maxrounds)
		return true;

	return false;
}

stock bool:is_firstround ()
	return firstround;

