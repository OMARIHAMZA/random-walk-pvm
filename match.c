#include <stdio.h>
#include <time.h>
#include "pvm3.h"

int main(){

	int cc, ptid;

	int coins[4]; 
	/* 
	[0] First Player Index
	[1] First Player Coins 
	[2] Second Player Index 
	[3] Second Player Coins 
	*/
	int result[2];
	/*
	[0] Winner Player Index
	[1] Winner Player Coins
	*/

	int firstPlayerCoins, secondPlayerCoins, eliminatedPlayerIndex;

	ptid = pvm_parent();

	cc = pvm_recv(ptid, -1);

	pvm_bufinfo(cc, (int *) 0, (int *) 0, &ptid);
	
	pvm_upkint(coins, 4, 1);

	//Initialize Random Number Generator
    srand(time(NULL));

	firstPlayerCoins = coins[1];
	secondPlayerCoins = coins[3];


	while (firstPlayerCoins > 0 && secondPlayerCoins > 0) {
        int randomNumber = rand() % 10;
		if (randomNumber >= 5) {
		 firstPlayerCoins += 1;
		 secondPlayerCoins -= 1;
        } else {
		 firstPlayerCoins -= 1;
		 secondPlayerCoins += 1;
        }
    }

	if(firstPlayerCoins == 0){ //First Player has been eliminated

		result[0] = coins[2];
		result[1] = secondPlayerCoins;
		eliminatedPlayerIndex = coins[0];


	}else{ // Second player has been eliminated
	
		result[0] = coins[0];
		result[1] = firstPlayerCoins;
		eliminatedPlayerIndex = coins[2];
	
	}

	pvm_initsend(PvmDataDefault);

	pvm_pkint(result, 2, 1);

	pvm_send(ptid, eliminatedPlayerIndex);

	pvm_exit();

	return 0;

}