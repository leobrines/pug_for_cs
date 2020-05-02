#if defined _menutils_included
  #endinput
#endif

#define _menutils_included

stock menu_display_team (TeamName:team, menu) {
	new players[MAX_PLAYERS], num, strteam[12];

	team_to_str(team, strteam, charsmax(strteam));
	get_players(players, num, "ceh", strteam);

	for (new i; i<num; i++)
		menu_display(players[i], menu);

	return num;
}

stock menu_cancel_team (TeamName:team) {
	new players[MAX_PLAYERS], num, strteam[12];

	team_to_str(team, strteam, charsmax(strteam));
	get_players(players, num, "ceh", strteam);

	for (new i; i<num; i++)
		menu_cancel(players[i]);
}

stock menu_create_players (id, handler[]) {
	new players[MAX_PLAYERS], num;
	get_players(players, num, "ch");

	new menu = menu_create_ptitle(handler);
	menu_addplayers(menu, players, num, id);

	return menu;
}

stock menu_create_tplayers (id, handler[], bool:showself=false) {
	new players[MAX_PLAYERS], num, strteam[12];

	team_to_str(client_get_team(id), strteam, charsmax(strteam));
	get_players(players, num, "ceh", strteam);

	new menu = menu_create_ptitle(handler);
	menu_addplayers(menu, players, num, showself ? id : 0);

	return menu;
}

// Menu for player team and spectators 
stock menu_create_tsplayers (id, handler[], bool:showself=false) {
	new players[MAX_PLAYERS], num, TeamName:team;
	new playersf[MAX_PLAYERS], pindex;

	get_players(players, num, "ch");

	team = client_get_team(id);

	for (new i = 0; i < num; i++) {
		new p = players[i]
		new TeamName:pteam = client_get_team(p);

		if (!showself && p == id)
			continue

		if (team == pteam || pteam == TEAM_SPECTATOR) {
			playersf[pindex] = p
			pindex++
		}
	}

	new menu = menu_create_ptitle(handler);
	menu_addplayers(menu, playersf, num, showself ? id : 0);

	return menu;
}

stock menu_create_ptitle (handler[]) {
	new title[32];
	format(title, charsmax(title), "%L", LANG_SERVER, "PUG_MENU_SELECT")

	return menu_create(title, handler);
}

stock menu_addplayers (menu, players[], num, plyrignore=0) {
	new player, name[32], authid[32];

	for (new i; i<num; i++) {
		player = players[i];

		if (player == plyrignore)
			continue;

		get_user_name(player, name, charsmax(name));
		get_user_authid(player, authid, charsmax(authid));

		menu_additem(menu, name, authid);
	}
}
