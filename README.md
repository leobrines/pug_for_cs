# Modo Competitivo 5v5 para Counter Strike 1.6

Tome lo mejor de varios plugins para hacerlo un poco mas automatico. Por ejemplo, inicia automaticamente al estar 10 jugadores en el servidor, kickea automaticamente a los AFK y muestra el daño que realizaste en una ronda justo luego de morir.

## Requisitos

- [Amxmodx 1.8.x](https://www.amxmodx.org/)
- [Reapi 5.6.x](https://github.com/s1lentq/reapi)

## Pasos para instalar

1. Compilar y comprimir

En Linux: 

```bash
$ bash make.sh
```

2. Descomprime en la raiz del HLDS el zip creado por la compilacion 

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

pug_tag		"[Server]" // Prefix del servidor<br>
pug_owner	"" // Nombre de los lideres del servidor<br>

// Archivos de configuracion del servidor (/cstrike/addons/amxmodx/pug/)<br>
pug_config_warmup		"warmup.rc"<br>
pug_config_pugmode		"pugmode.rc"<br>
pug_config_scrimmode	"scrimmode.rc"<br>
pug_config_halftime		"halftime.rc"<br>
pug_config_overtime		"overtime.rc"<br>
pug_config_end			"end.rc"<br>

pug_players			"10" // Cantidad de jugadores para iniciar el partido<br>
pug_block_shield	"1" // Activa el bloqueo del escudo<br>
pug_block_nvgs		"1" // Activa el bloqueo de los lentes nocturnos<br>
pug_block_grenades	"1" // Activa el bloqueo de las granadas en el modo pregame<br>
pug_block_last_maps "0" // Activa el bloqueo del ultimo mapa en la votacion de mapas<br>
pug_rounds_max		"30" // Rondas maximas del partido<br>
pug_rounds_ot		"6" // Rondas maximas del overtime<br>
pug_vote_map		"1" // Activa la votacion de mapas<br>
pug_vote_team		"1" // Activa la votacion de equipos<br>
pug_vote_delay		"10.0" // Tiempo de duracion de cada votacion<br>
pug_allow_tie		"0" // Activa el empate de la partida<br>
pug_delay_end		"5.0" // Tiempo para mostrar el scoreboard al final de la partida<br>
pug_intermission_time	"15" // Tiempo de descanso entre half<br>
pug_absence_players		"2" // Cantidad de jugadores para cancelar el partido<br>
pug_votekick_percent	"0.7" // Porcentaje de votos para patear a un jugador<br>
pug_afktime				"90" // Tiempo AFK para kickear a un jugador<br>
pug_manual				"0" // Habilita el modo manual (solo admines podrian iniciar el mod)<br>
pug_show_money			"1" // 0 = Deshabilita; 1 = Mostrar dinero por chat; 2 = Mostrar dinero en hud; 3 = Mostrar dinero arriba de los jugadores<br>
pug_allow_soundmsg		"1" // Habilita/Deshabilita el sonido hecho por los mensajes del say<br>

## Cosas por agregar
- Añadir ronda a cuchillo
- Que los espectadores entren automaticamente por orden de llegada
- Bloquear modo espectador, solo para admins

## Inspirado en los siguientes plugins

[Sugisaki](https://amxmodx-es.com/Thread-Competitive-Face-it-Pick-Up-Game-PUG)<br>
[PredatorFlys](https://amxmodx-es.com/Thread-Auto-Mix-YAP-Capitan-resubido)<br>
[SmileYzn](https://github.com/SmileYzn/CS_PugMod)<br>

