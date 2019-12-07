#if defined _moneysave_included
  #endinput
#endif

#define _moneysave_included

static clients_money[MAX_PLAYERS+1];
static frags[MAX_PLAYERS+1];
static deaths[MAX_PLAYERS+1];
static winsCT;
static winsTT;
static bool:is_round_restarted;

stock round_restarted ()
	is_round_restarted = true;

stock round_start () {
	if (is_round_restarted) {
		is_round_restarted = false;
		players_return_money();
	} else {
		players_save_money();
	}
}

stock players_save_money () {
	static players[MAX_PLAYERS], count, id;
	get_players(players, count, "h");

	for (new i; i<count; i++) {
		id = players[i];
		clients_money[id] = client_get_money(id);
	}
}

stock players_return_money () {
	static players[MAX_PLAYERS], count, id;
	get_players(players, count, "h");

	for (new i; i<count; i++) {
		id = players[i];
		rg_add_account(id, clients_money[id], AS_SET);
	}
}

stock client_add_frag (id)
	frags[id]++;

stock client_add_death (id)
	deaths[id]++;

stock client_reset_score (id) {
	frags[id] = 0;
	deaths[id] = 0;
}

stock clients_reset_scores () {
	new players[MAX_PLAYERS], count;
	get_players(players, count, "h");

	for (new i; i<count; i++)
		client_reset_score(players[i]);
}

stock client_get_frags (id)
	return frags[id];

stock client_get_deaths (id)
	return deaths[id];

stock teamtt_get_score ()
	return winsTT;

stock teamct_get_score ()
	return winsCT;

stock teams_reset_scores () {
	winsCT = 0;
	winsTT = 0;
}

stock bool:teamct_is_winning ()
	return winsCT > winsTT;

stock bool:teamtt_is_winning ()
	return winsTT > winsCT;

stock bool:teamtt_is_winner ()
	return winsTT > (get_maxrounds()/2);

stock bool:teamct_is_winner ()
	return winsCT > (get_maxrounds()/2);

stock bool:teamtt_is_winner_ot ()
	return winsTT > get_maxrounds()/2 + get_overtime_rounds()/2;

stock bool:teamct_is_winner_ot ()
	return winsCT > get_maxrounds()/2 + get_overtime_rounds()/2;

stock bool:is_scores_tie () {
	static maxrounds;
	maxrounds = (get_maxrounds()/2);

	return winsCT == maxrounds && winsTT == maxrounds;
}

stock teamtt_add_score () {
	winsTT++;
	write_score("TT", winsTT);
}

stock teamct_add_score () {
	winsCT++;
	write_score("CT", winsCT);
}

stock write_score (team[3], score) {
	emessage_begin(MSG_ALL, get_user_msgid("TeamScore"));
	ewrite_string(team);
	ewrite_short(score);
	emessage_end();
}

stock teams_switch () {
	teams_switch_side();
	teams_switch_score();
}

static teams_switch_side () {
	chat_print(0, "%L", LANG_SERVER, "PUG_TEAM_CHANGING")

	static playersTT[MAX_PLAYERS], countTT;
	static playersCT[MAX_PLAYERS], countCT;

	// Get actual teams
	get_players(playersTT, countTT, "eh", "TERRORIST");
	get_players(playersCT, countCT, "eh", "CT");

	for (new i; i < countTT; i++)
		rg_set_user_team(playersTT[i], TEAM_CT);

	for (new i; i < countCT; i++)
		rg_set_user_team(playersCT[i], TEAM_TERRORIST);
}

static teams_switch_score () {
	static tmp;

	tmp = winsCT;
	winsCT = winsTT;
	winsTT = tmp;
}
