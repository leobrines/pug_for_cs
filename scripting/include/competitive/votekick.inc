#include <amxmodx>
#include <competitive/utils>

static const MAX_VOTEKICKS = 3;

static target, target_name[32];
static admin_name[32], reason[50];

static menu, votes[2];
static votekick_limiter[33];

static vote_delay;
static vote_perc;

stock votekick_init () {
	create_cmd();
	create_cvars();
	create_menu();
}

static create_cmd () {
	registerCommand("votekick", "start_votekick", ADMIN_ALL,
					"<name or #userid> <reason> Starts a votekick");
}

static create_cvars () {
	vote_delay = register_cvar("pug_vote_delay", "10");
	vote_perc = register_cvar("pug_votekick_percent", "0.7");
}

static create_menu () {
	menu = menu_create("Votekick:", "votekick_menu_handler");

	menu_additem(menu, "Si", "1");
	menu_additem(menu, "No", "2");
	menu_setprop(menu, MPROP_EXIT, MEXIT_NEVER);
}

public start_votekick (const id, level, cid) {
	if (!validate_cmd(id, level, cid))
		return;

	target = find_target(id);

	if (!validate_target(target))
		return;

	set_names(id, target);
	set_reason();
	set_menu_title();
	reset_votes();

	votekick_limiter[id]++;

	display_menu_all(menu);

	new const Float:votedelay = _get_votedelay();
	set_task(votedelay, "finish_votekick", _, _, _, "a", 1);

	chat_print(0, "%L", LANG_SERVER, "PUG_VOTEKICK_STARTED",
				admin_name, target_name, reason);
}

public finish_votekick () {
	cancel_menu();
	verify_votes();
}

public votekick_menu_handler (const id, menu, item) {
	if (item == MENU_EXIT)
		return PLUGIN_HANDLED;

	votes[item]++;

	if (valid_votes())
		finish_votekick();

	return PLUGIN_HANDLED;
}

static Float:_get_votedelay ()
	return get_pcvar_float(vote_delay);

static Float:_get_voteperc ()
	return get_pcvar_float(vote_perc);

static bool:validate_cmd (id, level, cid) {
	if (client_is_spectator(id)) {
		chat_print(id, "%L", LANG_SERVER, "PUG_VOTEKICK_SPECTATORS");
		return false;
	} else if (!cmd_access(id, level, cid, 2)) {
		chat_print(id, "%L", LANG_SERVER, "PUG_VOTEKICK_SPECIFY");
		return false;
	} else if (votekick_limiter[id] > MAX_VOTEKICKS) {
		chat_print(id, "%L", LANG_SERVER, "PUG_VOTEKICK_MAXVOTES");
		return false;
	}

	return true;
}

static find_target (id) {
	new target_finder[32];

	read_argv(1, target_finder, charsmax(target_finder));
	return cmd_target(id, target_finder, CMDTARGET_OBEY_IMMUNITY |
							CMDTARGET_NO_BOTS);
}

static bool:validate_target (id) {
	if (!target) {
		chat_print(id, "%L", LANG_SERVER, "PUG_VOTEKICK_UNAVAILABLE")
		return false;
	} 

	return true;
}

static set_names (admin, target) {
	get_user_name(admin, admin_name, charsmax(admin_name));
	get_user_name(target, target_name, charsmax(target_name));
}

static set_reason () {
	if (read_argc() > 2)
		read_argv(2, reason, charsmax(reason));
	else
		formatex(reason, charsmax(reason), "%L", LANG_SERVER,
					"PUG_VOTEKICK_NOREASON")
}

static reset_votes ()
	arrayset(votes, 0, 2);

static set_menu_title () {
	static votename[100];
	formatex(votename, charsmax(votename), "%L", LANG_SERVER, "PUG_VOTEKICK_TITLE", target_name, reason);
	menu_setprop(menu, MPROP_TITLE, votename);
}

static verify_votes () {
	new winner;

	if (votes[0] > votes[1]) {
		winner = 0;
	} else if (votes[0] < votes[1]) {
		winner = 1;
	} else {
		chat_print(0, "%L", LANG_SERVER, "PUG_VOTEKICK_TIE");
		return PLUGIN_HANDLED;
	}

	if (votes[winner] == 0) {
		chat_print(0, "%L", LANG_SERVER, "PUG_VOTEKICK_FAILED");
		return PLUGIN_HANDLED;
	}

	new Float:tmp = float(get_playersnum()) * _get_voteperc();

	if (votes[winner] < floatround(tmp, floatround_floor)) {
		chat_print(0, "%L", LANG_SERVER, "PUG_VOTEKICK_INSUFFICIENT");
		return PLUGIN_HANDLED;
	}

	if (winner == 1) {
		server_cmd("kick #%i %s", get_user_userid(target), reason);
		chat_print(0, "%L", LANG_SERVER, "PUG_VOTEKICK_KICKED", target_name, votes[winner]);
	} else if (winner == 2) {
		chat_print(0, "%L", LANG_SERVER, "PUG_VOTEKICK_NOTKICKED", target_name, votes[winner]);
	}

	return PLUGIN_HANDLED;
}

static display_menu_all (menu_id) {
	new players[MAX_PLAYERS], count, id;
	get_players(players, count, "ch");

	for (new i=0; i<count; i++) {
		id = players[i];
		menu_display(id, menu_id)
	}
}

static cancel_menu () {
	new players[MAX_PLAYERS], count;
	get_players(players, count, "ch");

	for (new i=0; i<count; i++)
		menu_cancel(players[i]);
}

static get_total_votes ()
	return votes[0] + votes[1];

static bool:valid_votes () {
	new players[MAX_PLAYERS], count;
	get_players(players, count, "ch");

	new const total_votes = get_total_votes();

	if (total_votes >= count)
		return true;

	return false;
}

