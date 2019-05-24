# Modo Competitivo 5v5 para Counter Strike 1.6

Tome lo mejor de varios plugins para hacerlo un poco mas automatico. Por ejemplo, inicia automaticamente al estar 10 jugadores en el servidor, kickea automaticamente a los AFK y muestra el daño que realizaste en una ronda justo luego de morir.

## Requerimientos

- [Amxmodx 1.8.x](https://www.amxmodx.org/)
- [Reapi 5.6.x](https://github.com/s1lentq/reapi)

## Para compilar y comprimir

En Linux: 

```bash
$ bash make.sh
```

## Para instalar solo descomprime en la raiz del HLDS ("/") el .zip creado por la compilacion 

```bash
$ unzip build-{date}.zip
```

## Comandos de chat

- Administrador<br>
	.start		- Forza el inicio del pug<br>
	.cancel		- Forza el final del pug<br>
	.manual		- El modo iniciara manualmente<br>
	.auto		- El modo iniciara automaticamente<br>

- Jugador<br>
	.dmg						- Muestra el daño hecho en la ronda<br>
	.hp							- Muestra la vida de los jugadores<br>
	.votekick <nombre> <razon>	- Inicia votacion para expulsar a un jugador<br>

## Cvars configurables (pugconfig.cfg)

pug_tag		"[Server]" // Prefix del servidor
pug_owner	"" // Nombre de los lideres del servidor

// Archivos de configuracion del servidor (/cstrike/addons/amxmodx/pug/)
pug_config_warmup		"warmup.rc"
pug_config_pugmode		"pugmode.rc"
pug_config_scrimmode	"scrimmode.rc"
pug_config_halftime		"halftime.rc"
pug_config_overtime		"overtime.rc"
pug_config_end			"end.rc"

pug_players			"10" // Cantidad de jugadores para iniciar el partido
pug_block_shield	"1" // Activa el bloqueo del escudo
pug_block_nvgs		"1" // Activa el bloqueo de los lentes nocturnos
pug_block_grenades	"1" // Activa el bloqueo de las granadas en el modo pregame
pug_block_last_maps "0" // Activa el bloqueo del ultimo mapa en la votacion de mapas
pug_rounds_max		"30" // Rondas maximas del partido
pug_rounds_ot		"6" // Rondas maximas del overtime
pug_vote_map		"1" // Activa la votacion de mapas
pug_vote_team		"1" // Activa la votacion de equipos
pug_vote_delay		"10.0" // Tiempo de duracion de cada votacion
pug_allow_tie		"0" // Activa el empate de la partida
pug_delay_end		"5.0" // Tiempo para mostrar el scoreboard al final de la partida
pug_intermission_time	"15" // Tiempo de descanso entre half
pug_absence_players		"2" // Cantidad de jugadores para cancelar el partido
pug_votekick_percent	"0.7" // Porcentaje de votos para patear a un jugador
pug_afktime				"90" // Tiempo AFK para kickear a un jugador
pug_manual				"0" // Habilita el modo manual (solo admines podrian iniciar el mod)
pug_show_money			"1" // 0 = Deshabilita; 1 = Mostrar dinero por chat; 2 = Mostrar dinero en hud; 3 = Mostrar dinero arriba de los jugadores
pug_allow_soundmsg		"1" // Habilita/Deshabilita el sonido hecho por los mensajes del say

## Inspirado en los siguientes plugins

[Sugisaki](https://amxmodx-es.com/Thread-Competitive-Face-it-Pick-Up-Game-PUG)
[PredatorFlys](https://amxmodx-es.com/Thread-Auto-Mix-YAP-Capitan-resubido)
[SmileYzn](https://github.com/SmileYzn/CS_PugMod)

