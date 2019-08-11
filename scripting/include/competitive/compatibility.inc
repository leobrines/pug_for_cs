#if AMXX_VERSION_NUM >= 183
	#define client_disconnect client_disconnected
	#define register_cvar create_cvar
	#define strbreak argbreak
#else
	#define MAX_PLAYERS 32
	#define MAX_NAME_LENGTH 32
	#define MENU_TIMEOUT -4
#endif
