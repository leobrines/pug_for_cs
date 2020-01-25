#if defined _globals_included
  #endinput
#endif

#define _globals_included

#define TASK_HUD_VOTE    996541
#define TASK_HUD_MONEY 3001
#define TASK_DISPLAY_INFO 4563

#define MAX_BYTES 192
#define MAX_MAPS 25

new g_iRound;

// Votation System
new g_iVoteId;
new g_iVotesCount

// VoteMap
new g_mMap
new g_iMapCount
new g_sMapNames[MAX_MAPS][32]
new g_iMapVotes[MAX_MAPS];
new g_sLastMaps[2][32]

// VoteTeam
new g_mTeam
new g_iTeamCount
new g_sTeamNames[2][32]
new g_iTeamVotes[32]

// Team names
new const g_szTeams[TeamName][MAX_NAME_LENGTH]
new const g_szTeams2[TeamName][MAX_NAME_LENGTH]