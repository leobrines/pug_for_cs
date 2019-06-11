#if defined _utils_included
  #endinput
#endif

#define _utils_included

#include <amxmodx>
#include <reapi>

#if AMXX_VERSION_NUM >= 183
	#define client_disconnect client_disconnected
	#define strbreak argbreak
#else
	#define MAX_PLAYERS 32
	#define MAX_NAME_LENGTH 32
	#define MENU_TIMEOUT -4
#endif

#define PUG_CMD_LVL	ADMIN_LEVEL_A
#define PUG_MOD_LVL	ADMIN_LEVEL_H

stock g_iStage
stock g_sCurrentMap[32]
stock g_iMaxClients
stock g_sLang[3]
stock TAG[32]

stock const team_name[TeamName][MAX_NAME_LENGTH];

enum _:PUGSTATE
{
	STAGE_DEAD = 0, 
	STAGE_WARMUP, 
	STAGE_START, 
	STAGE_FIRSTHALF, 
	STAGE_SECONDHALF, 
	STAGE_FIRSTOT, 
	STAGE_SECONDOT
}

stock utils_init () {
	formatex(team_name[TEAM_TERRORIST], charsmax(team_name[]), "%L",
				LANG_SERVER, "TERRORISTS")
	formatex(team_name[TEAM_CT], charsmax(team_name[]), "%L",
				LANG_SERVER, "CTS")

	formatex(team_name[TEAM_UNASSIGNED], charsmax(team_name[]), "Auto")
	formatex(team_name[TEAM_SPECTATOR], charsmax(team_name[]), "%L",
				LANG_SERVER, "SPECTATOR")
}

stock registerCommand (const szCommand[], const szFunction[], iFlags = 0, const szInfo[] = "")
{
	new szBuffer[16];
	formatex(szBuffer, charsmax(szBuffer), ".%s", szCommand);
	register_concmd(szBuffer, szFunction, iFlags, szInfo);
}

stock getPlayersNum ()
{
	new players[MAX_CLIENTS], count;
	get_players(players, count, "h");

	return count ? count : 0;
}

stock getPlayersTeam (TeamName:team, bool:bots=true)
{
	new players[MAX_PLAYERS], count;
	new tmp_team_name[10];

	switch (team) {
		case TEAM_CT:
			copy(tmp_team_name, charsmax(tmp_team_name), "CT");
		case TEAM_TERRORIST:
			copy(tmp_team_name, charsmax(tmp_team_name), "TERRORIST");
		default:
			copy(tmp_team_name, charsmax(tmp_team_name), "SPECTATOR");
	}

	get_players(players, count, bots ? "eh" : "ceh", tmp_team_name);

	return count ? count : 0;
}

/* Next version
stock client_cprint (const id, Colors:color, message[192])
{
	message_begin(MSG_ONE_UNRELIABLE, get_user_msgid("SayText"), _, id);
	write_byte(color ? (_:color) : 33);
	write_string(message);
	message_end();
}
*/

stock chat_print (const playerID, const message[], any:...)
{
	new msgFormated[191];
	vformat(msgFormated, charsmax(msgFormated), message, 3);
	
	if ( equali(TAG,"") )
		client_print_color(playerID, print_team_grey, "%s", msgFormated);
	else
		client_print_color(playerID, print_team_grey, "%s %s", TAG, msgFormated);
}

stock team_print (TeamName:team, const msg[], any:...) {
	new players[MAX_PLAYERS], count;
	new tmp_team[10];

	new msgFormated[191];
	vformat(msgFormated, charsmax(msgFormated), msg, 3);

	switch (team) {
		case TEAM_TERRORIST:
			copy(tmp_team, charsmax(tmp_team), "TERRORIST")
		case TEAM_CT:
			copy(tmp_team, charsmax(tmp_team), "CT")
		default:
			copy(tmp_team, charsmax(tmp_team), "SPECTATOR")
	}

	get_players(players, count, "ceh", tmp_team);

	for (new i; i<count; i++)
		chat_print(players[i], msgFormated);
}

stock bool:game_is_started ()
	return (g_iStage >= STAGE_START);

stock bool:game_is_live () {
	return (g_iStage >= STAGE_FIRSTHALF);
}

stock bool:team_is_full(TeamName:team, bool:bots=true)
	return (getPlayersTeam(team, bots) >= get_pug_maxplayers()/2);

stock team_count_players(TeamName:team) {
	new players[MAX_PLAYERS], count, tmp_team[10];

	switch (team) {
		case TEAM_TERRORIST:
			copy(tmp_team, charsmax(tmp_team), "TERRORIST")
		case TEAM_CT:
			copy(tmp_team, charsmax(tmp_team), "CT")
		default:
			copy(tmp_team, charsmax(tmp_team), "SPECTATOR")
	}

	get_players(players, count, "ceh", tmp_team);

	return count ? count : 0;
}

stock bool:client_is_player (id)
	return TEAM_TERRORIST <= client_get_team(id) <= TEAM_CT;

stock bool:client_is_spectator (id) 
	return client_get_team(id) == TEAM_SPECTATOR;

stock bool:client_is_terrorist (id)
	return client_get_team(id) == TEAM_TERRORIST;

stock bool:client_is_ct (id)
	return client_get_team(id) == TEAM_CT;

stock TeamName:client_get_team (id)
	return get_member(id, m_iTeam);

stock client_give_money (id, amount)
	set_member(id, m_iAccount, amount);

stock client_get_money (id)
	return get_member(id, m_iAccount);

stock clients_print_money () {
	static players[MAX_PLAYERS], count, id
	get_players(players, count, "ah");

	for (new i; i < count; i++) {
		id = players[i]
		client_cmd(id, "say_team $%i", client_get_money(id));
	}
}

stock clients_inteam_count () {
	static players[MAX_PLAYERS], count, count_all;

	count_all = 0;

	get_players(players, count, "ceh", "TERRORIST");
	count_all += count ? count : 0;

	get_players(players, count, "ceh", "CT");
	count_all += count ? count : 0;

	return count_all;
}

stock is_player_id (id)
	return 1 <= id <= MAX_PLAYERS+1;

stock client_set_team (const id, TeamName:team) {
	if (client_get_team(id) == team)
		return;

	set_task(5.0, "show_owners", id, _, _, "a", 1) 

	new TeamName:new_team;

	if (team == TEAM_SPECTATOR) {
		if (!is_spectator_allowed() && !access(id, PUG_CMD_LVL))
			chat_print(id, "%L", LANG_SERVER, "PUG_TEAMS_SPECTATORS");
		else
			client_join_team(id, team);

		return;
	} else if (team == TEAM_UNASSIGNED) {
		new_team = TeamName:random_num(_:TEAM_TERRORIST, _:TEAM_CT);
	} else {
		new_team = team;
	}

	if (team_is_full(new_team, true)) {
		if (is_user_bot(id))
			return;

		if (team_is_full(new_team, false)) {
			chat_print(id, "%L", LANG_SERVER, "PUG_TEAMS_FULL",
						team_name[new_team]);

			return;
		}
	}

	client_join_team(id, new_team);
}

stock client_join_team (id, TeamName:team) {
	rg_join_team(id, team);
	//rg_round_respawn(id);

	autoready_check();

	#if AMXX_VERSION_NUM < 183
	chatcolor_send_teaminfo(id);
	#endif
}

stock show_hudtitle(const id, msg[], any:...) {
	new fmt[50]
	vformat(fmt, charsmax(fmt), msg, 3)
	set_hudmessage(0, 255, 0, 0.23, 0.02, 0, 1.0, 1.1, 0.0, 0.0, 1)
	show_hudmessage(id, fmt);
}

stock show_hudbody(const id, msg[], any:...) {
	new fmt[512]
	vformat(fmt, charsmax(fmt), msg, 3)
	set_hudmessage(255, 255, 255, 0.23, 0.05, 0, 1.0, 1.1, 0.0, 0.0, 2)
	show_hudmessage(id, fmt);
}

stock server_changemap(sMap[])
{
	#if AMXX_VERSION_NUM >= 183
	engine_changelevel(sMap);
	#else
	server_cmd("changelevel %s", sMap);
	#endif
}
