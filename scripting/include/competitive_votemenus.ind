#include <amxmodx>
#include <reapi>

stock pug_voted[33];
stock menu_temp_id[33];

// Votekick
stock displayMenuAllPlayers(menuID)
{
	new playersID[MAX_CLIENTS], amountPlayers, playerID;
	get_players(playersID, amountPlayers, "ch");

	for (new i; i < amountPlayers; i++)
	{
		playerID = playersID[i];
		displayMenuSinglePlayer(playerID, menuID)
	}
}

stock displayMenuToTeam(TeamName:team, menuID)
{
	new playersID[MAX_CLIENTS], amountPlayers, playerID;
	get_players(playersID, amountPlayers, "che", team == TEAM_TERRORIST ? "TERRORIST": "CT");

	for (new i; i < amountPlayers; i++)
	{
		playerID = playersID[i]
		displayMenuSinglePlayer(playerID, menuID)
	}
}

stock displayMenuSinglePlayer(const playerID, menuID)
{
	if (!is_user_connected(playerID))
		return;

	menu_temp_id[playerID] = menuID
	displayMenuClient(playerID)
}

stock displayMenuClient(const playerID)
{
	new oldMenu, newMenu;
	new isPlayerViewingMenu = player_menu_info(playerID, oldMenu, newMenu);

	if ( !isPlayerViewingMenu || oldMenu < 0 )
		menu_display(playerID, menu_temp_id[playerID]);

	else
		set_task(1.0, "displayMenuClient", playerID);
}

stock cancelMenuToTeam(TeamName:team, menuID)
{
	new playersID[MAX_CLIENTS], amountPlayers, playerID;
	get_players(playersID, amountPlayers, "che", team == TEAM_TERRORIST ? "TERRORIST": "CT");

	for (new i; i < amountPlayers; i++)
	{
		playerID = playersID[i]
		menu_cancel(playerID)
	}
}

stock cancelAnyMenu()
{
	new playersID[MAX_CLIENTS], amountPlayers, playerID;
	get_players(playersID, amountPlayers, "ch");

	for (new i; i < amountPlayers; i++)
	{
		playerID = playersID[i];
		menu_cancel(playerID);
	}
}

stock bool:isVoteComplete()
{
	new playersID[MAX_CLIENTS], amountPlayers, playerID;
	get_players(playersID, amountPlayers, "ch");

	for (new i; i < amountPlayers; i++)
	{
		playerID = playersID[i];
		if (!pug_voted[playerID])
			return false;
	}

	return true;
}
