# Battleships Contest Instructions
Basic steps to use our software for creating and battling Battleships AIs.

## Running The Controller
*This was written and tested on UNIX systems, so macOS might work, but if it does not or if you are running off of Windows, log into the remote workspace.
1. Pull this repository.
2. Compile the AIs and the controller using
```
make all
```
3. Run the controller using
```
./controller
```
Or with configs 
```
./controller -c ./Configs/defaults.txt
```
4. Follow the prompts to run the game, start a contest file's creation, or replay a previous game. 

## Making a New AI
1. Enter the AI_Files directory.
2. Make a copy or edit the **player_example.cpp** file.
3. Key Functions:
    - **sendGameVars**: In this function you should add your name(s) as the author of the file.
    - **messageHandler**: This is the function that is used to call other functions depending on the message type which is what the server wants the client to do. You can add your own function calls to this function if you want new utility.
    - **placeShip**: This is where you will write your ship placement code.
    - **shotReturned**: This is where you process the results of both your own and your opponent's shots.
    - **shootShot**: This is where you will write your code to shoot at the opponent.
4. Run the new AI using the steps above. 

## Known Issues
1. The display functionality does not display correctly on the Manjaro distro, but it does still display so it is not on the "must fix now" list
