// Thanks to Kiske https://forums.alliedmods.net/showthread.php?t=177310

#if defined _chooseteam_included
  #endinput
#endif

#define _chooseteam_included

#include <amxmodx>

static const KEYSMENU = MENU_KEY_1|MENU_KEY_2|MENU_KEY_5|MENU_KEY_6;

stock chooseteam_init () {
	register_clcmd("chooseteam", "chooseteam");
	register_clcmd("jointeam", "jointeam");
	register_menucmd(register_menuid("Team_Select", 1), KEYSMENU, "teamselect"); 
	register_menucmd(-2, KEYSMENU, "teamselect"); 
}

// Blocked  command
public chooseteam (id) {
	set_pdata_int(id, 125, get_pdata_int(id, 125, 5) & ~(1<<8), 5);
	return PLUGIN_CONTINUE;
}

public jointeam (id) {
	return chooseteam_check(id, read_argv_int(1))
}

public teamselect (id, selected) {
	return chooseteam_check(id, (selected+1))
}

public chooseteam_check (id, selected) {
	new TeamName:team;

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

