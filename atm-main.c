#include "atm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <unistd.h>
#include <math.h>

// Default port and ip address are defined here
#define FAILURE 255   
#define DEFAULT_AUTH_FILE "bank.auth"
#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT "3000"
#define DEFAULT_PORT_NUM 3000


int main(int argc, char** argv){
  	
	char *auth_file = NULL;
	char *ip_addr = NULL;  
	char *port = NULL;
	char *card = NULL;
	char *account = NULL;
	char mode = '\0';
	char *amount = NULL;
	
	size_t file_len;
	uint port_num;
	float amount_num;
	
	//Might be wrong max args
	if(argc > 13){
		return FAILURE;
	}

	//Section: parse command line input 
	int cflag;
	regex_t valid_account;
	regex_t valid_numbers;
	regex_t valid_filenames;
	regex_t valid_ip;
	regex_t valid_card;
	//Compile Regex patterns
	regcomp(&valid_account, "^[a-zA-Z0-9_-]*$", REG_EXTENDED);
	regcomp(&valid_numbers, "^(0|[1-9][0-9]*)$", REG_EXTENDED);
	regcomp(&valid_filenames, "^([a-zA-Z0-9._\\-]*)$", REG_EXTENDED); 
	regcomp(&valid_ip, "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?).){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$", REG_EXTENDED);
	regcomp(&valid_card, "^([a-zA-Z0-9._\\-]*).card$", REG_EXTENDED);
	while ((cflag = getopt(argc, argv, "p:s:i:a:c:n:d:w:g::")) != -1) {
		switch(cflag){
			case 'a':
				if(account == NULL){
					account = optarg;
				} else {
					return FAILURE;
				}
				if(regexec(&valid_account, account, 0, NULL, 0) != 0){
					return FAILURE;
				}
				break;

			case 's':
				if(auth_file == NULL){
					auth_file = optarg;
				} else {
					return FAILURE;
				}
				if(regexec(&valid_filenames, auth_file, 0, NULL, 0) != 0){
					return FAILURE;
				}
				file_len = strlen(auth_file);
				if(file_len < 1 || file_len > 127){
					return FAILURE;
				}
				//strcmp returns 0 if match
				if(strcmp(auth_file, ".") == 0 || strcmp(auth_file, "..") == 0){
					return FAILURE;
				}
				break;

			case 'i':
				if(ip_addr == NULL){
					ip_addr = optarg;
				} else {
					return FAILURE;
				}
				if(regexec(&valid_ip, ip_addr, 0, NULL, 0) != 0){
					return FAILURE;
				}
				break;

			case 'p':
				if(port == NULL){
					port = optarg;
				} else {
					return FAILURE; // duplicate
				}
				//port # did not match regex
				if(regexec(&valid_numbers, optarg, 0, NULL, 0) != 0){
					return FAILURE;
				}
				break;

			case 'c':
				if(card == NULL){
					card = optarg;
				} else {
					return FAILURE;
				}
				if(regexec(&valid_card, card, 0, NULL, 0) != 0){
					return FAILURE;
				}
				file_len = strlen(card);
				if(file_len < 1 || file_len > 127){
					return FAILURE;
				}
				//strcmp returns 0 if match
				if(strcmp(card, ".") == 0 || strcmp(card, "..") == 0){
					return FAILURE;
				}
				break;

			case 'n':
				if(!mode){
					mode = 'n';
					amount = optarg;
				} else {
					return FAILURE;
				}
				break;

			case 'd':
				if(!mode){
					mode = 'd';
					amount = optarg;
				} else {
					return FAILURE;
				}
				break;

			case 'w':
				if(!mode){
					mode = 'w';
					amount = optarg;
				} else {
					return FAILURE;
				}
				break;

			case 'g':
				if(!mode){
					mode = 'g';
					amount = "N/A";
				} else {
					return FAILURE;
				}
				break;

			case '?':
				return FAILURE;
		}

	}	
	// if(account == NULL){
	// 	return FAILURE;
	// }
	if(auth_file == NULL){
		auth_file = DEFAULT_AUTH_FILE;
	}
	if(ip_addr == NULL){
		ip_addr = DEFAULT_IP;
	}
	if(port == NULL){
		port = DEFAULT_PORT;
		port_num = DEFAULT_PORT_NUM;
	} else {
		port_num = atoi(port); 
		if (port_num <= 1024 && port_num >= 65535){
			return FAILURE;
		}
	}
	if(card == NULL){
		card = malloc(strlen(account));
		strcpy(card, account); 
		strcat(card, ".card"); 
	}
	if(!mode){
		return FAILURE; 
	}
	switch (mode) {
		case 'n':
			amount_num = atof(amount); 
			amount_num = roundf(amount_num * 100) / 100; // round to 2 decimal places
			if (amount_num < 10) {
				return FAILURE;
			}
			sprintf(amount, "%f", amount_num);
			break;
		case 'd':
			amount_num = atof(amount); 
			amount_num = roundf(amount_num * 100) / 100; // round to 2 decimal places
			if (amount_num <= 0) {
				return FAILURE;
			}
			sprintf(amount, "%f", amount_num);
			break;
		case 'w':
			amount_num = atof(amount); 
			amount_num = roundf(amount_num * 100) / 100; // round to 2 decimal places
			if (amount_num <= 0) {
				return FAILURE;
			}
			sprintf(amount, "%f", amount_num);
			break;
		case 'g':
			break;
	}
	printf("Account: %s\n", account);
	printf("Auth file: %s\n", auth_file);
	printf("IP: %s\n", ip_addr);
	printf("Port: %s\n", port);
	printf("Card: %s\n", card);
	printf("Mode: %c\n", mode);
	printf("Amount: %s\n", amount);
	/* send messages */

	char buffer[] = "Hello I am the atm and I would like to have money please";
	int msg_size = sizeof(account) + sizeof(auth_file) + sizeof(ip_addr) + sizeof(port) + sizeof(card) + sizeof(amount) + 7;
	char *msg = malloc(msg_size);
	int j = snprintf(msg, msg_size, "%s %s %s %s %s %c %s", account, auth_file, ip_addr, port, card, mode, amount);
	printf("msg: %s\n", msg);
	
	ATM *atm = atm_create(ip_addr, port_num);
	atm_send(atm, msg, j);
	//atm_send(atm, buffer, sizeof(buffer));
	atm_recv(atm, buffer, sizeof(buffer));
	printf("atm received %s\n", buffer);
	
	atm_free(atm);
	

	// Implement how atm protocol will work: sanitizing inputs and using different modes of operations

	return EXIT_SUCCESS;
}
