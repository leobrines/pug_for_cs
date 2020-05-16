#if defined _event_included
  #endinput
#endif

#define _event_included

#define HUD_HIDE_TIMER (1<<4)
#define HUD_HIDE_MONEY (1<<5)

static Float:nextPlayerThink[MAX_PLAYERS+1]
static bool:restarting;

stock event_init () {
	register_message(get_user_msgid("TeamScore"), "event_team_score");
	register_message(get_user_msgid("ScoreInfo"), "event_player_score");

	register_forward(FM_SetModel, "event_drop_item") 
	register_forward(FM_GetGameDescription, "event_game_description");
	register_forward(FM_PlayerPostThink, "event_post_think");

	register_event("Money", "event_money", "b")
	register_event("Damage", "event_damage", "b", "2!0", "3=0", "4!0")
	register_event("DeathMsg", "event_death_player", "a", "1!0", "2!0");
	register_event("TextMsg", "event_round_restart","a","2=#Game_will_restart_in")
	register_logevent("event_ct_win", 6, "3=CTs_Win", "3=Target_Saved", "3=Bomb_Defused", "3=All_Hostages_Rescued")
	register_logevent("event_tt_win" , 6, "3=Terrorists_Win", "3=Target_Bombed")
	register_logevent("event_joined_team", 3, "1=joined team");
	register_logevent("event_new_round", 3, "2=Spawned_With_The_Bomb");
	register_logevent("event_freezetime_end", 2, "1=Round_Start") 
	register_logevent("event_round_end", 2, "1=Round_End") 
}

public event_player_score (m, s, id) {
	static player_id;
	player_id = get_msg_arg_int(1);

	if (game_is_live()) {
		set_msg_arg_int(2, ARG_SHORT, client_get_frags(player_id));
		set_msg_arg_int(3, ARG_SHORT, client_get_deaths(player_id));
	}
}

public event_team_score (m, e, id) {
	static team_score[2];
	get_msg_arg_string(1, team_score, charsmax(team_score));

	switch(team_score[0]) {
		case 'T':
			set_msg_arg_int(2, ARG_SHORT, teamtt_get_score());
		case 'C':
			set_msg_arg_int(2, ARG_SHORT, teamct_get_score());
	}
}

public event_joined_team () {
    new team[2], id;
    
    id = get_loguser_index();
    read_logargv(2, team, 1);

    user_joined(id);

    if ((team[0] == 'T' || team[0] == 'C') && !game_is_started())
        autoready_check();
}

public event_drop_item(entity, const model[]) {
	if (game_is_live())
		return FMRES_IGNORED 

	static class[10]
	pev(entity, pev_classname, class, sizeof class - 1)

	if (equal(class, "weaponbox")) {
		set_pev(entity, pev_nextthink, get_gametime () + 0.5)
	}

	return FMRES_HANDLED
}

public event_game_description() {
    new description[32]
    get_game_description(description, charsmax(description))

    forward_return(FMV_STRING, description);

    return FMRES_SUPERCEDE
}

public event_new_round () {
	restarting = false;

	if (!game_is_started()) {
		new id = get_loguser_index()
		client_delete_c4(id);

		return PLUGIN_CONTINUE;
	}
	if (!game_is_live())
		return PLUGIN_CONTINUE;

	round_start();

	if (!g_iRound) {
		teams_reset_scores();
		clients_reset_scores();
	}

	new showMoneyMode = get_showmoney_mode();

	switch (showMoneyMode) {
		case 1: {
			clients_print_money();
		}
		case 2: {
			set_task(0.2, "fnHudMoney", TASK_DISPLAY_INFO, _, _, "b")
			set_task(float(get_freezetime()), "fnRemoveHudMoney", _, _, _, "a", 1)
		}
	}

	return PLUGIN_HANDLED;
}

public event_money (id) {
	if (!is_user_connected(id))
		return PLUGIN_CONTINUE;

	if (game_is_started())
		return PLUGIN_CONTINUE;

	client_give_money(id, 16000);

	return PLUGIN_HANDLED;
}

public event_damage (victim) {
	new attacker, damage;
	attacker = get_user_attacker(victim);
	damage = read_data(2);

	if (attacker != victim &&
		is_player_id(attacker) &&
		is_player_id(victim))
	{
		dmg_addattack(victim, attacker, damage);
	}
}

public event_death_player () {
	if (!game_is_live() || is_restarting())
		return; 
	
	new const killer = read_data(1);
	new const victim = read_data(2);

	client_add_frag(killer);
	client_add_death(victim);

	new args[2];
	args[0] = victim;
	set_task(1.0, "printdmg_task", _, args, charsmax(args), "a", 1);
}

public event_round_restart() {
	restarting = true;

	if (!game_is_live() ||is_firstround())
		return;

	round_restarted();
}

public event_tt_win() {
	if (!game_is_live())
		return;

	teamtt_add_score();
}

public event_ct_win() {
	if (!game_is_live())
		return;

	teamct_add_score();
}

public event_round_end() {
	if (!game_is_live())
		return;

	g_iRound++;

	if (get_cvar_num("pug_dmgmode")) {
		new iPlayers[MAX_PLAYERS], iNum;
		get_players(iPlayers, iNum, "ach");
		
		for (new i;i < iNum;i++)  {
			new args[2];
			args[0] = iPlayers[i];
			set_task(1.0, "printdmg_task", _, args, charsmax(args), "a", 1);
		}
	}

	check_halfend();
	votepause_check(false);
}

public event_freezetime_end() {
	votepause_check(true);
	dmg_reset();
	set_task(0.8, "afkicker_newround_task");
}

// To show score when is gamebreak
public event_post_think(const id) {
	if (!is_gamebreak())
		return FMRES_IGNORED;

	set_pev(id, pev_maxspeed, 0.1)

	// We wait a time to dont overflow client_cmd and lag player
	if(nextPlayerThink[id] <= get_gametime())
	{
		client_cmd(id, "+showscores")
		nextPlayerThink[id] = get_gametime() + 0.2
	}

	return FMRES_SUPERCEDE;
}

stock get_loguser_index() {
	static loguser[80], name[32];

	read_logargv(0, loguser, 79);
	parse_loguser(loguser, name, 31);

	return get_user_index(name);
}

stock bool:is_restarting () {
	return restarting
}
