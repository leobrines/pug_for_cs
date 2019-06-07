#if defined _votekick_included
  #endinput
#endif

#define _votekick_included

#include <amxmodx>
#include <competitive/utils>

static const MAX_VOTEKICKS = 3;

static target[TeamName], target_name[TeamName][32];
static votekick_limiter[33];

static menu[TeamName], votes[TeamName][2];
static teammates_count[TeamName];
static bool:is_voting[TeamName];

static vote_delay;

stock votekick_init () {
	vote_delay = register_cvar("pug_vote_delay", "10");
}

static create_menu () {
	new const newmenu = menu_create("Votekick:", "votekick_menu_handler");

	menu_additem(newmenu, "Si");
	menu_additem(newmenu, "No");
	menu_setprop(newmenu, MPROP_EXIT, MEXIT_NEVER);

	return newmenu;
}

public votekick_menu_handler (const id, menu, item) {
	if (item == MENU_EXIT)
		return PLUGIN_HANDLED;
	
	new const TeamName:team = client_get_team(id);

	votes[team][item]++;

	if (all_voted(team))
		finish_votekick(team);

	return PLUGIN_HANDLED;
}

stock start_votekick (id, target) {
	if (!validate_cmd(id, target))
		return;

	new const TeamName:team = client_get_team(id);

	set_init_values(id, team);

	new name[32];
	get_user_name(id, name, charsmax(name));
	team_print(team, "%L", LANG_SERVER, "PUG_VOTING_STARTED", name);

	display_menu_team(id, menu[team], team);

	new args[1];
	args[0] = _:team;
	set_task(_get_votedelay(), "finish_votekick_timeout", _, args, 1, "a", 1);

	return;
}

public finish_votekick_timeout (args[]) {
	if (is_voting[TeamName:args[0]])
		finish_votekick(TeamName:args[0]);
}

static finish_votekick (TeamName:team) {
	is_voting[team] = false;

	cancel_menu(team);

	if (votes[team][0] < teammates_count[team] - 1) {
		team_print(team, "%L", LANG_SERVER, "PUG_VOTING_INSUFFICIENT");
		return;
	}

	server_cmd("amx_ban #%i 60 ^"Votekick^"", get_user_userid(target[team]));
	chat_print(0, "%L", LANG_SERVER, "PUG_VOTEKICK_KICKED", target_name[team]);

	return;
}

static display_menu_team (id, menu_id, TeamName:team) {
	new players[MAX_PLAYERS], player;
	get_players(players, teammates_count[team], "ceh", team == TEAM_TERRORIST ? "TERRORIST" : "CT");

	for (new i; i<teammates_count[team]; i++) {
		player = players[i];

		if (player == id)
			continue;

		menu_display(player, menu_id)
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

static bool:validate_cmd (id, target) {
	if (!client_is_player(id)) {
		return false;
	} else if (votekick_limiter[id] > MAX_VOTEKICKS) {
		chat_print(id, "%L", LANG_SERVER, "PUG_VOTEKICK_MAXVOTES");
		return false;
	} else if (is_voting[client_get_team(id)]) {
		return false;
	} else if (team_count_players(client_get_team(id)) > 2) {
		return false;
	} else if (client_get_team(id) != client_get_team(target)) {
		return false;
	}

	return true;
}

static set_init_values (id, TeamName:team) {
	menu[team] = create_menu();
	set_menu_title(menu[team]);

	votekick_limiter[id]++;
	get_user_name(target[team], target_name[team], charsmax(target_name[]));

	is_voting[team] = true;

	arrayset(votes[team], 0, 2);
	votes[team][0]++;
}

static set_menu_title (menu_target) {
	static votename[100];
	formatex(votename, charsmax(votename), "%L", LANG_SERVER, "PUG_VOTEKICK_TITLE", target_name);
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

