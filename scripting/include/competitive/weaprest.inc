public restrict_weapons_pregame() {
	server_cmd("amx_restrict off")
	server_cmd("amx_restrict load pug/weaprest_pregame.ini")
}

public restrict_weapons_live() {
	server_cmd("amx_restrict off")
	server_cmd("amx_restrict load pug/weaprest_live.ini")
}

