#define USE_REAPI

#if AMXX_VERSION_NUM >= 183
	#define client_disconnect client_disconnected
	#define register_cvar create_cvar
	#define strbreak argbreak
#else
	#define MAX_PLAYERS 32
	#define MAX_NAME_LENGTH 32
	#define MENU_TIMEOUT -4
#endif

#if defined USE_REAPI
    #include <reapi>
    
    stock TeamName:client_get_team (id)
        return get_member(id, m_iTeam);

    stock client_set_team (id, TeamName:team)
        rg_set_user_team(id, team)

    stock client_give_money (id, amount)
        set_member(id, m_iAccount, amount);

    stock client_get_money (id)
        return get_member(id, m_iAccount);

    stock client_set_money (id, amount)
        rg_add_account(id, amount, AS_SET);

    stock client_drop_c4(id)
        rg_drop_item(id, "weapon_c4");
#else
    #include <cstrike>
    #include <engine>
    
    enum TeamName
    {
        TEAM_UNASSIGNED,
        TEAM_TERRORIST,
        TEAM_CT,
        TEAM_SPECTATOR
    };

    stock TeamName:client_get_team (id)
        return TeamName:cs_get_user_team(id);

    stock client_set_team (id, TeamName:team)
        cs_set_user_team(id, CsTeams:team);

    stock client_give_money (id, amount)
        cs_set_user_money(id, client_get_money(id) + amount)

    stock client_get_money (id)
        return cs_get_user_money(id)

    stock client_set_money (id, amount)
        cs_set_user_money(id, amount)

    stock client_drop_c4(id)
        engclient_cmd(id, "drop", "weapon_c4")

#endif

