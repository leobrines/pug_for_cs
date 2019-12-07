#if defined _afkicker_included
  #endinput
#endif

#define _afkicker_included

#define MIN_AFK_TIME 30
#define DROPC4_AFK_TIME 20
#define CHECK_FREQ 5

new old_angles[33][3]
new afktime[33]
new bool:spawned[33] = {true, ...}

stock afkicker_start () {
	set_task(float(CHECK_FREQ), "check_afkplayers", _, _, _, "b");
	register_cvar("pug_afktime", "60");
}

public afkicker_newround_task () 
	afkicker_newround();

stock afkicker_newround () {
	new players[MAX_PLAYERS], num;

	get_players(players, num, "ceh", "TERRORIST");
	afkicker_players_spawn(players, num);

	get_players(players, num, "ceh", "CT");
	afkicker_players_spawn(players, num);
}

stock afkicker_players_spawn (players[MAX_PLAYERS], num) {
	new id;
	for (new i; i<num; i++) {
		id = players[i];
		get_user_origin(id, old_angles[id]);
		spawned[id] = true;
	}
}

public check_afkplayers () {
	if (!game_is_live())
		return;

	new players[MAX_PLAYERS], num;
	get_players(players, num, "ach");

	for (new i; i<num; i++)
		user_update_angles(players[i]);
}

stock user_update_angles (id) {
	if (user_is_afk(id)) {
		afktime[id] += CHECK_FREQ
		afkicker_trykick(id);
	} else {
		get_user_origin(id, old_angles[id]);
		afktime[id] = 0;
	}
}

stock bool:user_is_afk (id) {
	new angles[3];
	get_user_origin(id, angles);

	return (angles[0] == old_angles[id][0] &&
			angles[1] == old_angles[id][1] &&
			angles[2] == old_angles[id][2]);
}

stock afkicker_trykick (id) {
	new maxafktime = get_cvar_num("pug_afktime");

	if (maxafktime < MIN_AFK_TIME) {
		log_amx("%L", LANG_SERVER, "PUG_AFKKICKER_CVAR", maxafktime, MIN_AFK_TIME)
		maxafktime = MIN_AFK_TIME
		set_cvar_num("pug_afktime", MIN_AFK_TIME)
	}

	if (afktime[id] > DROPC4_AFK_TIME)
        client_drop_c4(id);

	if (afktime[id] > maxafktime) {
		new szName[32]
		get_user_name(id, szName, 31)
		chat_print(0, "%L", LANG_SERVER, "PUG_AFKKICKER_KICK", szName, maxafktime)
		log_amx("%s was kicked for being AFK longer than %i seconds", szName, maxafktime)
		server_cmd("kick #%d ^"%L^"", get_user_userid(id), LANG_SERVER, "PUG_AFKKICKER_KICK2", maxafktime)
	}
}
