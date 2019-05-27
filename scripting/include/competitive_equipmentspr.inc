// DISPLAY 

#define DISPLAY_ENTS_TASK 4563
stock bool:isNewRound;
stock Float:playerOrigin[33][3];
stock players[32], count;

// DISPLAY MONEY 

#define NUMBER_OF_SPRITES	5
stock money_sprites[MAX_PLAYERS+1][NUMBER_OF_SPRITES];
stock tmpstr[6], value[2];
stock dolar_sprite[MAX_PLAYERS+1];

// DISPLAY WEAPONS

#define NUMBER_OF_WEAPTYPES	7
stock const Float:pistols[6] = {1.0,10.0,11.0,16.0,17.0,26.0};
stock const Float:rifles[18] = {3.0,5.0,7.0,8.0,12.0,13.0,14.0,15.0,18.0,19.0,20.0,21.0,22.0,23.0,24.0,27.0,28.0,30.0};
stock weapon_sprites[33][NUMBER_OF_WEAPTYPES];
stock ArmorType:armor, weapons;

stock precache_equipment_models () {
	precache_model("sprites/10000.spr");
	precache_model("sprites/1000.spr");
	precache_model("sprites/100.spr");
	precache_model("sprites/10.spr");
	precache_model("sprites/1.spr");
	precache_model("sprites/weap.spr");
	precache_model("sprites/weap2.spr");
	precache_model("sprites/cash.spr");
	precache_model("sprites/arrow.spr");
}

stock show_team_equipment () {
	isNewRound = true;

	create_sprites();

	set_task(get_cvar_float("mp_freezetime"), "remove_sprites", _, _, _, "a", 1);
	set_task(0.2, "display_sprites", DISPLAY_ENTS_TASK, "", 0, "b");
}

stock create_sprites () {
	new id, players[MAX_PLAYERS], count;
	get_players(players, count, "ah");

	for (new i; i<count; i++) {
		id = players[i];

		if (is_user_playing(id))
			create_user_sprites(id);
	}
}

stock display_sprites () {
	new id;
	get_players(players, count, "ah");

	for (new i; i<count; i++) {
		id = players[i];

		if (is_user_playing(id))
			display_user_sprites(id);
		else
			remove_user_sprites(id);
	}

	isNewRound = false;
}

stock remove_sprites(){
	remove_task(DISPLAY_ENTS_TASK);

	for (new id; id < MAX_PLAYERS+1; id++)
		remove_user_sprites(id);
}

// CREATE SPRITES FUNCTIONS

stock create_user_sprites (id) {
	dolar_sprite[id]=rg_create_entity("env_sprite");

	for (new i; i < sizeof(money_sprites[]); i++)
		money_sprites[id][i] = rg_create_entity("env_sprite");

	for (new i; i < sizeof(weapon_sprites[]); i++)
		weapon_sprites[id][i] = rg_create_entity("env_sprite");
}

// REMOVE SPRITES FUNCTIONS

stock remove_user_sprites (id) {
	rg_remove_entity(dolar_sprite[id]);

	for (new i; i < sizeof(money_sprites[]); i++)
		rg_remove_entity(money_sprites[id][i])

	for (new i; i < sizeof(weapon_sprites[]); i++)
		rg_remove_entity(weapon_sprites[id][i])
}

// DISPLAY SPRITES FUNCTIONS

stock display_user_sprites (id) {
	display_money(id);
	display_weapons(id);
}

stock display_money (id) {
	for (new sprite; sprite < NUMBER_OF_SPRITES; sprite++){
		arrayset(tmpstr, 0, sizeof tmpstr);
		num_to_str(get_member(id, m_iAccount), tmpstr, charsmax(tmpstr))

		value[0]=tmpstr[sprite];
		value[1]=0;

		if (tmpstr[sprite])
			display_money_digit(id, sprite, floatstr(value), 255);
		else
			display_money_digit(id, sprite, 1.0, 0);
	}

	drawspr(dolar_sprite[id], id, "sprites/cash.spr", 1.0,34.0,255,0,255,0);
}

stock display_money_digit (id, sprite, Float:frame, amount) {
	switch (sprite) {
		case 0:
			drawspr(money_sprites[id][4], id, "sprites/10000.spr",
						frame,34.0,amount,0,255,0);
		case 1:
			drawspr(money_sprites[id][3], id, "sprites/1000.spr",
						frame,34.0,amount,0,255,0);
		case 2:
			drawspr(money_sprites[id][2], id, "sprites/100.spr",
						frame,34.0,amount,0,255,0);
		case 3:
			drawspr(money_sprites[id][1], id, "sprites/10.spr",
						frame,34.0,amount,0,255,0);
		case 4:
			drawspr(money_sprites[id][0], id, "sprites/1.spr",
						frame,34.0,amount,0,255,0);
	}
}

stock display_weapons (id) {
	weapons = pev(id, pev_weapons);

	display_pistol(id);
	display_rifle(id);
	display_grenades(id);
	display_armor(id);
	display_c4_or_defuse(id);
}

stock display_pistol (id) {
	for (new i; i < sizeof pistols; i++) {
		if(weapons & 1<<floatround(pistols[i])){
			drawspr(weapon_sprites[id][0],id,"sprites/weap.spr",pistols[i],50.0,255,255,255,0);
			return;
		}
	}

	drawspr(weapon_sprites[id][0],id,"sprites/weap.spr",pistols[0],50.0,0,255,255,0);
}	

stock display_rifle (id) {
	for(new i; i < sizeof rifles; i++) {
		if(weapons & 1<<floatround(rifles[i])){
			drawspr(weapon_sprites[id][1],id,"sprites/weap.spr",rifles[i],50.0,255,255,255,0);
			return;
		}
	}

	drawspr(weapon_sprites[id][1],id,"sprites/weap.spr",rifles[0],50.0,0,255,255,0);
}

stock display_grenades (id) {
	if (weapons & 1<<CSW_HEGRENADE)
		drawspr(weapon_sprites[id][2],id,"sprites/weap.spr",4.0,50.0,255,255,255,255);
	else
		drawspr(weapon_sprites[id][2],id,"sprites/weap.spr",4.0,50.0,0,255,255,255);
	
	if (weapons & 1<<CSW_FLASHBANG)
		drawspr(weapon_sprites[id][3],id,"sprites/weap.spr",25.0,50.0,255,255,255,255);
	else
		drawspr(weapon_sprites[id][3],id,"sprites/weap.spr",25.0,50.0,0,255,255,255);
	
	if (weapons & 1<<CSW_SMOKEGRENADE)
		drawspr(weapon_sprites[id][4],id,"sprites/weap.spr",9.0,50.0,255,255,255,255);
	else
		drawspr(weapon_sprites[id][4],id,"sprites/weap.spr",9.0,50.0,0,255,255,255);
}

stock display_armor (id) {
	rg_get_user_armor(id, armor);

	switch (armor) {
		case 2:
			drawspr(weapon_sprites[id][5],id,"sprites/weap2.spr",2.0,50.0,255,0,0,255);
		case 1:
			drawspr(weapon_sprites[id][5],id,"sprites/weap2.spr",1.0,50.0,255,0,0,255);
		default:
			drawspr(weapon_sprites[id][5],id,"sprites/weap2.spr",1.0,50.0,0,0,0,255);
	}
}

stock display_c4_or_defuse (id) {
	if (weapons & 1<<CSW_C4)
		drawspr(weapon_sprites[id][6],id,"sprites/weap2.spr",3.0,50.0,255,0,0,255);
	else if (get_member(id, m_bHasDefuser))
		drawspr(weapon_sprites[id][6],id,"sprites/weap2.spr",4.0,50.0,255,0,0,255);
	else
		drawspr(weapon_sprites[id][6],id,"sprites/weap2.spr",3.0,50.0,0,0,0,255);
}

stock drawspr (const ent, player_id, str[], Float:frame, Float:offset, amount, r, g, b) {
	if (!is_entity(ent))
		return;

	if (isNewRound){		
		engfunc(EngFunc_SetModel, ent, str);
		set_pev(ent, pev_movetype, MOVETYPE_NOCLIP);
		set_pev(ent, pev_framerate, 1.0);
		set_pev(ent, pev_scale, 0.3);
	}

	set_pev(ent, pev_frame, frame)
	rg_set_rendering(ent, kRenderFxNone, r,g,b, kRenderTransAdd, amount)
	pev(player_id, pev_origin, playerOrigin[player_id]);
	playerOrigin[player_id][2] += offset;
	set_pev(ent, pev_origin, playerOrigin[player_id]);
}
