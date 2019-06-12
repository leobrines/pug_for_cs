#if defined _cmd_included
  #endinput
#endif

#define _cmd_included

static const KEYSMENU = (1<<0)|(1<<1)|(1<<4)|(1<<5);

stock cmd_init () {
	register_concmd(".auto", "cmd_auto", PUG_CMD_LVL)
	register_concmd(".manual", "cmd_manual", PUG_CMD_LVL)
	register_concmd(".restart", "cmd_restart", PUG_CMD_LVL);

	register_concmd(".start", "cmd_start", PUG_CMD_LVL);
	register_concmd(".cancel", "cmd_cancel", PUG_CMD_LVL);

	register_concmd(".mute", "cmd_mute", ADMIN_ALL);
	register_concmd(".unmute", "cmd_unmute", ADMIN_ALL);

	register_concmd(".votekick", "cmd_votekick", ADMIN_ALL);
	register_concmd(".pause", "cmd_votepause", ADMIN_ALL);

	register_clcmd("chooseteam", "cmd_chooseteam");
	register_clcmd("jointeam", "cmd_jointeam");
	register_menucmd(register_menuid("Team_Select", 1), KEYSMENU,
										"cmd_teamshowmenu"); 
}

public cmd_jointeam (id) {
	new param[2], selected, TeamName:team;

	read_argv(1, param, charsmax(param)); 
	selected = str_to_num(param);

	switch (selected) {
		case 1: team = TEAM_TERRORIST;
		case 2: team = TEAM_CT;
		case 6: team = TEAM_SPECTATOR;
		case 5: team = TEAM_UNASSIGNED;
		default: return PLUGIN_CONTINUE;
	}

	if (valid_jointeam(id, team))
		return PLUGIN_CONTINUE;

	return PLUGIN_HANDLED;
}

public cmd_teamshowmenu (id, selected) {
	new TeamName:team;

	switch (selected) {
		case 0: team = TEAM_TERRORIST;
		case 1: team = TEAM_CT;
		case 4: team = TEAM_UNASSIGNED;
		case 5: team = TEAM_SPECTATOR;
	}

	if (valid_jointeam(id, team))
		return PLUGIN_CONTINUE;

	return PLUGIN_HANDLED;
}

public cmd_chooseteam (id) {
	// Unlocks the selection of a single team
	set_pdata_int(id, 125, get_pdata_int(id, 125, 5) & ~(1<<8), 5);

	return PLUGIN_CONTINUE;
}

public cmd_restart (id, level, cid) {
	if (!cmd_access(id, level, cid, 1))
		return PLUGIN_CONTINUE;

	server_cmd("sv_restart 3");

	return PLUGIN_HANDLED;
}

public cmd_start (id, level, cid) {
	if (!cmd_access(id, level, cid, 1))
		return PLUGIN_CONTINUE;

	if (game_is_started()) {
		chat_print(id, "%L", LANG_SERVER, "PUG_ACTION_NOTALLOWED")
		return PLUGIN_HANDLED;
	}

	new sName[MAX_NAME_LENGTH]
	get_user_name(id, sName, charsmax(sName))
	chat_print(0, "%L", LANG_SERVER, "PUG_FORCE_GAME", sName)

	PugStart();

	return PLUGIN_HANDLED;
}

public cmd_cancel (id, level, cid) {
	if (!cmd_access(id, level, cid, 1))
		return PLUGIN_CONTINUE;

	if (!game_is_live()) {
		chat_print(id, "%L", LANG_SERVER, "PUG_ACTION_NOTALLOWED");
		return PLUGIN_HANDLED;
	}

	new sName[MAX_NAME_LENGTH]
	get_user_name(id, sName, charsmax(sName))
	chat_print(0, "%L", LANG_SERVER, "PUG_FORCE_CANCEL", sName)

	PugWarmup();

	return PLUGIN_HANDLED;
}

public cmd_manual (id, level, cid) {
	if (!cmd_access(id, level, cid, 1))
		return PLUGIN_CONTINUE;

	new sName[MAX_NAME_LENGTH]
	get_user_name(id, sName, charsmax(sName))
	chat_print(0, "%L", LANG_SERVER, "PUG_CHANGE_TO_MANUAL", sName);

	set_server_manual();
	
	return PLUGIN_HANDLED;
}

public cmd_auto (id, level, cid) {
	if (!cmd_access(id, level, cid, 1))
		return PLUGIN_CONTINUE;

	new sName[MAX_NAME_LENGTH]
	get_user_name(id, sName, charsmax(sName))
	chat_print(0, "%L", LANG_SERVER, "PUG_CHANGE_TO_AUTO", sName);

	set_server_auto();
	
	return PLUGIN_HANDLED;
}

public cmd_mute (const id, level, cid) {
	new const target = find_target(id, level, cid);

	if (target)
		client_mute(id, target);

	return PLUGIN_HANDLED
}

public cmd_unmute (const id, level, cid) {
	new const target = find_target(id, level, cid);

	if (target)
		client_unmute(id, target);

	return PLUGIN_HANDLED
}

public cmd_votekick (id, level, cid) {
	if (!client_is_player(id) || !game_is_started())
		return PLUGIN_CONTINUE;
		
	new const target = find_target(id, level, cid);

	if (target)
		start_votekick(id, target);

	return PLUGIN_HANDLED
}

public cmd_votepause (id, level, cid) {
	if (!cmd_access(id, level, cid, 1) || 
		!client_is_player(id) ||
		!game_is_live())
	{
		return PLUGIN_CONTINUE;
	}

	start_votepause(id);

	return PLUGIN_HANDLED
}

static find_target (id, level, cid) {
	if (!cmd_access(id, level, cid, 2)) {
		chat_print(id, "%L", LANG_SERVER, "PUG_CMD_SPECIFY");
		return 0;
	}

	new target, targetName[32];

	read_argv(1, targetName, charsmax(targetName));
	target = cmd_target(id, targetName, CMDTARGET_OBEY_IMMUNITY | 
										CMDTARGET_NO_BOTS |
										CMDTARGET_ALLOW_SELF);

	if (!target) {
		chat_print(id, "%L", LANG_SERVER, "PUG_CMD_UNAVAILABLE")
		return 0;
	}

	return target;
}

