/*********************************************************
*forked from github.com/edoardo/RF24RaspberryCommunicator*
*author:yasharne                                         *
*nesabian@gmail.com                                      *
*********************************************************/
#include <string>
//#include <getopt.h>
#include <unistd.h>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <RF24/RF24.h>

using namespace std;
//RF24 radio("/dev/spidev0.0",8000000 , 25);  
//RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

RF24 radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);
//const int role_pin = 7;
//const uint64_t pipes[3] = { 0xF0F0F0F0E1EE, 0xF0F0F0F0D2LL, 0xF0F0F0F0C3LL};
//const uint8_t pipes[][6] = {"1Node","2Node"};

// hack to avoid SEG FAULT, issue #46 on RF24 github https://github.com/TMRh20/RF24.git
unsigned long  got_message;

void setup(char choice){
	//Prepare the radio module
	radio.begin();
	radio.setRetries( 15, 15);
	//	radio.setChannel(0x4c);
	//	radio.setPALevel(RF24_PA_MAX);
	//	radio.setPALevel(RF24_PA_MAX);

	if (choice == 'm'){
		radio.openWritingPipe(0xF0F0F0F0E1EE);
		radio.openReadingPipe(1, 0xF0F0F0F0C3LL);
	} else if (choice == 'n'){
		radio.openWritingPipe(0xF0F0F0F0E1LL);
		radio.openReadingPipe(1, 0xF0F0F0F0D2LL);
	}
	//	radio.startListening();

}

bool sendMessage(int action){
	//This function send a message, the 'action', to the arduino and wait for answer
	//Returns true if ACK package is received
	//Stop listening
	radio.stopListening();
	unsigned long message = action;

	//Send the message
	bool ok = radio.write( &message, sizeof(unsigned long) );
//	if (!ok){
//		printf("failed...\n\r");
//	}else{
//		printf("ok!\n\r");
//	}	
	//Listen for ACK
	radio.startListening();
	//Let's take the time while we listen
	unsigned long started_waiting_at = millis();
	bool timeout = false;
	while ( ! radio.available() && ! timeout ) {
		//printf("%d", !radio.available());
		if (millis() - started_waiting_at > 1000 ){
			timeout = true;
		}
	}

	if( timeout ){
		//If we waited too long the transmission failed
//		printf("Oh gosh, it's not giving me any response...\n\r");
		printf("%lu\n", -1);
		return false;
	}else{
		//If we received the message in time, let's read it and print it
		radio.read( &got_message, sizeof(unsigned long) );
		printf("%lu\n", got_message);
		return true;
	}

}  

int main( int argc, char ** argv){

	char choice;
//	setup();
	bool switched = false;
	int counter = 0;

	//Define the options

	while(( choice = getopt( argc, argv, "m:n:")) != -1){
		setup(choice);
		while(switched == false && counter < 5){

			switched = sendMessage(atoi(optarg));
				
			counter ++;

			sleep(1);
		}

		//return 0 if everything went good, 2 otherwise
		if (counter < 5)
			return 0;
		else
			return 2;
	}
}
