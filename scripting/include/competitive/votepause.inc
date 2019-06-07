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

static vote_delay, vote_times, vote_time;

stock votepause_init () {
	vote_delay = register_cvar("pug_votepause_delay", "10");
	vote_times = register_cvar("pug_votepause_times", "1");
	vote_time = register_cvar("pug_votepause_time", "60");

	registerCommand("pause", "start_votepause", ADMIN_ALL,
					"Starts a votepause");
}

static create_menu () {
	new const newmenu = menu_create("Pausar ronda:", "votepause_menu_handler");

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

public start_votepause (const id, level, cid) {
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

	if (votes[team][0] < teammates_count[team] - 1) {
		team_print(team, "%L", LANG_SERVER, "PUG_VOTING_INSUFFICIENT");
		return;
	}

	chat_print(0, "%L", LANG_SERVER, "PUG_VOTEPAUSE_SUCCESS", team_name[team]);
	set_freezetime(15 + _get_votetime());

	return;
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

static _get_votetimes ()
	return get_pcvar_num(vote_times);

static _get_votetime ()
	return get_pcvar_num(vote_time);

static bool:validate_cmd (id, TeamName:team) {
	if (!(TEAM_TERRORIST <= team <= TEAM_CT))
		return false;

	if (is_voting[team])
		return false;

	if (!game_is_live())
		return false;

	if (pause_limiter[team] >= _get_votetimes()) {
		chat_print(id, "%L", LANG_SERVER, "PUG_VOTEPAUSE_MAXVOTES");
		return false;
	}

	return true;
}

static set_init_values (TeamName:team) {
	menu[team] = create_menu();
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

