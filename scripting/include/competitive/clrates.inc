#if defined _clrates_included
  #endinput
#endif

#define _clrates_included

static cvar_minrate
static cvar_minupdaterate

public clrates_init() {
	cvar_minrate = register_cvar("pug_cl_minrate", "10000");
	cvar_minupdaterate = register_cvar("pug_cl_minupdaterate", "30");

	set_task(30.0, "check_players_rates", _, _, _, "b")
}

public check_players_rates() {
	new players[MAX_PLAYERS], count;
	get_players(players, count, "ch");

	for (new i=0; i < count; i++) {
		check_player_rates(players[i])
	}
}

public check_player_rates(id) {
	query_client_cvar(id, "rate", "check_rate") 
	query_client_cvar(id, "cl_cmdrate", "check_rate") 
	query_client_cvar(id, "cl_updaterate", "check_rate") 
}

public check_rate(id, cvar[], value[]) {
	new minvalue;

	if (equali(cvar, "rate"))
		minvalue = get_pcvar_num(cvar_minrate)
	else if (equali(cvar, "cl_cmdrate") || equali(cvar, "cl_updaterate"))
		minvalue = get_pcvar_num(cvar_minupdaterate)

	if (str_to_num(value) >= minvalue)
		return

	new name[32]
	get_user_name(id, name, charsmax(name))
					
	client_print(0, print_chat, "El jugador %s tiene pocos ^"%s^"", name, cvar)
	server_cmd("kick #%d ^"%s deben ser %i o mas!^"", get_user_userid(id), cvar, minvalue)
}

