# Modo Competitivo 5v5 para Counter Strike 1.6

Tome lo mejor de varios plugins para hacerlo un poco mas automatico. Por ejemplo, inicia automaticamente al estar 10 jugadores en el servidor, kickea automaticamente a los AFK y muestra el daño que realizaste en una ronda justo luego de morir.

## Caracteristicas 
- Inicio de juego automatico
- Pausar partido (.votepause)
- Mutear y desmutear (.mute .unmute)
- Expulsar jugador (.votekick)
- DMG automatico en consola
- Empate disponible y configurable
- Mantiene puntuaciones al cambio de bando
- Al cambiar bando, se mantienen los scores
- Los vivos pueden leer a los muertos

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
	.votekick <nombre>	- Votacion para expulsar a un jugador<br>
	.votepause			- Votacion para pausar partido<br>
	.mute <nombre>		- Mutear jugador<br>
	.unmute <nombre>	- Desmutear jugador<br>

## Configuraciones destacables  (pugconfig.cfg)

pug_tag		"[Server]" // Prefix del servidor<br>
pug_owner	"" // Nombre de los lideres del servidor<br>
pug_rounds_max		"30" // Rondas maximas del partido<br>
pug_rounds_ot		"6" // Rondas maximas del overtime<br>
pug_allow_tie		"0" // Activa el empate de la partida<br>
pug_show_money			"1" // Muestra el dinero. 0 = Deshabilita; 1 = Por chat; 2 = Por HUD; 3 = Por sprites<br>
pug_votepause_time			"60" // Tiempo que duran las pausas

## Cosas por hacer
- Añadir ronda a cuchillo y configurar por cvar
- Que los espectadores entren automaticamente por orden de llegada
- Bloquear modo espectador, solo para admins
- Refactorizar todo el codigo por modulos (includes)
- Banear solamente de la ronda en juego, cuando se da votekick

## Inspirado en los siguientes plugins

[Sugisaki](https://amxmodx-es.com/Thread-Competitive-Face-it-Pick-Up-Game-PUG)<br>
[PredatorFlys](https://amxmodx-es.com/Thread-Auto-Mix-YAP-Capitan-resubido)<br>
[SmileYzn](https://github.com/SmileYzn/CS_PugMod)<br>

