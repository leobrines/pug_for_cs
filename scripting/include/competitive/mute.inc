#if defined _mute_included
  #endinput
#endif

#define _mute_included

static clients_muted[MAX_PLAYERS+1][MAX_PLAYERS+1];

stock client_mute_reset (id)
	arrayset(clients_muted[id], false, sizeof(clients_muted[]))

stock mute_reset () {
	new players[MAX_PLAYERS], count;
	get_players(players, count, "ch");
	
	for (new i; i<count;i++) 
		client_mute_reset(players[i]);
}

stock mute_init () {
	register_forward(FM_Voice_SetClientListening, "mute_hook");
	mute_reset();
}

public mute_hook (receiver, sender, bool:listen) {
	if (receiver == sender)
		return FMRES_IGNORED;
	
	if (clients_muted[receiver][sender]) {
		engfunc(EngFunc_SetClientListening, receiver, sender, false)
		return FMRES_SUPERCEDE;
	}

	return FMRES_IGNORED;
}

stock client_mute (id, target) {
	clients_muted[id][target] = true	
	chat_print(id, "%L", LANG_SERVER, "PLAYER_MUTED");
}

stock client_unmute (id, target) {
	clients_muted[id][target] = false
	chat_print(id, "%L", LANG_SERVER, "PLAYER_UNMUTED");
}
