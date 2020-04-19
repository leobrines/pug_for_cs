// Thanks to Kiske https://forums.alliedmods.net/showthread.php?t=177310

#if defined _chooseteam_included
  #endinput
#endif

#define _chooseteam_included

#include <amxmodx>

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
	new param[2];
	read_argv(1, param, charsmax(param)); 

	new const selected = str_to_num(param);
	new TeamName:team;

	client_print(id, print_chat, "selected1: %d", selected)

	switch (selected) {
		case 1:
			team = TEAM_TERRORIST;
		case 2:
			team = TEAM_CT;
		case 5:
			team = TEAM_UNASSIGNED;
		case 6:
			team = TEAM_SPECTATOR;
	}

	return check_team(id, team)
}

public chooseteam_select_showmenu (id, selected) {
	new TeamName:team;

	client_print(id, print_chat, "selected2: %d", selected)

	switch (selected) {
		case 0:
			team = TEAM_TERRORIST;
		case 1:
			team = TEAM_CT;
		case 4:
			team = TEAM_UNASSIGNED;
		case 5:
			team = TEAM_SPECTATOR;
	}
	
	return check_team(id, team)
}

