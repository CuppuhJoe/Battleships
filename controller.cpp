#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fstream>
#include <regex>
#include <time.h>

#include "subprocess.hpp"
#include "server.cpp"
#include "display.cpp"
#include "defines.h"


using namespace std;
using namespace subprocess;


int main(int argc, char *argv[]){
    int waste=0;

    time_t t1;
    time_t t2;
    srand(time(NULL));

    // Edit how many players here
    const int playerCount = 50; // We assume size 50 as a max here but make this bigger if tournament requires it.

    /*
        make a list of all AI executable files
        call runGame() for those files and do tourney cr-arbage
    */
    Player players[playerCount];
    waste=system("ls ./AI_Executables | sort -nr > ais.txt");
    int size = 0;
    string line;
    ifstream ai_file ("ais.txt");
    if (ai_file.is_open())
    {
        while ( getline (ai_file,line) )
        {
            players[size].name   = line;
            players[size].author = "";
            players[size].wins   = 0;
            players[size].ties   = 0;
            players[size].losses = 0;
            players[size].lives = 3;
            players[size].gameWins = 0;
            
            size++;
        }
        ai_file.close();
    }
    
    regex int_expr("^[0-9]+$");
    regex doub_expr("^[0-9]*\\.[0-9]+$");

    string gameType = "";
    int numGames    = 500;
    int boardSize   = 10;
    int watchAll    = 1;
    int runChoice   = 1;
    double delay    = 0.3;
    string temp     = "";

    int aiChoiceOne = -1;
    int aiChoiceTwo = -1;

    // Handle Config file:
    bool configBool = false;
    if (argc > 1){
        if(strcmp(argv[1], "-c") == 0){
            configBool = true;
        }
    }
    line = "";
    int tracker = 0;
    if(configBool){
        ifstream conf_file (argv[2]);
        if (conf_file.is_open()) {
            while ( getline (conf_file,line) ) {
                if(line[0] != '#'){
                    if(tracker == 0){
                        gameType = line;
                        if(stoi(gameType) != 1 && stoi(gameType) != 2 && stoi(gameType) != 3){
                            cout << "Invalid gameType provided" << endl;
                            return 0;
                        }
                    } else if(tracker == 1){
                        numGames = stoi(line);
                    } else if(tracker == 2){
                        boardSize = stoi(line);
                        if(boardSize<7 || boardSize>10){
                            cout << "Invalid boardSize provided. Valid range 7-10." << endl;
                            return 0;
                        }
                    } else if(tracker == 3){
                        watchAll = stoi(line);
                        if(watchAll != 1 && watchAll != 2 && watchAll != 3){
                            cout << "Invalid watchAll provided" << endl;
                            return 0;
                        }
                    } else if(tracker == 4){
                        runChoice = stoi(line);
                        if(runChoice != 1 && runChoice != 2){
                            cout << "Invalid runChoice provided" << endl;
                            return 0;
                        }
                    } else if(tracker == 5){
                        delay = stod(line);
                    } else if(tracker == 6){ // error handling is done further down
                        for(int i=0; i<size; i++){
                            if(strcmp(players[i].name.c_str(), line.c_str()) == 0){
                                aiChoiceOne = i;
                                break;
                            }
                        }
                    } else if(tracker == 7){ // error handling is done further down
                        for(int i=0; i<size; i++){
                            if(strcmp(players[i].name.c_str(), line.c_str()) == 0){
                                aiChoiceTwo = i;
                                break;
                            }
                        }
                    }
                    tracker++;
                }
            }
            conf_file.close();
        } else {
            cout << "Config file doesn't exist" << endl;
            return 0;
        }
        if(aiChoiceOne == -1){
            cout << "AI_One doesn't exist" << endl;
            return 0;
        } else if(aiChoiceTwo == -1){
            cout << "AI_Two doesn't exist" << endl;
            return 0;
        } else if (aiChoiceOne == aiChoiceTwo){
            cout << "Invalid AI choice--AI cannot match" << endl;
            return 0;
        }

    }

    if(configBool == false){
        do{
            cout << "What do you want to run? [(1:test), 2:competition, 3:replay] ";
            getline(cin, gameType);
        }while (gameType!="1" && gameType!="2" && gameType!="3" && gameType!="");
    }
    
    if(gameType == "1" || gameType == ""){
        if(configBool == false) {
            //if 2 player ask for the starting values
                    // - flow or stutter step games
                    // - which AIs will battle
            
            cout << "How many games should the AI play? (default: 500) ";
            getline(cin, temp);
            if (regex_match(temp, int_expr)){
                numGames = stoi(temp);    
            }else if(temp != ""){
                cout << "The default value will be used." << endl;
            }

            cout << "What size should the board be? (default: 10) "; // Maybe force value of boardSize to be in range 7-10
            getline(cin, temp);
            if (regex_match(temp, int_expr)){
                boardSize = stoi(temp);    
            }else if(temp != ""){
                cout << "The default value will be used." << endl;
            }
            if(boardSize < 7 || boardSize > 10){
                cout << "The default value will be used. Boardsize to small or large. Valid range 7-10." << endl;
                boardSize = 10;
            }
            
            cout << "Watch all recorded games or only the last? [(1:last), 2:all, 3:none] ";
            getline(cin, temp);
            if (regex_match(temp, int_expr)){
                watchAll = stoi(temp);    
            }else if(temp != ""){
                cout << "The default value will be used." << endl;
            }

            if(watchAll!=3){
                cout << "Should the game run via time delay or directed input? [(1:delay), 2:input] ";
                getline(cin, temp);
                if (regex_match(temp, int_expr)){
                    runChoice = stoi(temp);    
                }else if(temp != ""){
                    cout << "The default value will be used." << endl;
                }

                if (runChoice == 1){
                    cout << "How long should the delay between actions be? (default: 0.3) ";
                    getline(cin, temp);
                    if (regex_match(temp, doub_expr) || regex_match(temp, int_expr)){
                        delay = stod(temp);    
                    }else if(temp != ""){
                        cout << "The default value will be used." << endl;
                    }
                }
            }
            
            
            cout << endl;
            for(int i=0;i<size;i++){
                cout << "[" << i << "] " << players[i].name << endl;
            }
            do{
                cout << endl << "Choose an AI to battle: ";
                getline(cin, temp);
                if (regex_match(temp, int_expr) && (stoi(temp)>=0 && stoi(temp)<size)){
                    aiChoiceOne = stoi(temp);    
                }
            }while(aiChoiceOne == -1);
            
            do{
                cout << "Choose another AI to battle: ";
                getline(cin, temp);
                if (regex_match(temp, int_expr) && (stoi(temp)>=0 && stoi(temp)<size)){
                    aiChoiceTwo = stoi(temp);    
                }
            }while(aiChoiceTwo == -1 || aiChoiceTwo == aiChoiceOne);
            cout << endl;
        }

        string clientNameOne = players[aiChoiceOne].name;
        string clientNameTwo = players[aiChoiceTwo].name;

        // cout << "clientNameOne: " << clientNameOne << endl
        //      << "clientNameTwo: " << clientNameTwo << endl;

        string matchFile = clientNameOne + "_vs_" + clientNameTwo + ".log";
        string remove = "rm -f ./logs/";
        string touch = "touch ./logs/"; 
        waste=system((remove + matchFile).c_str());
        waste=system((touch + matchFile).c_str());

        //start game
        cout << "Running..." << endl;

        time(&t1);
        waste = runGame(numGames, players[aiChoiceOne], players[aiChoiceTwo], boardSize, matchFile);
        time(&t2);
        sleep(5);

        if(watchAll != 3 && waste != -1){
            display(matchFile, watchAll, runChoice, delay);
        } else {
                        //display final stats in case of error
            cout << players[aiChoiceOne].name 
                 <<" RECORD W-L-T: " 
                 << players[aiChoiceOne].wins << " " 
                 << players[aiChoiceOne].losses << " " 
                 << players[aiChoiceOne].ties 
                 << endl;
            cout << players[aiChoiceTwo].name 
                 << " RECORD W-L-T: " 
                 << players[aiChoiceTwo].wins << " " 
                 << players[aiChoiceTwo].losses << " " 
                 << players[aiChoiceTwo].ties 
                 << endl;
        }
        
        
    }else if(gameType=="2"){
        //the game is a competition
        vector<Player> activePlayers; // players who are still in the game ready to play a match.
        vector<Player> passivePlayers; // players who have already played a match and are waiting for everyone else to finish.
        vector<Player> deadPlayers; // players who are deactivated and should not be considered for a match.
        
        for(int index=0; index<size; index++){
            activePlayers.push_back(players[index]);
        }

        string complogFile = "competition.comp.log";
        string remove = "rm -f ./logs/";
        string touch = "touch ./logs/"; 
        waste=system((remove + complogFile).c_str());
        waste=system((touch + complogFile).c_str());

        ofstream complog_stream("./logs/"+complogFile, std::ios_base::app);
        int round=0;

        //start game
        cout << endl << "Running..." << endl;

        time(&t1);

        while(activePlayers.size()+passivePlayers.size()>1){
            //COMPETITION_START_ROUND
            complog_stream << "COMPETITION_START_ROUND: " << round << endl;
            /* print player information
                // cout << "----------------------------------------------------------------------------" << endl;
                // cout << "Listing Player Data: " << endl;
                // for(int index=0; index<(int)activePlayers.size(); index++){
                //     cout << activePlayers[index].name << ": " << activePlayers[index].lives << endl;
                // }
                // cout << "----------------------------------------------------------------------------" << endl;
                // cout << endl; 
            */
            complog_stream << "ALIVE_PLAYERS" << endl;
            for(int index=0;index<(int)activePlayers.size(); index++){
                complog_stream << "name: "     << activePlayers[index].name     << endl
                               << "wins: "     << activePlayers[index].wins     << endl
                               << "ties: "     << activePlayers[index].ties     << endl
                               << "losses: "   << activePlayers[index].losses   << endl
                               << "lives: "    << activePlayers[index].lives    << endl
                               << "gameWins: " << activePlayers[index].gameWins << endl;
            }
            for(int index=0;index<(int)passivePlayers.size(); index++){
                complog_stream << "name: "     << passivePlayers[index].name     << endl
                               << "wins: "     << passivePlayers[index].wins     << endl
                               << "ties: "     << passivePlayers[index].ties     << endl
                               << "losses: "   << passivePlayers[index].losses   << endl
                               << "lives: "    << passivePlayers[index].lives    << endl
                               << "gameWins: " << passivePlayers[index].gameWins << endl;
            }
            complog_stream << "DEAD_PLAYERS" << endl;
            if(deadPlayers.size()>0){
                for(int index=0;index<(int)deadPlayers.size(); index++){
                    complog_stream << "name: "     << deadPlayers[index].name     << endl
                                   << "wins: "     << deadPlayers[index].wins     << endl
                                   << "ties: "     << deadPlayers[index].ties     << endl
                                   << "losses: "   << deadPlayers[index].losses   << endl
                                   << "lives: "    << deadPlayers[index].lives    << endl
                                   << "gameWins: " << deadPlayers[index].gameWins << endl;
                }
            }else{
                complog_stream << "none" << endl;
            }
            
            
            //play all of the players that are in active
            while(activePlayers.size()>1){
                //MATCH_START_ROUND: ai_one.ai_vs_ai_two.ai
                // randomly select two players to play a match
                int indexOne = rand()%activePlayers.size();
                int indexTwo;
                do {
                    indexTwo = rand()%activePlayers.size();
                } while(indexTwo == indexOne);
                
                // prepare a match to be played
                string clientNameOne = activePlayers[indexOne].name;
                string clientNameTwo = activePlayers[indexTwo].name;
                cout << "Playing " << clientNameOne << " and " << clientNameTwo << endl;
                string matchFile = clientNameOne + "_vs_" + clientNameTwo + ".log";
                waste=system((remove + matchFile).c_str());
                waste=system((touch + matchFile).c_str());

                // int prevc1Wins = activePlayers[indexOne].wins; // Could be pointer crap
                // int prevc2Wins = activePlayers[indexTwo].wins; // I want to go home
                
                // save the player data (then zero the old data) so the match can run without extra wins/losses
                int player1[]={activePlayers[indexOne].wins, activePlayers[indexOne].ties, activePlayers[indexOne].losses};
                int player2[]={activePlayers[indexTwo].wins, activePlayers[indexTwo].ties, activePlayers[indexTwo].losses};
                activePlayers[indexOne].wins=0;
                activePlayers[indexOne].ties=0;
                activePlayers[indexOne].losses=0;
                activePlayers[indexTwo].wins=0;
                activePlayers[indexTwo].ties=0;
                activePlayers[indexTwo].losses=0;

                // play a match
                waste = runGame(numGames, activePlayers[indexOne], activePlayers[indexTwo], boardSize, matchFile);
                
                //record the final match in the compfile
                ifstream match_stream ("./logs/"+matchFile);

                regex match_start("^MATCH_START_ROUND: " + to_string(numGames-1));
                bool found_match_start = false;
                string line;
                while(getline(match_stream, line)){
                    if(regex_match(line, match_start)){
                        found_match_start = true;
                    }
                    if(found_match_start == true){
                        complog_stream << line << endl;
                    }
                }

                match_stream.close();
                waste=system((remove + matchFile).c_str());

                
                // // figure out who won
                // int c1Wins = activePlayers[indexOne].wins-prevc1Wins;
                // int c2Wins = activePlayers[indexTwo].wins-prevc2Wins;

                //apply the wins/losses
                if (activePlayers[indexOne].wins > activePlayers[indexTwo].wins){
                    activePlayers[indexOne].gameWins += 1;
                    activePlayers[indexTwo].lives -= 1;
                }else if(activePlayers[indexOne].wins < activePlayers[indexTwo].wins){
                    activePlayers[indexOne].lives -= 1;
                    activePlayers[indexTwo].gameWins += 1;
                }

                // apply win loss statistics
                activePlayers[indexOne].wins+= player1[0]; 
                activePlayers[indexOne].ties+= player1[1];
                activePlayers[indexOne].losses+= player1[2];
                activePlayers[indexTwo].wins+= player2[0];
                activePlayers[indexTwo].ties+= player2[1];
                activePlayers[indexTwo].losses+= player2[2];
                


                // figure out where to put the players
                if (activePlayers[indexOne].lives<=0){
                    deadPlayers.push_back(activePlayers[indexOne]);
                }else{
                    passivePlayers.push_back(activePlayers[indexOne]);
                }
                if(activePlayers[indexTwo].lives<=0){
                    deadPlayers.push_back(activePlayers[indexTwo]);
                }else{
                    passivePlayers.push_back(activePlayers[indexTwo]);
                }

                //remove the already-played players
                if (indexOne > indexTwo){
                    activePlayers.erase(activePlayers.begin()+indexOne);
                    activePlayers.erase(activePlayers.begin()+indexTwo);
                }else{
                    activePlayers.erase(activePlayers.begin()+indexTwo);
                    activePlayers.erase(activePlayers.begin()+indexOne);
                }


                sleep(1);        
            }
            complog_stream << "COMPETITION_ROUND_OVER" << endl;
            cout << "Round " << round << " completed" << endl;
            //COMPETITION_ROUND_OVER
            
            //move the passive into the active
            activePlayers.insert(activePlayers.end(), passivePlayers.begin(), passivePlayers.end());
            passivePlayers.clear();
            round++;
        }
        complog_stream << "FINAL_PLAYER_FOUND" << endl;
        if(passivePlayers.size()!=0){
            cout << passivePlayers[0].name << " wins!" << endl;
        }else if(activePlayers.size()!=0){
            cout << activePlayers[0].name << " wins!" << endl;
        }
        complog_stream << "PRINT_ALIVE_PLAYERS" << endl;
        for(int index=0;index<(int)activePlayers.size(); index++){
            complog_stream  << "name: "     << activePlayers[index].name     << endl
                            << "wins: "     << activePlayers[index].wins     << endl
                            << "ties: "     << activePlayers[index].ties     << endl
                            << "losses: "   << activePlayers[index].losses   << endl
                            << "lives: "    << activePlayers[index].lives    << endl
                            << "gameWins: " << activePlayers[index].gameWins << endl;
        }
        for(int index=0;index<(int)passivePlayers.size(); index++){
            complog_stream  << "name: "     << passivePlayers[index].name     << endl
                            << "wins: "     << passivePlayers[index].wins     << endl
                            << "ties: "     << passivePlayers[index].ties     << endl
                            << "losses: "   << passivePlayers[index].losses   << endl
                            << "lives: "    << passivePlayers[index].lives    << endl
                            << "gameWins: " << passivePlayers[index].gameWins << endl;
        }
        complog_stream << "PRINT_DEAD_PLAYERS" << endl;
        if(deadPlayers.size()>0){
            for(int index=0;index<(int)deadPlayers.size(); index++){
                complog_stream  << "name: "     << deadPlayers[index].name     << endl
                                << "wins: "     << deadPlayers[index].wins     << endl
                                << "ties: "     << deadPlayers[index].ties     << endl
                                << "losses: "   << deadPlayers[index].losses   << endl
                                << "lives: "    << deadPlayers[index].lives    << endl
                                << "gameWins: " << deadPlayers[index].gameWins << endl;
            }
        }else{
            complog_stream << "none" << endl;
        }
        complog_stream << "END_SCREEN" << endl;
        complog_stream.close();
        time(&t2);

        //PRINT_ALIVE_PLAYERS
        //PRINT_DEAD_PLAYERS

        //END_SCREEN
        sleep(5);
    }else if(gameType=="3"){
        //grab the list of logfiles
        regex log("^.*.log$");
        regex comp("^.*.comp.log$");
        waste=system("ls ./logs | sort -nr > logs.txt");
        vector<string> logfiles;
        logfiles.clear();
        int logs_size = 0;
        string line;
        ifstream logs_file ("logs.txt");
        int log_choice = -1;

        if (logs_file.is_open())
        {
            while ( getline (logs_file,line) )
            {
                if(regex_match(line, log)){
                    logfiles.push_back(line);
                }
                
                logs_size++;
            }
            logs_file.close();
        }

        cout << endl;
        for(int i=0;i<logs_size;i++){
            cout << "[" << i << "] " << logfiles[i] << endl;
        }
        do{
            cout << endl << "Choose a file to replay: ";
            getline(cin, temp);
            if (regex_match(temp, int_expr) && (stoi(temp)>=0 && stoi(temp)<logs_size)){
                log_choice = stoi(temp);    
            }
        }while(log_choice == -1);

        cout << endl;
        
        if(regex_match(logfiles[log_choice], comp)){ //the chosen file is a competition
            cout << "Watch all recorded games or only the last? [(1:all), 2:last] ";
            getline(cin, temp);
            if (regex_match(temp, int_expr) && (stoi(temp)==1 || stoi(temp)==2)){
                watchAll = stoi(temp);    
            }else if(temp != ""){
                cout << "The default value will be used." << endl;
            }
        }else{ //the chosen file is a game
            cout << "Watch all recorded games or only the last? [(1:last), 2:all] ";
            getline(cin, temp);
            if (regex_match(temp, int_expr) && (stoi(temp)==1 || stoi(temp)==2)){
                watchAll = stoi(temp);    
            }else if(temp != ""){
                cout << "The default value will be used." << endl;
            }
        }

        cout << "Should the game run via time delay or directed input? [(1:delay), 2:input] ";
        getline(cin, temp);
        if (regex_match(temp, int_expr) && (stoi(temp)==1 || stoi(temp)==2)){
            runChoice = stoi(temp);    
        }else if(temp != ""){
            cout << "The default value will be used." << endl;
        }

        if (runChoice == 1){
            cout << "How long should the delay between actions be? (default: 0.3) ";
            getline(cin, temp);
            if (regex_match(temp, doub_expr) || regex_match(temp, int_expr)){
                delay = stod(temp);    
            }else if(temp != ""){
                cout << "The default value will be used." << endl;
            }
            cout << "delay: " << delay << endl;
        }

        if(regex_match(logfiles[log_choice], comp)){
            cout << "Playing competition..." << endl;
            time(&t1);
            display_competition(logfiles[log_choice], watchAll, runChoice, delay);
            time(&t2);
        }else{
            cout << "Playing game..." << endl;
            time(&t1);
            display(logfiles[log_choice], watchAll, runChoice, delay);
            time(&t2);
        }
    }else{
        cout << "Invalid gameType detected." << endl;
    }
    
    cout << "TIME: " << t2-t1 << endl;
    
    
    waste=system("rm -f ais.txt");
    waste=system("rm -f logs.txt");
    if(waste){
        //do nothing--this is just to stop a warning from popping up.
    }
    return 0;
}





