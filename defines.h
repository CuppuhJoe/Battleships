/**
 * defines.h: lists globally defined values.
 * @author Stefan Brandle, May 2004
 * @author Joey Gorski, August 2020
 */

#ifndef DEFINES_H        // Double inclusion protection
#define DEFINES_H

using namespace std;

    // Shot related
    const char SHOT = '@';
    const char MISS = '*';
    const char WALL = 'W';
    const char DUPLICATE_SHOT  = '!';
    const char DUPLICATE_HIT  = 'H';
    const char HIT  = 'X';
    const char KILL = 'K';
    const char SHIP = 'S';
    const char WATER = '~';
    const char INVALID_SHOT = char(0);
    const char PLACE_SHIP = 'Z';

    const int MAX_BOARD_SIZE = 10;
    const int MAX_SHIP_SIZE = 5;
    const int MIN_SHIP_SIZE = 3;

    enum Direction { NONE=0, HORIZONTAL=1, VERTICAL=2 };

    // Struct information
    struct Player {
        string name;
        string author;
        int wins;
        int ties;
        int losses;
        int lives;
        int gameWins;
    };
    struct GameInfo {
        Player player1;
        Player player2;
        bool error;
    };
    struct playerData{
        string name    = "ERROR HERE     ";
        string author  = "ERROR HERE     ";
        string board[10];
        string shot    = "ERROR HERE     ";
        bool   won     = false;
    };
    struct turnStruct{
        playerData player1;
        playerData player2;
    };
    struct WLTData{
        string player1Record="";
        string player2Record="";
    };
    struct roundData{
        vector<Player> player_data;
        vector<vector<turnStruct>> matches; 
        vector<WLTData> match_results;
    };
#endif
