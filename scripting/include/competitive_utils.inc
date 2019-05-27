#include <amxmodx>
#include <reapi>
#include <colorchat>
#include <fakemeta>

#if AMXX_VERSION_NUM >= 183
	#define client_disconnect client_disconnected
	#define strbreak argbreak
#else
	#define MAX_PLAYERS 32
	#define MAX_NAME_LENGTH 32
	#define MENU_TIMEOUT -4
#endif

#include <competitive_reapi>
#include <competitive_cvars>
#include <competitive_equipmentspr>

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

stock TAG[32]

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

		if (get_member(playerID, m_iTeam) == team)
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
