#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "pvm3.h"


typedef enum PlayerStatus {
    IDLE, PLAYING, ELIMINATED, QUALIFIED
} PlayerStatus;
struct Player {
    char *name;
    PlayerStatus status;
    int coins;
};

typedef struct Player Player;

void initPlayers(Player *players, int count);

int qualifiedCount(Player *players, int count, int *winnerIndex);

Player *filterQualifiedPlayers(Player *players, int *playersCount);

void getCurrentPlayers(Player *players, int count, int *firstPlayerIndex, int *secondPlayerIndex);

void qualifyHighestPlayer(Player *players, int count);

int main(int argc, char *argv[]) {
    
	int playersCount, currentRound;
	Player *players;


    //Initialize Random Number Generator
    srand(time(NULL));

    //Generate Random Number of Players
    playersCount = 5 + rand() % 5;

    players = (Player *) malloc(sizeof(Player) * playersCount);

    initPlayers(players, playersCount);

    currentRound = 1;


	while (playersCount > 1) {
		int i;
		int numberOfMatches = 0;
		if (playersCount % 2) {
            printf("\nOdd: %d\n", playersCount);
            qualifyHighestPlayer(players, playersCount);
        }
		printf("\n~~~ Coins ~~~\n");
		for (i = 0 ; i < playersCount ; i++){
			printf("\n%s - Coins: %d", players[i].name, players[i].coins);
		}
        while (1) {
			int cc, tid;
            int firstPlayerIndex = -1, secondPlayerIndex = -1;
            int coins[4];

			getCurrentPlayers(players, playersCount, &firstPlayerIndex, &secondPlayerIndex);
            
			if (firstPlayerIndex == -1 || secondPlayerIndex == -1) break;
            
			players[firstPlayerIndex].status = players[secondPlayerIndex].status = PLAYING;

			coins[0] = firstPlayerIndex;
			coins[1] = players[firstPlayerIndex].coins;
			coins[2] = secondPlayerIndex;
			coins[3] = players[secondPlayerIndex].coins;


			cc = pvm_spawn("random_walk_match", (char**) 0, 0, "", 1, &tid);
			
			if (cc == 1){
				pvm_initsend(PvmDataDefault);
				pvm_pkint(coins, 4, 1);
				pvm_send(tid, 1);
				numberOfMatches++;
				printf("\nSpawned match no.%d\n", numberOfMatches);

			}else{

				printf("\nFailed to spawn child!\n");

				pvm_exit();
				return 0;

			}

        }

		for(i = 0 ; i < numberOfMatches ; i++){
			int cc, tid, qualifiedPlayerIndex, qualifiedPlayerCoins, eliminatedPlayerIndex, result[2];


			cc = pvm_recv(-1, -1);

			pvm_bufinfo(cc, (int * ) 0, &eliminatedPlayerIndex, &tid);

			pvm_upkint(result, 2, 1);

			qualifiedPlayerIndex = result[0];
			qualifiedPlayerCoins = result[1];

			players[qualifiedPlayerIndex].status = QUALIFIED;
			players[qualifiedPlayerIndex].coins = qualifiedPlayerCoins;
			players[eliminatedPlayerIndex].status = ELIMINATED;
			players[eliminatedPlayerIndex].coins = 0;

			printf("\nRound: %d - %s has eliminated %s\n", currentRound, players[qualifiedPlayerIndex].name,
                       players[eliminatedPlayerIndex].name);


		}

        currentRound++;
        players = filterQualifiedPlayers(players, &playersCount);
    }

    printf("\nWinner: %s\n", players[0].name);

    free(players); 
    
	return 0;
 
}

void getCurrentPlayers(Player *players, int count, int *firstPlayerIndex, int *secondPlayerIndex) {
    int foundFirstPlayer = -1;
	int i;
    for (i = 0; i < count; ++i) {
        Player currentPlayer = players[i];
        if (currentPlayer.status == IDLE) {
            if (foundFirstPlayer == -1) {
                foundFirstPlayer = 1;
                *firstPlayerIndex = i;
            } else {
                *secondPlayerIndex = i;
                break;
            }
        }
    }
}

void qualifyHighestPlayer(Player *players, int count) {
    int highestPlayerIndex = 0;
	int i;
    for (i = 0; i < count; ++i) {
        if (players[i].coins > players[highestPlayerIndex].coins) {
            highestPlayerIndex = i;
        }
    }
	printf("\nPlayer %s auto qualified with balance of %d", players[highestPlayerIndex].name, players[highestPlayerIndex].coins);
    players[highestPlayerIndex].status = QUALIFIED;
}

Player *filterQualifiedPlayers(Player *players, int *playersCount) {
    int newCount = 0;
    int i, j;
	Player *qualifiedPlayers;
	int currentPlayerIndex = 0;
	for (i = 0; i < (*playersCount); ++i) {
        if (players[i].status == QUALIFIED) {
            newCount += 1;
        }
    }
    
	qualifiedPlayers = (Player *) malloc(newCount * sizeof(Player));

    for (j = 0; j < (*playersCount); ++j) {
        Player currentPlayer = players[j];
        if (currentPlayer.status == QUALIFIED) {
            qualifiedPlayers[currentPlayerIndex].name = currentPlayer.name;
            qualifiedPlayers[currentPlayerIndex].status = IDLE;
            qualifiedPlayers[currentPlayerIndex].coins = currentPlayer.coins;
            currentPlayerIndex += 1;
        }
    }
   
	*playersCount = newCount;
   
	free(players);
    
    return qualifiedPlayers;
}

void initPlayers(Player *players, int count) {
    //Generate Names and Coins
	int i;
    for (i = 0; i < count; ++i) {
        int coins;
		char name[20];
		char counterString[2];
		coins = 10 + rand() % 10;
        strcpy(name, "Player - ");   
        sprintf(counterString, "%d", (i + 1));
        strcat(name, counterString);
        players[i].coins = coins;
        players[i].name = (char *) malloc(strlen(name));
        strcpy(players[i].name, name);
        players[i].status = IDLE;
    }
}
