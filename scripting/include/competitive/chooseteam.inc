// Thanks to Kiske https://forums.alliedmods.net/showthread.php?t=177310

#if defined _chooseteam_included
  #endinput
#endif

#define _chooseteam_included

#include <amxmodx>
#include <reapi>

static const KEYSMENU = (1<<0)|(1<<1)|(1<<4)|(1<<5);

stock chooseteam_init () {
	register_clcmd("chooseteam", "chooseteam_vgui");
	register_clcmd("jointeam", "chooseteam_select_vgui");
	register_menucmd(register_menuid("Team_Select", 1), KEYSMENU,
										"chooseteam_select_showmenu"); 
}

public chooseteam_vgui (id) {
	set_pdata_int(id, 125, get_pdata_int(id, 125, 5) & ~(1<<8), 5);
	return PLUGIN_CONTINUE;
}

public chooseteam_select_vgui (id) {
	if (!valid_player(id))
		return PLUGIN_HANDLED;

	new param[2];
	read_argv(1, param, charsmax(param)); 

	new const selected = str_to_num(param);

	switch (selected) {
		case 1:
			client_set_team(id, TEAM_TERRORIST);
		case 2:
			client_set_team(id, TEAM_CT);
		case 5:
			client_set_team(id, TEAM_UNASSIGNED);
		case 6:
			client_set_team(id, TEAM_SPECTATOR);
	}

	return PLUGIN_HANDLED;
}

public chooseteam_select_showmenu (id, selected) {
	if (!valid_player(id))
		return PLUGIN_HANDLED;

	switch (selected) {
		case 0:
			client_set_team(id, TEAM_TERRORIST);
		case 1:
			client_set_team(id, TEAM_CT);
		case 4:
			client_set_team(id, TEAM_UNASSIGNED);
		case 5:
			client_set_team(id, TEAM_SPECTATOR);
	}

	return PLUGIN_HANDLED;
}

static bool:valid_player (id) {
	if (game_is_started() && client_is_player(id)) {
		chat_print(id, "%L", LANG_SERVER, "PUG_TEAMS_NOT_CHANGE");
		return false;
	}

	return true;
}

