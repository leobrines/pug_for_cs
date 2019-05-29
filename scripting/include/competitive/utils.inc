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
	STAGE_HALFTIME, 
	STAGE_SECONDHALF, 
	STAGE_OVERTIME, 
	STAGE_FINISHED
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
	new playersID[MAX_CLIENTS], amountPlayers, amountPlayers;
	get_players(playersID, amountPlayers, "h");

	return amountPlayers;
}

stock getPlayersTeam (TeamName:team, bool:includeBots=true)
{
	new playersID[MAX_CLIENTS], amountPlayers, playerID, amountPlayersInTeam;
	get_players(playersID, amountPlayers, includeBots ? "h" : "ch");
	 
	for (new i; i < amountPlayers; i++)
	{
		playerID = playersID[i]

		if (client_get_team(playerID) == team)
			amountPlayersInTeam++;
	}

	return amountPlayersInTeam;
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

stock bool:is_user_playing (const id) {
	if (is_user_connected(id) &&
		TEAM_TERRORIST <= client_get_team(id) <= TEAM_CT)
		return true;

	return false;
}

stock bool:game_is_started ()
	return STAGE_START <= g_iStage <= STAGE_FINISHED ? true : false;

stock bool:game_is_live ()
	return (g_iStage == STAGE_FIRSTHALF ||
			g_iStage == STAGE_SECONDHALF ||
			g_iStage == STAGE_OVERTIME) ? true : false;

stock bool:team_is_full(TeamName:team, bool:bots=true)
	return getPlayersTeam(team, bots) >= get_pug_maxplayers()/2 ? true : false

stock bool:client_is_spectator (id) 
	return client_get_team(id) == TEAM_SPECTATOR ? true : false;

stock bool:client_is_player (id)
	return TEAM_TERRORIST <= client_get_team(id) <= TEAM_CT ? true : false;

stock TeamName:client_get_team (id)
	return get_member(id, m_iTeam);

stock client_set_team (const id, TeamName:team) {
	if (client_get_team(id) == team)
		return;

	new TeamName:new_team;

	if (team == TEAM_SPECTATOR) {
		if (!is_spectator_allowed() && !access(id, PUG_CMD_LVL)) {
			chat_print(id, "%L", LANG_SERVER, "PUG_TEAMS_SPECTATORS");
		} else {
			fnNotReady(id)
			rg_join_team(id, team);
		}

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

	rg_join_team(id, new_team);
	rg_round_respawn(id);
	fnReady(id);
}
