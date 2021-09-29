
#include "ai_functions.h"

void placeShip(json &msg);
void shootShot(json &msg);
void shotReturned(json &msg);

/**
 * @struct container
 * 
 * @brief container is used as a global variable for important items for you to keep track of.
 */
struct container{
    int  boardSize=10;
    char shipBoard[10][10];
    char shotBoard[10][10];
};
container gameVars;


/**
 * @brief Happens at the beginning of the battleships game only happens once (in 500 matches it only occurs once).
 * 
 * @param msg Used to give you the boardsize and for you to give your author name in response.
 *      json msg = {
 *          {"str", "10"}
 *      }
 */
void sendGameVars(json &msg){
    string temp = msg.at("str");
    gameVars.boardSize = stoi(temp);
    msg.at("str") = "Joey Gorski"; // Your author name(s) here
}

/**
 * @brief Parses the messageType, then calls the appropriate function. 
 * 
 * @param clientID The current AI's ID.
 * @param msg Contains which action to take.
 *      json msg = {
 *          {"messageType", "placeShip"}
 *      }
 */
void messageHandler(json &msg, string &clientID){
    if(msg.at("messageType")=="setupGame"){
        msg.at("client") = clientID;
        sendGameVars(msg);
        wipeBoards();   //populate boards with nothing
    }else if(msg.at("messageType")=="matchOver"){
        msg.at("client") = clientID;
        wipeBoards();   //clear the boards after each round
    }else if(msg.at("messageType")=="placeShip"){
        msg.at("client") = clientID;
        placeShip(msg);
    }else if(msg.at("messageType")=="shootShot"){
        msg.at("client") = clientID;
        shootShot(msg);
    }else if (msg.at("messageType")=="shotReturn"){ 
        shotReturned(msg);
    }else if(msg.at("messageType")=="shipDied"){ 
        //char board[10][10], int row, int col, int length, Direction dir, char newChar
        updateBoard(gameVars.shipBoard, msg.at("row"), msg.at("col"), msg.at("length"), msg.at("dir"), KILL);
    }else if (msg.at("messageType")=="killedShip"){ 
        //char board[10][10], int row, int col, int length, Direction dir, char newChar
        updateBoard(gameVars.shotBoard, msg.at("row"), msg.at("col"), msg.at("length"), msg.at("dir"), KILL);
    }
    
}

/**
 * @brief Fills the shipBoard and the shotBoard with WATER.
 */
void wipeBoards(){
    for(int row=0;row<gameVars.boardSize;row++){
        for(int col=0;col<gameVars.boardSize;col++){
            gameVars.shipBoard[row][col]=WATER;
            gameVars.shotBoard[row][col]=WATER;
        }
    }
}

/**
 * @brief The ship placement logic.
 * 
 * @param msg Contains the length of the ship, and must have the row, col, and dir be returned.
 *      json msg = {
 *          {"row", 0},
 *          {"col", 0},
 *          {"dir", HORIZONTAL},
 *          {"length", 4}
 *      }
 */
void placeShip(json &msg){ // This is an extremely bad-to-use algorithm in the long term--for more reasons than just the obvious. Try writing your own from scratch.
    int length = msg.at("length");
    for(int row=0;row<(gameVars.boardSize-length);row++){
        for(int col=0;col<(gameVars.boardSize-length);col++){
            if(gameVars.shipBoard[row][col]==WATER){
                int ship_okay = 1;
                msg.at("row") = row;
                msg.at("col") = col;
                msg.at("dir") = HORIZONTAL;
                for(int len=0;len<msg.at("length");len++){
                    if(gameVars.shipBoard[row][col]!=WATER){
                        ship_okay = 0;
                    }
                }
                if(ship_okay == 1){
                    updateBoard(gameVars.shipBoard, row, col, msg.at("length"), HORIZONTAL, SHIP);
                    return;
                }
            }
        }
    }
}

/**
 * @brief The shot taking logic.
 * 
 * @param msg Must have the row and col be returned.
 *      json msg = {
 *          {"row", 0},
 *          {"col", 0}
 *      }
 */
void shootShot(json &msg){
    for(int row=0;row<gameVars.boardSize;row++){
        for(int col=0;col<gameVars.boardSize;col++){
            if(gameVars.shotBoard[row][col]==WATER){
                msg.at("row") = row;
                msg.at("col") = col;
                updateBoard(gameVars.shotBoard, row, col, 1, NONE, SHOT);
                return;
            }
        }
    }
    msg.at("row") = 8;
    msg.at("col") = 8;
    updateBoard(gameVars.shotBoard, 8, 8, 1, NONE, SHOT);
}

/**
 * @brief shotReturned is called when there is a shot taken from either AI.
 * 
 * @param msg Contains the result of a shot at a specified row and col. Client is the AI who took the shot.
 *      json msg = {
 *          {"row", 0},
 *          {"col", 0},
 *          {"str", "HIT"},
 *          {"client", "1234"}
 *      }
 */
void shotReturned(json &msg){
    if(msg.at("client") == clientID){ // this is your AI's shot
        // do something with the message data here. 
    }else{ // this is your opponent's shot
        // do nothing
        // unless...?
    }
}
