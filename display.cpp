#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <regex>
#include <sstream>
#include <unistd.h>
#include <map>

#include "defines.h"
#include "conio.cpp"
#include "conio.h"



void displayTurn(turnStruct turnData, bool endDisplay);
int  extractInteger(string str);
void printBoard(turnStruct &turn, int boardChoice, bool endDisplay);
void slumber(double secs);
void display_competition(string compFile, int watchAll, int runChoice, double delay);
void print_vector_ai(vector<Player> players);

using namespace std;
using namespace conio;

int display(string matchFile, int watchAll, int runChoice, double delay){
    
    map<int, map<int, turnStruct>> matches;  // This maps the 50 matches that were played in the log file.
    map<int, turnStruct> match;              // This maps the individual turns in a single match. accessing a turn looks like match[x][y]
    map<int, int> matchNum;
    int matchIndex = 0;
    int turnIndex  = 0;
    
    regex match_start("^MATCH_START_ROUND:.*");
    regex match_end("^MATCH_OVER");
    regex matchWinLoss(".*WIN-LOSS-TIE:.*");

    cout << clrscr() << flush;

    string line;
    ifstream log_file("logs/"+matchFile);
    
    //print off dummy board
    cout << gotoRowCol(3, 1)   << "author1" << endl
         << gotoRowCol(4, 1)   << "client1"      << endl
         << gotoRowCol(6, 1)   << " |0123456789" << endl
         << gotoRowCol(7, 1)   << "------------" << endl
         << gotoRowCol(8, 1)   << "0|~~~~~~~~~~" << endl
         << gotoRowCol(9, 1)   << "1|~~~~~~~~~~" << endl
         << gotoRowCol(10, 1)  << "2|~~~~~~~~~~" << endl
         << gotoRowCol(11, 1)  << "3|~~~~~~~~~~" << endl
         << gotoRowCol(12, 1)  << "4|~~~~~~~~~~" << endl
         << gotoRowCol(13, 1)  << "5|~~~~~~~~~~" << endl
         << gotoRowCol(14, 1)  << "6|~~~~~~~~~~" << endl
         << gotoRowCol(15, 1)  << "7|~~~~~~~~~~" << endl
         << gotoRowCol(16, 1)  << "8|~~~~~~~~~~" << endl
         << gotoRowCol(17, 1)  << "9|~~~~~~~~~~" << endl;
        
    cout << gotoRowCol(3, 50)  << "author2" << endl
         << gotoRowCol(4, 50)  << "client2"      << endl
         << gotoRowCol(6, 50)  << " |0123456789" << endl
         << gotoRowCol(7, 50)  << "------------" << endl
         << gotoRowCol(8, 50)  << "0|~~~~~~~~~~" << endl
         << gotoRowCol(9, 50)  << "1|~~~~~~~~~~" << endl
         << gotoRowCol(10, 50) << "2|~~~~~~~~~~" << endl
         << gotoRowCol(11, 50) << "3|~~~~~~~~~~" << endl
         << gotoRowCol(12, 50) << "4|~~~~~~~~~~" << endl
         << gotoRowCol(13, 50) << "5|~~~~~~~~~~" << endl
         << gotoRowCol(14, 50) << "6|~~~~~~~~~~" << endl
         << gotoRowCol(15, 50) << "7|~~~~~~~~~~" << endl
         << gotoRowCol(16, 50) << "8|~~~~~~~~~~" << endl
         << gotoRowCol(17, 50) << "9|~~~~~~~~~~" << endl;
    
    string player1Record = "";
    string player2Record = "";

    if(log_file.is_open()){
        while(getline(log_file, line)){
            if(regex_match(line, match_start)){
                matchNum[matchIndex] = extractInteger(line);
                cout << gotoRowCol(1,1) << "Match Number: " << extractInteger(line) << endl;
                map<int, turnStruct> match;
                matches[matchIndex] = match;
                turnIndex = 0;

            }else if(regex_match(line, match_end)){
                cout << gotoRowCol(1,1) << "Match Ended!                   " << endl;
                matchIndex++;
            }else if(regex_match(line, matchWinLoss)){
                player1Record = line;
                getline(log_file, line);
                player2Record = line;
                break;
            }else{
                //save match to current game match
                
                //read in player1
                matches[matchIndex][turnIndex].player1.author = line;
                getline(log_file, line);
                matches[matchIndex][turnIndex].player1.name = line;
                getline(log_file, line);
                for(int i=0; i<10; i++){
                    matches[matchIndex][turnIndex].player1.board[i] = line;
                    getline(log_file, line);
                }

                matches[matchIndex][turnIndex].player1.shot = line;
                getline(log_file, line);

                //start reading in player2
                matches[matchIndex][turnIndex].player2.author = line;
                getline(log_file, line);
                matches[matchIndex][turnIndex].player2.name = line;
                getline(log_file, line);
                for(int i=0; i<10; i++){
                    matches[matchIndex][turnIndex].player2.board[i] = line;
                    getline(log_file, line);
                }

                matches[matchIndex][turnIndex].player2.shot = line;

                turnIndex++;
            }
        }
    }else{
        cerr << "There was an issue opening the logfile" << endl;
        return -1;
    }
    
    int curMatch = matchIndex-1;
    if(watchAll == 2){
        curMatch = 0;
    }
    for( ; curMatch<matchIndex; curMatch++){
        int turnsInMatch = matches[curMatch].size();
        cout << gotoRowCol(1,1) << "Match Number: " << matchNum[curMatch] << endl;
        cout << gotoRowCol(2,1) << "                                                     " << endl;
        cout << gotoRowCol(21,1) << "                                                     " << endl;
        int turnCount = 0;
        for( ; turnCount < turnsInMatch; ){
            displayTurn(matches[curMatch][turnCount], false);
            if(runChoice == 1){
                slumber(delay);
                turnCount++;
            }else{
                // handle input for going back
                cout << gotoRowCol(27, 1);
                char temp;
                temp = cin.get();
                if(temp != '\n'){
                    cin.ignore(1024, '\n');
                }
                cout << gotoRowCol(27, 1) << "                                                                                                                   ";
                if(temp == 'b' || temp == 'r'){
                    if(turnCount > 0){
                        turnCount--;
                    }
                }else{
                    if(turnCount < turnsInMatch){
                        turnCount++;
                    }
                }
            }
        }
        //handle end-game display
        cout << gotoRowCol(1,1) << "Match Ended!                                       " << endl;
        displayTurn(matches[curMatch][turnCount-1], true);
        if(curMatch!=matchIndex-1){
            char temp;
            temp = cin.get();
            if(temp != '\n'){
                cin.ignore(1024, '\n');
            }
        }
        
    }

    cout << gotoRowCol(23,1) << player1Record << endl;
    cout << gotoRowCol(24,1) << player2Record << endl;

    cout << gotoRowCol(30, 1) << "Press Enter to finish ";
    char temp;
    temp = cin.get();
    if(temp != '\n'){
        cin.ignore(1024, '\n');
    }

    return 0;
}

void display_competition(string compFile, int watchAll, int runChoice, double delay){
    vector<roundData> competition;

    regex comp_round_start("^COMPETITION_START_ROUND: [0-9]+");
    regex comp_round_end("^COMPETITION_ROUND_OVER");
    regex alive_players("ALIVE_PLAYERS");
    regex dead_players("DEAD_PLAYERS");
    regex match_start("^MATCH_START_ROUND:.*");
    regex match_end("^MATCH_OVER");
    regex match_win_loss(".*WIN-LOSS-TIE:.*");
    regex winner_found("FINAL_PLAYER_FOUND");
    regex print_alive_players("PRINT_ALIVE_PLAYERS");
    regex print_dead_players("PRINT_DEAD_PLAYERS");
    regex end_screen("END_SCREEN");
    
    regex player_name("^name: .+.[ai|py]");

    string state="comp_round_start";
    string line="";
    ifstream log_file("logs/"+compFile);

    if(log_file.is_open()){
        while(getline(log_file, line)){
            //handle state changing
            if(regex_match(line, comp_round_start)){
                state="comp_round_start";
            }else if(regex_match(line, comp_round_end)){
                state="comp_round_end";
            }else if(regex_match(line, alive_players)){
                state="alive_players";
            }else if(regex_match(line, dead_players)){
                state="dead_players";
            }else if(regex_match(line, match_start)){
                state="match_start";
            }else if(regex_match(line, match_end)){
                state="match_end";
            }else if(regex_match(line, match_win_loss)){
                state="match_win_loss";
            }else if(regex_match(line, winner_found)){
                state="winner_found";
            }else if(regex_match(line, print_alive_players)){
                state="print_alive_players";
            }else if(regex_match(line, print_dead_players)){
                state="print_dead_players";
            }else if(regex_match(line, end_screen)){
                state="end_screen";
            }

            //handle after the state changes
            if(state=="comp_round_start"){
                roundData tempObject;
                competition.push_back(tempObject);
            }else if(state=="comp_round_end"){
                //do nothing?        
            }else if(state=="alive_players" || state=="print_alive_players"){
                //check for the first of five lines of data, then process in each player individually
                Player temp_player;
                if(regex_match(line, player_name)){
                    temp_player.author = "alive";
                    temp_player.name = line.substr(5,-1);
                    getline(log_file, line);
                    temp_player.wins = stoi(line.substr(5,-1));
                    getline(log_file, line);
                    temp_player.ties = stoi(line.substr(5,-1));
                    getline(log_file, line);
                    temp_player.losses = stoi(line.substr(7,-1));
                    getline(log_file, line);
                    temp_player.lives = stoi(line.substr(6,-1));
                    getline(log_file, line);
                    temp_player.gameWins = stoi(line.substr(9,-1));
                    competition[competition.size()-1].player_data.push_back(temp_player);
                }        
            }else if(state=="dead_players" || state=="print_dead_players"){
                Player temp_player;
                if(regex_match(line, player_name)){
                    temp_player.author = "dead";
                    temp_player.name = line.substr(5,-1);
                    getline(log_file, line);
                    temp_player.wins = stoi(line.substr(5,-1));
                    getline(log_file, line);
                    temp_player.ties = stoi(line.substr(5,-1));
                    getline(log_file, line);
                    temp_player.losses = stoi(line.substr(7,-1));
                    getline(log_file, line);
                    temp_player.lives = stoi(line.substr(6,-1));
                    getline(log_file, line);
                    temp_player.gameWins = stoi(line.substr(9,-1));
                    competition[competition.size()-1].player_data.push_back(temp_player);
                }          
            }else if(state=="match_start"){
                int turnIndex=0;
                do{
                    if(regex_match(line, match_start)){
                        vector<turnStruct> match;
                        competition[competition.size()-1].matches.push_back(match);
                        turnIndex = 0;
                    }else if(regex_match(line, match_end)){
                        //matchIndex++;
                    }else if(regex_match(line, match_win_loss)){
                        WLTData temp;
                        temp.player1Record = line;
                        getline(log_file, line);
                        temp.player2Record = line;
                        competition[competition.size()-1].match_results.push_back(temp);
                        break;
                    }else{
                        
                        //create the turnStruct
                        turnStruct temp;
                        int compIndex=competition.size()-1;
                        int matchIndex=competition[compIndex].matches.size()-1;
                        competition[compIndex].matches[matchIndex].push_back(temp);                            

                        //save the player data
                        competition[compIndex].matches[matchIndex][turnIndex].player1.author = line;
                        getline(log_file, line);
                        competition[compIndex].matches[matchIndex][turnIndex].player1.name = line;
                        getline(log_file, line);
                        for(int i=0; i<10; i++){
                            competition[compIndex].matches[matchIndex][turnIndex].player1.board[i] = line;
                            getline(log_file, line);
                        }
                        competition[compIndex].matches[matchIndex][turnIndex].player1.shot = line;
                        getline(log_file, line);

                        //start reading in player2
                        competition[competition.size()-1].matches[matchIndex][turnIndex].player2.author = line;
                        getline(log_file, line);
                        competition[competition.size()-1].matches[matchIndex][turnIndex].player2.name = line;
                        getline(log_file, line);
                        for(int i=0; i<10; i++){
                            competition[competition.size()-1].matches[matchIndex][turnIndex].player2.board[i] = line;
                            getline(log_file, line);
                        }

                        competition[competition.size()-1].matches[matchIndex][turnIndex].player2.shot = line;

                        turnIndex++;
                    }
                }while(getline(log_file, line));   
            }else if(state=="winner_found"){
                roundData tempObject;
                competition.push_back(tempObject);
            }else if(state=="end_screen"){
                cout << "found the end screen" << endl;         
                log_file.close();
                break;
            }
        }
    }else{
        cerr << "There was an issue opening the logfile" << endl;
    }


    int currentCompRound=0;
    if(watchAll==2){
        currentCompRound=(int)competition.size()-2;
    }
    while(currentCompRound<(int)competition.size()){
        print_vector_ai(competition[currentCompRound].player_data);
        cout << gotoRowCol(27, 1);
        char temp;
        temp = cin.get();
        if(temp != '\n'){
            cin.ignore(1024, '\n');
        }
        cout << gotoRowCol(27, 1) << "                                                                                                                   "; 
        // display the matches
        for(int curMatch=0; curMatch<(int)competition[currentCompRound].matches.size(); curMatch++){
            cout << clrscr() << flush;
            //put in the board as well
            cout    << gotoRowCol(3, 1)   << "author1" << endl
                    << gotoRowCol(4, 1)   << "client1"      << endl
                    << gotoRowCol(6, 1)   << " |0123456789" << endl
                    << gotoRowCol(7, 1)   << "------------" << endl
                    << gotoRowCol(8, 1)   << "0|~~~~~~~~~~" << endl
                    << gotoRowCol(9, 1)   << "1|~~~~~~~~~~" << endl
                    << gotoRowCol(10, 1)  << "2|~~~~~~~~~~" << endl
                    << gotoRowCol(11, 1)  << "3|~~~~~~~~~~" << endl
                    << gotoRowCol(12, 1)  << "4|~~~~~~~~~~" << endl
                    << gotoRowCol(13, 1)  << "5|~~~~~~~~~~" << endl
                    << gotoRowCol(14, 1)  << "6|~~~~~~~~~~" << endl
                    << gotoRowCol(15, 1)  << "7|~~~~~~~~~~" << endl
                    << gotoRowCol(16, 1)  << "8|~~~~~~~~~~" << endl
                    << gotoRowCol(17, 1)  << "9|~~~~~~~~~~" << endl;
                
            cout    << gotoRowCol(3, 50)  << "author2" << endl
                    << gotoRowCol(4, 50)  << "client2"      << endl
                    << gotoRowCol(6, 50)  << " |0123456789" << endl
                    << gotoRowCol(7, 50)  << "------------" << endl
                    << gotoRowCol(8, 50)  << "0|~~~~~~~~~~" << endl
                    << gotoRowCol(9, 50)  << "1|~~~~~~~~~~" << endl
                    << gotoRowCol(10, 50) << "2|~~~~~~~~~~" << endl
                    << gotoRowCol(11, 50) << "3|~~~~~~~~~~" << endl
                    << gotoRowCol(12, 50) << "4|~~~~~~~~~~" << endl
                    << gotoRowCol(13, 50) << "5|~~~~~~~~~~" << endl
                    << gotoRowCol(14, 50) << "6|~~~~~~~~~~" << endl
                    << gotoRowCol(15, 50) << "7|~~~~~~~~~~" << endl
                    << gotoRowCol(16, 50) << "8|~~~~~~~~~~" << endl
                    << gotoRowCol(17, 50) << "9|~~~~~~~~~~" << endl;
            int turnsInMatch = competition[currentCompRound].matches[curMatch].size();
            cout << gotoRowCol(1,1) << "Match Number: " << 499 << endl;
            cout << gotoRowCol(2,1) << "                                                     " << endl;
            cout << gotoRowCol(21,1) << "                                                     " << endl;
            int turnCount = 0;
            for( ; turnCount < turnsInMatch; ){
                displayTurn(competition[currentCompRound].matches[curMatch][turnCount], false);
                if(runChoice == 1){
                    slumber(delay);
                    turnCount++;
                }else{
                    // handle input for going back
                    cout << gotoRowCol(27, 1);
                    char temp;
                    temp = cin.get();
                    if(temp != '\n'){
                        cin.ignore(1024, '\n');
                    }
                    cout << gotoRowCol(27, 1) << "                                                                                                                   ";
                    if(temp == 'b' || temp == 'r'){
                        if(turnCount > 0){
                            turnCount--;
                        }
                    }else{
                        if(turnCount < turnsInMatch){
                            turnCount++;
                        }
                    }
                }
            } 
            //handle end-game display
            cout << gotoRowCol(1,1) << "Match Ended!                                       " << endl;
            displayTurn(competition[currentCompRound].matches[curMatch][turnCount-1], true);
            cout << gotoRowCol(23,1) << competition[currentCompRound].match_results[curMatch].player1Record << endl;
            cout << gotoRowCol(24,1) << competition[currentCompRound].match_results[curMatch].player2Record << endl;
            if(curMatch!=(int)competition[currentCompRound].matches.size()){
                char temp;
                temp = cin.get();
                if(temp != '\n'){
                    cin.ignore(1024, '\n');
                }
            }
        }
        currentCompRound++;
    }
    print_vector_ai(competition[competition.size()-1].player_data);
    cout << gotoRowCol(1,1) << "WINNER AI";
    cout << gotoRowCol(26, 1) << "Exit final score display? (press Enter)";
    cout << gotoRowCol(27, 1);
    char temp;
    temp = cin.get();
    if(temp != '\n'){
        cin.ignore(1024, '\n');
    }
    cout << gotoRowCol(27, 1) << "                                                                                                                   " << endl;
}

void displayTurn(turnStruct turnData, bool endDisplay){
    if(!endDisplay){
        cout << gotoRowCol(3, 1) << turnData.player1.author << endl;
        cout << gotoRowCol(4, 1) << turnData.player1.name << "'s board                                                                                   " << endl;
        printBoard(turnData, 1, false);
        cout << gotoRowCol(19, 1) << "                                                                                                                   ";
        cout << gotoRowCol(19, 1) << turnData.player2.shot << endl;

        cout << gotoRowCol(3, 50) << turnData.player2.author << endl;
        cout << gotoRowCol(4, 50) << turnData.player2.name << "'s board                                                                                   " << endl;
        printBoard(turnData, 2, false);
        cout << gotoRowCol(19, 50) << "                                                                                                                   ";
        cout << gotoRowCol(19, 50) << turnData.player1.shot << endl;
    }else{
        cout << gotoRowCol(3, 1) << turnData.player1.author << endl;
        cout << gotoRowCol(4, 1) << "Final Status of " << turnData.player1.name << "'s board" << endl;
        printBoard(turnData, 1, true);
        cout << gotoRowCol(19, 1) << "                                                                                                                   ";
        cout << gotoRowCol(19, 1) << turnData.player2.shot << endl;
        

        cout << gotoRowCol(3, 50) << turnData.player2.author << endl;
        cout << gotoRowCol(4, 50) << "Final Status of " << turnData.player2.name << "'s board" << endl;
        printBoard(turnData, 2, true);
        cout << gotoRowCol(19, 50) << "                                                                                                                   ";
        cout << gotoRowCol(19, 50) << turnData.player1.shot << endl;

        if((turnData.player1.won && turnData.player2.won) || (!turnData.player2.won && !turnData.player1.won)){
            cout << gotoRowCol(21, 1) << "TIE!!!" << endl;
        }else if(turnData.player1.won){
            cout << gotoRowCol(21, 1) << turnData.player1.name << " WON!!!" << endl;
        }else if(turnData.player2.won){
            cout << gotoRowCol(21, 1) << turnData.player2.name << " WON!!!" << endl;
        }else{
            cout << gotoRowCol(21, 1) << "Something reeeeeeeally funky happened. I'd recommend an exorcist, but whatever floats your boat." << endl;
        }
    }
    
}

void print_vector_ai(vector<Player> players){
    vector<Player> sortedAlive;
    vector<Player> sortedDead;
    
    //sort the players vector into the sortedAlive and the sortedDead vectors
    while(players.size()!=0){
        // find the player with the highest gameWins if tied highest wins, then highest ties, then highest losses
        int highestIndex=0;
        for(int i=1; i<(int)players.size(); i++){
            if(players[i].gameWins > players[highestIndex].gameWins){
                highestIndex=i;
            }else if(players[i].gameWins == players[highestIndex].gameWins){
                if(players[i].wins > players[highestIndex].wins){
                    highestIndex=i;
                }else if(players[i].wins == players[highestIndex].wins){
                    if(players[i].ties > players[highestIndex].ties){
                        highestIndex=i;
                    }else if(players[i].ties == players[highestIndex].ties){
                        if(players[i].losses < players[highestIndex].losses){
                            highestIndex=i;
                        }
                    }
                }
            }
        }
        if (players[highestIndex].author == "alive"){
            sortedAlive.push_back(players[highestIndex]);
        }else{
            sortedDead.push_back(players[highestIndex]);
        }
        //remove the highestIndex from players
        players.erase(players.begin()+highestIndex);
    }

    // clear the screen
    cout << clrscr() << flush << endl;

    //print out the sorted vectors
    cout << gotoRowCol(1,1)  << "ALIVE AI"
         << gotoRowCol(1,25) << "Games"
         << gotoRowCol(1,33) << "Wins"
         << gotoRowCol(1,41) << "Ties"
         << gotoRowCol(1,49) << "Losses"
         << gotoRowCol(1,57) << "Lives";

    int indexRow=2;
    while(sortedAlive.size()!=0){
        Color currentHighlight = RESET;
        Color currentFG = BLACK;
        if(sortedAlive[0].lives == 3){
            currentHighlight = GREEN;
        }else if(sortedAlive[0].lives == 2){
            currentHighlight = YELLOW;
        }else if(sortedAlive[0].lives == 1){
            currentHighlight = RED;
            currentFG = WHITE;
        }
        cout << bgColor(currentHighlight) << fgColor(currentFG)
             << gotoRowCol(indexRow,1) << sortedAlive[0].name
             << bgColor(RESET) << fgColor(RESET)
             << gotoRowCol(indexRow,25) << sortedAlive[0].gameWins
             << gotoRowCol(indexRow,33) << sortedAlive[0].wins
             << gotoRowCol(indexRow,41) << sortedAlive[0].ties
             << gotoRowCol(indexRow,49) << sortedAlive[0].losses
             << gotoRowCol(indexRow,57) << sortedAlive[0].lives << endl;
        sortedAlive.erase(sortedAlive.begin());
        indexRow++;
    }

    indexRow++;
    cout << gotoRowCol(indexRow,1)  << "FALLEN AI"
         << gotoRowCol(indexRow,25) << "Games"
         << gotoRowCol(indexRow,33) << "Wins"
         << gotoRowCol(indexRow,41) << "Ties"
         << gotoRowCol(indexRow,49) << "Losses"
         << gotoRowCol(indexRow,57) << "Lives";
    indexRow++;
    while(sortedDead.size()!=0){
        cout << bgColor(BLACK)
             << gotoRowCol(indexRow,1) << sortedDead[0].name
             << bgColor(RESET)
             << gotoRowCol(indexRow,25) << sortedDead[0].gameWins
             << gotoRowCol(indexRow,33) << sortedDead[0].wins
             << gotoRowCol(indexRow,41) << sortedDead[0].ties
             << gotoRowCol(indexRow,49) << sortedDead[0].losses
             << gotoRowCol(indexRow,57) << sortedDead[0].lives << endl;
        sortedDead.erase(sortedDead.begin());
        indexRow++;
    }
}

int extractInteger(string str) { 
    stringstream ss;     
    ss << str; 
  
    string temp; 
    int found; 
    while (!ss.eof()) { 
        ss >> temp; 
        if (stringstream(temp) >> found) 
            return found;
        temp = ""; 
    }
    return -1; 
} 

void printBoard(turnStruct &turn, int boardChoice, bool endDisplay){
    if(boardChoice==1){
        int pos = turn.player2.shot.find(":");
        int currRow = (turn.player2.shot)[pos+2]-48;
        int currCol = (turn.player2.shot)[pos+4]-48;

        for (int row = 0; row < 10; row++){
            cout << gotoRowCol(row+8, 3); 
            for(int col=0; col < 10; col++){
                char ch = turn.player1.board[row][col];
                if(ch == WATER || (!endDisplay && ch == SHIP)){ 
                    if(row==currRow && col==currCol){
                        cout << bgColor(BLACK) << fgColor(CYAN);
                    }else{
                        cout << bgColor(CYAN) << fgColor(BLACK);
                    }
                    cout << WATER << flush;
                }else if(endDisplay && (ch == SHIP || ch == HIT)){
                    if(ch == HIT){
                        cout << bgColor(LIGHT_MAGENTA) << fgColor(BLACK);
                    }
                    else if(ch == SHIP){
                        cout << bgColor(WHITE) << fgColor(BLACK);
                    }
                    cout << ch << flush;
                    turn.player1.won = true;
                }else if(ch == HIT){
                    if(row==currRow && col==currCol && !endDisplay){
                        cout << bgColor(BLACK) << fgColor(LIGHT_MAGENTA);
                    }else{
                        cout << bgColor(LIGHT_MAGENTA) << fgColor(BLACK);
                    }
                    cout << ch << flush;
                }else if(ch == MISS){
                    if(row==currRow && col==currCol && !endDisplay){
                        cout << bgColor(BLACK) << fgColor(GRAY);
                    }else{
                        cout << bgColor(GRAY) << fgColor(BLACK);
                    }
                    cout << ch << flush;
                }else if(ch == KILL){
                    if(row==currRow && col==currCol && !endDisplay){
                        cout << bgColor(BLACK) << fgColor(RED);
                    }else{
                        cout << bgColor(RED) << fgColor(BLACK);
                    }
                    cout << ch << flush;
                }else if(ch == WALL){
                    cout << bgColor(RESET) << " " << flush;
                }else{
                    cout << bgColor(RESET) << fgColor(WHITE) << ch << flush;
                }
            }
            cout << endl;
        }
        cout << bgColor(RESET) << fgColor(WHITE);
    }else{
        int pos = turn.player1.shot.find(":");
        int currRow = (turn.player1.shot)[pos+2]-48;
        int currCol = (turn.player1.shot)[pos+4]-48;

        for (int row = 0; row < 10; row++){
            cout << gotoRowCol(row+8, 52); 
            for(int col=0; col < 10; col++){
                char ch = turn.player2.board[row][col];
                if(ch == WATER || (!endDisplay && ch == SHIP)){ 
                    if(row==currRow && col==currCol){
                        cout << bgColor(BLACK) << fgColor(CYAN);
                    }else{
                        cout << bgColor(CYAN) << fgColor(BLACK);
                    }
                    cout << WATER << flush;
                }else if(endDisplay && (ch == SHIP || ch == HIT)){
                    if(ch == HIT){
                        cout << bgColor(LIGHT_MAGENTA) << fgColor(BLACK);
                    }
                    else if(ch == SHIP){
                        cout << bgColor(WHITE) << fgColor(BLACK);
                    }
                    cout << ch << flush;
                    turn.player2.won = true;
                }else if(ch == HIT){
                    if(row==currRow && col==currCol && !endDisplay){
                        cout << bgColor(BLACK) << fgColor(LIGHT_MAGENTA);
                    }else{
                        cout << bgColor(LIGHT_MAGENTA) << fgColor(BLACK);
                    }
                    cout << ch << flush;
                }else if(ch == MISS){
                    if(row==currRow && col==currCol && !endDisplay){
                        cout << bgColor(BLACK) << fgColor(GRAY);
                    }else{
                        cout << bgColor(GRAY) << fgColor(BLACK);
                    }
                    cout << ch << flush;
                }else if(ch == KILL){
                    if(row==currRow && col==currCol && !endDisplay){
                        cout << bgColor(BLACK) << fgColor(RED);
                    }else{
                        cout << bgColor(RED) << fgColor(BLACK);
                    }
                    cout << ch << flush;
                }else if(ch == WALL){
                    cout << bgColor(RESET) << " " << flush;
                }else{
                    cout << bgColor(RESET) << fgColor(WHITE) << ch << flush;
                }
            }
            cout << endl;
        }
        cout << bgColor(RESET) << fgColor(WHITE);
    }
    
}

void slumber(double secs){
    const double SEC_TO_MICROSEC = 1000000.0;
    usleep(secs * SEC_TO_MICROSEC);
}