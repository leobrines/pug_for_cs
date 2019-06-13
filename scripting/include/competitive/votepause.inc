#if defined _votepause_included
  #endinput
#endif

#define _votepause_included

#include <amxmodx>
#include <competitive/utils>

static pause_limiter[TeamName];

static menu[TeamName], votes[TeamName][2];
static teammates_count[TeamName];
static bool:is_voting[TeamName];
static bool:will_pause;
static bool:in_pause;

static vote_delay, vote_maxplayervotes, vote_time;

stock votepause_init () {
	vote_delay = register_cvar("pug_votepause_delay", "10");
	vote_maxplayervotes = register_cvar("pug_votepause_maxplayervotes", "1");
	vote_time = register_cvar("pug_votepause_time", "60");
}

static votepause_create_menu () {
	new newmenu = menu_create("Pausar ronda:", "votepause_menu_handler");

	menu_additem(newmenu, "Si");
	menu_additem(newmenu, "No");
	menu_setprop(newmenu, MPROP_EXIT, MEXIT_NEVER);

	return newmenu;
}

public votepause_menu_handler (const id, menu, item) {
	if (item == MENU_EXIT)
		return PLUGIN_HANDLED;
	
	new const TeamName:team = client_get_team(id);

	votes[team][item]++;

	if (all_voted(team))
		finish_votepause(team);

	return PLUGIN_HANDLED;
}

stock start_votepause (id) {
	new TeamName:team = client_get_team(id);

	if (!validate_cmd(id, team))
		return PLUGIN_HANDLED;
	
	set_init_values(team);

	new name[32];
	get_user_name(id, name, charsmax(name));
	team_print(team, "%L", LANG_SERVER, "PUG_VOTING_STARTED", name);

	display_menu_team(id, team);

	new args[1];
	args[0] = _:team;
	set_task(_get_votedelay(), "finish_votepause_timeout", _, args, 1, "a", 1);

	return PLUGIN_HANDLED;
}

public finish_votepause_timeout (args[]) {
	if (is_voting[TeamName:args[0]])
		finish_votepause(TeamName:args[0]);
}

static finish_votepause (TeamName:team) {
	is_voting[team] = false;
	cancel_menu(team);

	if (will_pause || in_pause)
		return;

	if (votes[team][0] < teammates_count[team] - 1) {
		team_print(team, "%L", LANG_SERVER, "PUG_VOTING_INSUFFICIENT");
		return;
	}

	chat_print(0, "%L", LANG_SERVER, "PUG_VOTEPAUSE_SUCCESS", team_name[team]);
	will_pause = true;
}

stock votepause_check () {
	if (will_pause)  {
		will_pause = false;
		in_pause = true;
		set_freezetime(15 + get_votetime());
		server_cmd("sv_restart 1");
	} else {
		set_freezetime(15);
		in_pause = false;
	}
}

stock votepause_end () {
	set_freezetime(15);
	will_pause = false;
}

static display_menu_team (id, TeamName:team) {
	new players[MAX_PLAYERS], player;
	get_players(players, teammates_count[team], "ceh", team == TEAM_TERRORIST ? "TERRORIST" : "CT");

	for (new i; i<teammates_count[team]; i++) {
		player = players[i];

		if (player == id)
			continue

		menu_display(player, menu[team])
	}
}

static cancel_menu (TeamName:team) {
	new players[MAX_PLAYERS], count;
	get_players(players, count, "ceh", team == TEAM_TERRORIST ? "TERRORIST" : "CT");

	for (new i; i<count; i++)
		menu_cancel(players[i]);
}

static Float:_get_votedelay ()
	return get_pcvar_float(vote_delay);

static get_maxplayervotes ()
	return get_pcvar_num(vote_maxplayervotes);

static get_votetime ()
	return get_pcvar_num(vote_time);

static bool:validate_cmd (id, TeamName:team) {
	if (is_voting[team] || !team_can_vote(team) || will_pause || in_pause)
		return false;

	if (pause_limiter[team] >= get_maxplayervotes()) {
		chat_print(id, "%L", LANG_SERVER, "PUG_VOTEPAUSE_MAXVOTES");
		return false;
	}

	return true;
}

static set_init_values (TeamName:team) {
	menu[team] = votepause_create_menu();
	set_menu_title(menu[team]);

	pause_limiter[team]++;

	is_voting[team] = true;

	arrayset(votes[team], 0, 2);
	votes[team][0]++;
}

static set_menu_title (menu_target) {
	static votename[100];
	formatex(votename, charsmax(votename), "%L", LANG_SERVER,
										"PUG_VOTEPAUSE_TITLE");
	menu_setprop(menu_target, MPROP_TITLE, votename);
}

static get_total_votes (TeamName:team)
	return votes[team][0] + votes[team][1];

static bool:all_voted (TeamName:team) {
	new const total_votes = get_total_votes(team);

	if (total_votes >= teammates_count[team])
		return true;

	return false;
}

