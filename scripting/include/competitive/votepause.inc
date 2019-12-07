#if defined _votepause_included
  #endinput
#endif

#define _votepause_included

#define TASK_HUD_VOTEPAUSE 7000

static pause_limiter[_:TeamName];

static votes[TeamName][2];
static teammates_count[TeamName];
static bool:is_voting[TeamName];
static bool:will_pause;
static bool:in_pause;

static vote_delay, vote_maxvotes, vote_time;
static freezetime_saved;

stock votepause_init () {
	vote_delay = register_cvar("pug_votepause_delay", "10");
	vote_maxvotes = register_cvar("pug_votepause_maxvotes", "1");
	vote_time = register_cvar("pug_votepause_time", "60");
}

stock votepause_menucreate () {
	new title[100], menu;
	new option_yes[5], option_no[5];

	formatex(title, charsmax(title), "%L", LANG_SERVER, "PUG_VOTEPAUSE_TITLE");
	format(option_yes, charsmax(option_yes), "%L", LANG_SERVER, "YES");
	format(option_no, charsmax(option_no), "%L", LANG_SERVER, "NO");

	menu = menu_create(title, "votepause_menuhandler");

	menu_additem(menu, option_yes);
	menu_additem(menu, option_no);

	menu_setprop(menu, MPROP_EXIT, MEXIT_NEVER);

	return menu;
}

public votepause_menuhandler (const id, menu, item) {
	if (item == MENU_EXIT)
		return PLUGIN_HANDLED;
	
	new const TeamName:team = client_get_team(id);

	votes[team][item]++;
	votepause_list(team);

	if (votepause_isready(team))
		votepause_finish(team, menu);

	return PLUGIN_HANDLED;
}

stock votepause_start (id) {
	new TeamName:team = client_get_team(id);

	if (!votepause_validate(id, team))
		return PLUGIN_HANDLED;
	
	is_voting[team] = true;
	arrayset(votes[team], 0, 2);

	new name[32];
	get_user_name(id, name, charsmax(name));
	team_print(team, "%L", LANG_SERVER, "PUG_VOTING_STARTED", name);

	new menu = votepause_menucreate();
	teammates_count[team] = menu_display_team(team, menu);

	new args[2];
	args[0] = _:team;
	args[1] = menu;
	set_task(_get_votedelay(), "votepause_finish_task", _, args, sizeof(args), "a", 1);
	set_task(0.1, "votepause_list_task", TASK_HUD_VOTEPAUSE, args, sizeof(args), "b");

	return PLUGIN_HANDLED;
}

public votepause_finish_task (args[]) {
	if (is_voting[TeamName:args[0]])
		votepause_finish(TeamName:args[0], args[1]);
}

public votepause_list_task (args[])
	votepause_list(TeamName:args[0])

stock votepause_list (TeamName:team) {
	new hud[512];
	new votes_yes, votes_no;

	votes_yes = votes[TeamName:team][0];
	votes_no = votes[TeamName:team][1];

	if (votes_yes)
		format(hud, charsmax(hud), "%s[%i] %L^n", hud, votes_yes, LANG_SERVER, "YES")
	if (votes_no)
		format(hud, charsmax(hud), "%s[%i] %L^n", hud, votes_no, LANG_SERVER, "NO")

	if (!(votes_yes + votes_no)) 
		formatex(hud, charsmax(hud), "%L", LANG_SERVER, "PUG_NOVOTES")

	showt_hudtitle(team, "Votepause");
	showt_hudbody(team, hud);
}

stock votepause_finish (TeamName:team, menu) {
	is_voting[team] = false;

	remove_task(TASK_HUD_VOTEPAUSE)
	menu_cancel_team(team);
	menu_destroy(menu);

	if (will_pause || in_pause)
		return;

	if (votes[team][0] < teammates_count[team] - 1 || !votes[team][0]) {
		team_print(team, "%L", LANG_SERVER, "PUG_VOTING_INSUFFICIENT");
		return;
	}

	chat_print(0, "%L", LANG_SERVER, "PUG_VOTEPAUSE_SUCCESS", team_name[team]);
	will_pause = true;
	pause_limiter[_:team]++;
}
stock votepause_check (bool:pause_now) {
	if (will_pause) {
		will_pause = false;
		in_pause = true;

		freezetime_saved = get_freezetime();
		set_freezetime(get_votetime());

		if (pause_now)
			server_cmd("sv_restart 1");
	} else {
		if (freezetime_saved) {
			set_freezetime(freezetime_saved);
			freezetime_saved = 0;
		}

		in_pause = false;
	}
}

stock votepause_reset ()
	arrayset(pause_limiter, 0, sizeof(pause_limiter));

static Float:_get_votedelay ()
	return get_pcvar_float(vote_delay);

static get_maxvotes ()
	return get_pcvar_num(vote_maxvotes);

static get_votetime ()
	return get_pcvar_num(vote_time);

stock bool:votepause_validate (id, TeamName:team) {
	if (is_voting[team] || !team_can_vote(team) || will_pause || in_pause)
		return false;

	if (pause_limiter[_:team] >= get_maxvotes()) {
		chat_print(id, "%L", LANG_SERVER, "PUG_VOTEPAUSE_MAXVOTES");
		return false;
	}

	return true;
}

stock votepause_totalvotes (TeamName:team)
	return votes[team][0] + votes[team][1];

stock bool:votepause_isready (TeamName:team) {
	new const total_votes = votepause_totalvotes(team);

	if (total_votes >= teammates_count[team])
		return true;

	return false;
}

