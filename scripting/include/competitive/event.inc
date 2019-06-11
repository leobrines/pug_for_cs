#if defined _event_included
  #endinput
#endif

#define _event_included

// Hook
new HookChain:g_hGiveC4
new HookChain:g_hPlayerPostThink
new HookChain:g_hRoundFreezeEnd
new HookChain:g_hHasRestrictItem
new HookChain:g_hRoundEnd
// new HookChain:g_hTakeDamage

stock event_init () {
	register_message(get_user_msgid("TeamScore"), "event_team_score")
	register_message(get_user_msgid("ScoreInfo"), "event_player_score")
	register_logevent("join_team", 3, "1=joined team")
}

public event_player_score (m, s, id) {
	static player_id;
	player_id = get_msg_arg_int(1);

	if (game_is_live()) {
		set_msg_arg_int(2, ARG_SHORT, client_get_frags(player_id));
		set_msg_arg_int(3, ARG_SHORT, client_get_deaths(player_id));
	}
}

public event_team_score (m, e, id) {
	static team_score[2];
	get_msg_arg_string(1, team_score, charsmax(team_score));

	switch(team_score[0]) {
		case 'T':
			set_msg_arg_int(2, ARG_SHORT, teamtt_get_score());
		case 'C':
			set_msg_arg_int(2, ARG_SHORT, teamct_get_score());
	}
}

public join_team () {
	static loguser[80], name[32], id, team[2];

	read_logargv(0, loguser, 79)
	parse_loguser(loguser, name, 31)
	id = get_user_index(name);

	read_logargv(2, team, 1);

	switch (team[0]) {
		case 'T':
			client_set_team(id, TEAM_TERRORIST);
		case 'C':
			client_set_team(id, TEAM_CT);
		case 'S':
			client_set_team(id, TEAM_SPECTATOR);
		default:
			client_set_team(id, TEAM_UNASSIGNED);
	}
}
