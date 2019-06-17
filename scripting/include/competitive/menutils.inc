#if defined _menutils_included
  #endinput
#endif

#define _menutils_included

stock menu_display_team (TeamName:team, menu) {
	new players[MAX_PLAYERS], num;

	switch (team) {
		case TEAM_TERRORIST:
			get_players(players, num, "ceh", "TERRORIST");
		case TEAM_CT:
			get_players(players, num, "ceh", "CT");
		default:
			get_players(players, num, "ceh", "SPECTATOR");
	}

	for (new i; i<num; i++)
		menu_display(players[i], menu);

	return num;
}

stock menu_cancel_team (TeamName:team) {
	new players[MAX_PLAYERS], num;

	switch (team) {
		case TEAM_TERRORIST:
			get_players(players, num, "ceh", "TERRORIST");
		case TEAM_CT:
			get_players(players, num, "ceh", "CT");
		default:
			get_players(players, num, "ceh", "SPECTATOR");
	}

	for (new i; i<num; i++)
		menu_cancel(players[i]);
}
