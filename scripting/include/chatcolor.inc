#if defined _chatcolor_included
  #endinput
#endif
#define _chatcolor_included

#pragma reqlib chatcolor

/* ColorChat Support */
/* Replace in .sma line #include <colorchat> with #include <chatcolor> */
// #define NORMAL 0 // use client_print or start sentence with ^1 and set sender to same value as index
// #define GREEN 0 // start sentence with ^4
// #define TEAM_COLOR 0 // determine who is sender and set ti that id, or choose a color
#define RED Red
#define BLUE Blue
#define GREY Grey
#define ColorChat client_print_color
/* ColorChat Support */

enum
{
	Grey = 33,
	Red,
	Blue
}

/**
 * Sends coloured message to player. Set index to 0 to send text globaly..
 * An example would be: client_print_color(id, Red, "^4This is green ^3this is red, ^1this is your default chat text color");
 * Look at the above enum for second parameter lists when you don't pass a player index in it.
 *
 * @param index			This is the player index (1 to maxplayer) you want to send the message, use 0 to send to all players.
 * @param sender		This is the player index you want to use the team color, see above enum if you want to force a color.
 * @param fmt			Format string in which patterns gonna be replaced with argument list.
 * 
 * @return 				Returns 1 if the message has been sent, 0 if the index specified is a not connected player, or if a global message has not been sent because there are no humans players.
 */
native client_print_color(index, sender, const fmt[], any:...);

/**
 * Coloured register_dictionary version
 *
 * @param filename		Dictionary file to register.
 * 						The file should be in "addons/amxx/data/lang/"
 * 						only the name needs to be given.
 * 						(e.g. register_dictionary("file.txt") will be addons/amxx/data/file.txt).
 * 
 * @return 				Returns 1 on success, 0 if register_dictionary has returned 0 or if file can't be opened.
 */
native register_dictionary_colored(const filename[]);
