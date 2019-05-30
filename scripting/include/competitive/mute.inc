#include <amxmodx>
#include <fakemeta>

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

public client_mute (const id, level, cid) {
	new const target = cmd_get_target(id, level, cid);

	if (target) {
		clients_muted[id][target] = true	
		chat_print(id, "%L", LANG_SERVER, "PLAYER_MUTED");
	}

	return PLUGIN_HANDLED
}

public client_unmute (const id, level, cid) {
	new const target = cmd_get_target(id, level, cid);

	if (target) {
		clients_muted[id][target] = false
		chat_print(id, "%L", LANG_SERVER, "PLAYER_UNMUTED");
	}

	return PLUGIN_HANDLED
}

static cmd_get_target (id, level, cid) {
	if (!cmd_access(id, level, cid, 2)) {
		chat_print(id, "%L", LANG_SERVER, "PUG_VOTEKICK_SPECIFY");
		return 0;
	}

	new target, targetName[32];

	read_argv(1, targetName, 31);
	target = cmd_target(id, targetName, CMDTARGET_OBEY_IMMUNITY | 
										CMDTARGET_NO_BOTS |
										CMDTARGET_ALLOW_SELF);

	if (!target) {
		chat_print(id, "%L", LANG_SERVER, "PUG_VOTEKICK_UNAVAILABLE")
		return 0;
	}

	return target;
}
