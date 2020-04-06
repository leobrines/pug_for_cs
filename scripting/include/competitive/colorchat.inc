#if defined _colorchat_included
  #endinput
#endif

#define _colorchat_included

#include <amxmodx>

stock client_print_color2(id, color, msg[], any:...)
{
	static msgfmt[191]
	vformat(msgfmt, charsmax(msgfmt), msg, 4);

	replace_color_abrv(msgfmt, charsmax(msgfmt))

	client_print_color(id, color, msgfmt)
}

stock replace_color_abrv(msg[], size) {
	replace_all(msg, size, "!g", "^4");
	replace_all(msg, size, "!y", "^1");
	replace_all(msg, size, "!t", "^3");
}

#if AMXX_VERSION_NUM < 183

enum
{
	print_team_grey = 33,
	print_team_red,
	print_team_blue
}

stock client_print_color(id, color, msg[], any:...)
{
	static szMsg[191], msgSayText;

	if (!msgSayText)
		msgSayText = get_user_msgid("SayText");

	vformat(szMsg, charsmax(szMsg), msg, 4);

	message_begin(id ? MSG_ONE_UNRELIABLE : MSG_BROADCAST, msgSayText, .player = id);
	write_byte(color ? color : 33);
	write_string(szMsg);
	message_end();
}

public chatcolor_send_teaminfo(id)
{
	if (!is_user_connected(id))
		return;
    
	static msgTeamInfo;

	if (!msgTeamInfo)
		msgTeamInfo = get_user_msgid("TeamInfo");

	static const pTeams[][] = { "", "TERRORIST", "CT" };

	for (new pid = 0; pid < 3; pid++)
	{
		message_begin(MSG_ONE_UNRELIABLE, msgTeamInfo, .player = id)
		write_byte(pid+33)
		write_string(pTeams[pid])
		message_end()
	}
}
#endif
