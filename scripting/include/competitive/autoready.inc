#if defined _autoready_included
  #endinput
#endif

#define _autoready_included

#define TASK_HUD_READY 552214

stock autoready_check () {
	if (game_is_started() || is_server_manual())
		return;

	if (get_readyplayers() >= get_pug_maxplayers()) {
		autoready_hide();
		PugStart();
	} else {
		set_task(1.0, "autoready_keep", TASK_HUD_READY, _, _, "b")
	}
}

stock autoready_hide () {
	autoready_display(0.0);
	remove_task(TASK_HUD_READY);
}

stock autoready_display (Float:fHoldTime) {
	if (is_server_manual()) {
		show_hudmessage(0, "");
		return;
	}

	new iNeed = get_pug_maxplayers() - get_readyplayers();

	set_hudmessage(0, 255, 0, 0.8, 0.07, 0, 0.0, fHoldTime, 0.0, 0.0, 1)

	if (iNeed > 1)
		show_hudmessage(0, "%L", LANG_SERVER, "PUG_PLAYERS_MISSING", iNeed)
	else
		show_hudmessage(0, "%L", LANG_SERVER, "PUG_PLAYERS_MISSING2", iNeed)
}

public autoready_keep ()
	autoready_display(999.9);
