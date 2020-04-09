stock pugmenu_start (id) {
	new menu = menu_create("Acciones: ", "pugmenu_handler");

	menu_additem(menu, "Pausar juego");
	menu_additem(menu, "Expulsar jugador");
	menu_additem(menu, "Mutear");
	menu_additem(menu, "Desmutear");

	menu_display(id, menu);
}

stock bool:player_can_vote (id) {
	if (is_restarting()) {
		return false;
	}
	if (!client_is_player(id)) {
		chat_print(id, "%L", LANG_SERVER, "PUG_VOTING_SPEC");
		return false;
	}
	if (!game_is_live()) {
		chat_print(id, "%L", LANG_SERVER, "PUG_GAME_NOTSTARTED");
		return false;
	}

	return true;
}

public pugmenu_handler (id, menu, item) {
	if (item == MENU_EXIT)
		return PLUGIN_HANDLED;

	switch (item) {
		case 0:
			if (player_can_vote(id))
				votepause_start(id);
		case 1:
			if (!is_votekick_enable())
				chat_print(id, "%L", LANG_SERVER, "PUG_VOTEKICK_DISABLED");
			else if (player_can_vote(id))
				pugmenu_votekick(id);
		case 2:
			pugmenu_mute(id);
		case 3:
			pugmenu_unmute(id);
	}

	return PLUGIN_HANDLED;
}

stock pugmenu_votekick (id) {
	new menu = menu_create_tplayers(id, "pugmenu_hvotekick");
	menu_display(id, menu);
}

public pugmenu_hvotekick (id, menu, item) {
	if (item == MENU_EXIT)
		return PLUGIN_HANDLED;

	new authid[32];
	menu_iteminfo(menu, item, authid, charsmax(authid));

	new target = find_player("c", authid);

	if (target)
		votekick_start(id, target);
		
	return PLUGIN_HANDLED;
}

stock menu_iteminfo (menu, item, info[], size)  {
	new dummy;
	menu_item_getinfo(menu, item, dummy, info, size, _, _, dummy);
}

stock pugmenu_mute (id) {
	new menu = menu_create_players(id, "pugmenu_hmute");
	menu_display(id, menu);
}

public pugmenu_hmute (id, menu, item) {
	if (item == MENU_EXIT)
		return PLUGIN_HANDLED;

	new authid[32];
	menu_iteminfo(menu, item, authid, charsmax(authid));

	new target = find_player("c", authid);

	if (target)
		client_mute(id, target);
		
	return PLUGIN_HANDLED;
}

stock pugmenu_unmute (id) {
	new menu = menu_create_players(id, "pugmenu_hunmute");
	menu_display(id, menu);
}

public pugmenu_hunmute (id, menu, item) {
	if (item == MENU_EXIT)
		return PLUGIN_HANDLED;

	new authid[32];
	menu_iteminfo(menu, item, authid, charsmax(authid));

	new target = find_player("c", authid);

	if (target)
		client_unmute(id, target);
		
	return PLUGIN_HANDLED;
}
