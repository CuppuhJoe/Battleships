CXXFLAGS 	= -std=c++11 -Wall -O3
MAKEFLAGS	+= --silent




all: controller
	cd AI_Files; make all; cd ..;

controller: controller.cpp server.cpp display.cpp defines.h
	echo "g++ $(CXXFLAGS) -o controller controller.cpp"
	g++ $(CXXFLAGS) -g -o controller controller.cpp;

clean:
	echo "rm -f controller ./AI_Executables/* ./logs/*.log serversocket"
	mv ./AI_Executables/01_bad_player.ai .; mv ./AI_Executables/02_dumb_player.ai .; mv ./AI_Executables/03_clean_player.ai .; mv ./AI_Executables/04_gambler_player.ai .; mv ./AI_Executables/05_learning_player.ai .;
	rm -f ./AI_Executables/* controller ./logs/*.log serversocket
	mv ./01_bad_player.ai ./AI_Executables/; mv ./02_dumb_player.ai ./AI_Executables/; mv ./03_clean_player.ai ./AI_Executables/; mv ./04_gambler_player.ai ./AI_Executables/; mv ./05_learning_player.ai ./AI_Executables/;



