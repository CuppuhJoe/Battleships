#!/usr/bin/env python3

# How to have file run as an executable: chmod 775 FILENAME.py

# Grabbing some constants similar to defines.h
# Shot related
SHOT = '@'
MISS = 'O'
DUPLICATE_SHOT  = '!'
HIT  = 'X'
KILL = 'K'
OPPONENT_SHOT = '*'
SHIP = 'S'
WATER = '~'
INVALID_SHOT = '\0'
PLACE_SHIP = 'Z'
# enum Direction
NONE=0
HORIZONTAL=1
VERTICAL=2

import os
import socket
import sys
import json
import copy


gameVars = {
    "boardSize": 0,
    "shipBoard": [],
    "shotBoard": [],
}

# DO NOT MODIFY THIS FUNCTION
def updateBoard(board, row, col, length, direction, newChar):
    if direction == HORIZONTAL:
        for len in range(length):
            gameVars[board][row][col+len] = newChar
    elif direction == VERTICAL:
        for len in range(length):
            gameVars[board][row+len][col] = newChar
    elif direction == NONE:
        gameVars[board][row][col] = newChar

# /**
#  * @brief Happens at the beginning of the battleships game only happens once (in 500 matches it only occurs once).
#  * 
#  * @param msg Used to give you the boardsize and for you to give your author name in response.
#  *      json msg = {
#  *          {"str", "10"}
#  *      }
#  */
def sendGameVars(msg):
    gameVars["boardSize"] = int(msg["str"])
    msg["str"] = "Matthew Bouch"

# /**
#  * @brief Fills the shipBoard and the shotBoard with WATER.
#  */
def wipeBoards():
    array = []
    gameVars["shipBoard"]=[]
    gameVars["shotBoard"]=[]
    for _ in range(gameVars["boardSize"]):
        array.append(WATER)
    for _ in range(gameVars["boardSize"]):
        gameVars["shipBoard"].append(copy.deepcopy(array))
        gameVars["shotBoard"].append(copy.deepcopy(array))
    return

# /**
#  * @brief The ship placement logic.
#  * 
#  * @param msg Contains the length of the ship, and must have the row, col, and dir be returned.
#  *      json msg = {
#  *          {"row", 0},
#  *          {"col", 0},
#  *          {"dir", HORIZONTAL},
#  *          {"length", 4}
#  *      }
#  */
def placeShip(msg): # This is an extremely bad-to-use algorithm in the long term--for more reasons than just the obvious one. Try writing your own from scratch.
    for row in range(gameVars["boardSize"]-msg["length"]):
        for col in range(gameVars["boardSize"]-msg["length"]):
            if(gameVars["shipBoard"][row][col] == WATER):
                msg["row"] = row
                msg["col"] = col
                msg["dir"] = HORIZONTAL
                for len in range(msg["length"]):
                    if(gameVars["shipBoard"][row][col] != WATER):
                        msg["dir"] = VERTICAL
                        updateBoard("shipBoard", row, col, msg["length"], VERTICAL, SHIP)
                        return
                updateBoard("shipBoard", row, col, msg["length"], HORIZONTAL, SHIP)
                return

# /**
#  * @brief The shot taking logic.
#  * 
#  * @param msg Must have the row and col be returned.
#  *      json msg = {
#  *          {"row", 0},
#  *          {"col", 0}
#  *      }
#  */
def shootShot(msg):
    for row in range(0, gameVars["boardSize"]):
       for col in range(0, gameVars["boardSize"]):
           if(gameVars["shotBoard"][row][col]==WATER):
               msg["row"]=row
               msg["col"]=col
               updateBoard("shotBoard", row, col, 1, NONE, SHOT)
               return
        
# /**
#  * @brief shotReturned is called when there is a shot taken from either AI.
#  * 
#  * @param msg Contains the result of a shot at a specified row and col. Client is the AI who took the shot.
#  *      json msg = {
#  *          {"row", 0},
#  *          {"col", 0},
#  *          {"str", "HIT"},
#  *          {"client", "1234"}
#  *      }
#  */
def shotReturned(msg):
    # if(msg["client"]==clientID): # this is your AI's shot
        # do something
    # else: # this is your opponant's shot
        # do nothing
        # unless...?
    pass

# /**
#  * @brief Parses the messageType, then calls the appropriate function. 
#  * 
#  * @param clientID The current AI's ID.
#  * @param msg Contains which action to take.
#  *      json msg = {
#  *          {"messageType", "placeShip"}
#  *      }
#  */
def messageHandler(msg, clientID):
    if(msg["messageType"] == "setupGame"):
        msg["client"]=clientID
        sendGameVars(msg)
        # Populating boards with blank data
        wipeBoards()
    elif(msg["messageType"] == "matchOver"):
        msg["client"]=clientID
        wipeBoards()
    elif(msg["messageType"] == "placeShip"):
        msg["client"]=clientID
        placeShip(msg)
    elif(msg["messageType"] == "shootShot"):
        msg["client"]=clientID
        shootShot(msg)
    elif(msg["messageType"] == "shotReturn"):
        shotReturned(msg)
    elif(msg["messageType"] == "shipDied"):
        updateBoard("shipBoard", msg["row"], msg["col"], msg["length"], msg["dir"], KILL)
    elif(msg["messageType"] == "killedShip"):
        updateBoard("shotBoard", msg["row"], msg["col"], msg["length"], msg["dir"], KILL)





# DO NOT MODIFY THIS FUNCTION
def main():
    global clientID
    
    # Create a UDS socket
    sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

    # Connect the socket to the port where the server is listening
    server_address = './serversocket'
    # print('connecting to {}'.format(server_address))
    try:
        sock.connect(server_address)
    except socket.error as msg:
        print(msg)
        sys.exit(1)


    # Setup Game Vars
    clientID = os.getpid()
    gameVars["boardSize"] = 10

    # Populating boards with blank data
    #wipeBoards()

    while True:

        msg = json.loads(sock.recv(1500))
        # print("Received:", msg)
        
        messageHandler(msg, clientID)
        
        toSend = (json.dumps(msg, separators=(',', ':'))).encode()
        # print("Sending:", toSend)

        sock.send(toSend)


        #break
    sock.close()

# DO NOT MODIFY THIS STATEMENT
if __name__ == "__main__":
    main()