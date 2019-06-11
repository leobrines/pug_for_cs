#if defined _client_included
  #endinput
#endif

#define _client_included

stock bool:client_is_player (id)
	return TEAM_TERRORIST <= client_get_team(id) <= TEAM_CT;

stock bool:client_is_spectator (id) 
	return client_get_team(id) == TEAM_SPECTATOR;

stock bool:client_is_terrorist (id)
	return client_get_team(id) == TEAM_TERRORIST;

stock bool:client_is_ct (id)
	return client_get_team(id) == TEAM_CT;

stock TeamName:client_get_team (id)
	return get_member(id, m_iTeam);

stock client_give_money (id, amount)
	set_member(id, m_iAccount, amount);

stock client_get_money (id)
	return get_member(id, m_iAccount);

stock clients_print_money () {
	static players[MAX_PLAYERS], count, id
	get_players(players, count, "ah");

	for (new i; i < count; i++) {
		id = players[i]
		client_cmd(id, "say_team $%i", client_get_money(id));
	}
}

stock clients_inteam_count () {
	static players[MAX_PLAYERS], count, count_all;

	count_all = 0;

	get_players(players, count, "ceh", "TERRORIST");
	count_all += count ? count : 0;

	get_players(players, count, "ceh", "CT");
	count_all += count ? count : 0;

	return count_all;
}

stock is_player_id (id)
	return 1 <= id <= MAX_PLAYERS+1;

stock client_set_team (const id, TeamName:team) {
	if (!valid_player_team(id, team))
		return;

	set_task(5.0, "show_owners", id, _, _, "a", 1) 

	new TeamName:new_team;

	if (team == TEAM_SPECTATOR) {
		if (!is_spectator_allowed() && !access(id, PUG_CMD_LVL))
			chat_print(id, "%L", LANG_SERVER, "PUG_TEAMS_SPECTATORS");
		else
			client_join_team(id, team);

		return;
	} else if (team == TEAM_UNASSIGNED) {
		new_team = TeamName:random_num(_:TEAM_TERRORIST, _:TEAM_CT);
	} else {
		new_team = team;
	}

	if (team_is_full(new_team, true)) {
		if (is_user_bot(id))
			return;

		if (team_is_full(new_team, false)) {
			chat_print(id, "%L", LANG_SERVER, "PUG_TEAMS_FULL",
						team_name[new_team]);

			return;
		}
	}

	client_join_team(id, new_team);
}

stock client_join_team (id, TeamName:team) {
	rg_join_team(id, team);

	autoready_check();

	#if AMXX_VERSION_NUM < 183
	chatcolor_send_teaminfo(id);
	#endif
}

static bool:valid_player_team (id, TeamName:team) {
	if (client_get_team(id) == team)
		return false;

	if (game_is_started() && client_is_player(id)) {
		chat_print(id, "%L", LANG_SERVER, "PUG_TEAMS_NOT_CHANGE");
		return false;
	}

	return true;
}
