#if defined _utils_included
  #endinput
#endif

#define _utils_included

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
	STAGE_SECONDOT,
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

stock get_readyplayers () {
	new count[2];

	count[0] = get_teamplayers_count(TEAM_TERRORIST);
	count[1] = get_teamplayers_count(TEAM_CT);

	return count[0] + count[1];
}

stock get_teamplayers_count (TeamName:team) {
	new players[MAX_PLAYERS], count;
	get_teamplayers(players, count, team);

	return count
}

stock get_teamplayers(team_players[MAX_PLAYERS], &c, TeamName:team)
{
	new players[MAX_PLAYERS], count;
	get_players(players, count, "ch");

	c = 0;

	for (new i=0; i < count; i++) {
		new id = players[i]
		new TeamName:pteam = client_get_team(id)

		if (team == pteam) {
			team_players[c] = id
			c++
		}
	}
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

stock chat_print (const id, const message[], any:...)
{
	new msgFormated[191];

	vformat(msgFormated, charsmax(msgFormated), message, 3);

	if ( equali(TAG,"") )
		client_print_color2(id, print_team_grey, "%s", msgFormated);
	else
		client_print_color2(id, print_team_grey, "%s %s", TAG, msgFormated);
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

stock team_to_str (TeamName:team, str[], size) {
	switch (team) {
		case TEAM_TERRORIST:
			format(str, size, "%s", "TERRORIST")
		case TEAM_CT:
			format(str, size, "%s", "CT");
		default:
			format(str, size, "%s", "SPECTATOR");
	}
}

stock bool:game_is_started ()
	return (g_iStage >= STAGE_START);

stock bool:game_is_live () {
	return (STAGE_FIRSTHALF <= g_iStage <= STAGE_SECONDOT);
}

stock bool:team_is_full(TeamName:team)
	return (get_teamplayers_count(team) >= get_pug_maxplayers()/2);

stock bool:team_can_vote(TeamName:team)
	return (get_teamplayers_count(team) > 2);

stock showt_hudtitle(TeamName:team, msg[], any:...) {
	new fmt[50]
	vformat(fmt, charsmax(fmt), msg, 3)

	new players[MAX_PLAYERS], num, strteam[12];
	team_to_str(team, strteam, charsmax(strteam));
	get_players(players, num, "ceh", strteam);

	for (new i; i<num; i++)
		show_hudtitle(players[i], fmt);
}

stock showt_hudbody(TeamName:team, msg[], any:...) {
	new fmt[50]
	vformat(fmt, charsmax(fmt), msg, 3)

	new players[MAX_PLAYERS], num, strteam[12];
	team_to_str(team, strteam, charsmax(strteam));
	get_players(players, num, "ceh", strteam);

	for (new i; i<num; i++)
		show_hudbody(players[i], fmt);
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

stock client_delete_c4(id) {
    new weaponID = get_weaponid("weapon_c4")
    new iconstatus = get_user_msgid("StatusIcon");

    user_has_weapon(id, weaponID, 0)

    message_begin(MSG_ONE,iconstatus,{0,0,0},id);
    write_byte(0); 
    write_string("c4");
    message_end();
}
