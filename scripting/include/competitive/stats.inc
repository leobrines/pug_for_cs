static stats_enable;

public stats_init() {
	stats_enable = register_cvar("pug_csstats_enable", "0");
}

public disable_stats(){
	if (get_pcvar_num(stats_enable)) {
		set_cvar_num("csstats_pause", 1);
	}
}

public enable_stats(){
	if (get_pcvar_num(stats_enable)) {
		set_cvar_num("csstats_pause", 0);
	}
}

