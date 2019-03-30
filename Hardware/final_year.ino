#include <StandardCplusplus.h>	//Library for all c++ data structures not included in Arduino
#include <vector>								//Library for vector data structure
#include <IRLibSendBase.h>    	//We need the base code
#include <IRLib_HashRaw.h>    	//Only use raw sender

using namespace std;						//Standard namespace for c++ methods

IRsendRaw mySender;							//Initialize IR sender object at digital pin 3

int tx=1;												//transmitter terminal
int rx=0;												//receiver terminal
char buf;												//buffer to hold received bytes from Bluetooth
vector<uint16_t> sample;				//Global Data structure to temporarily store IR code received through Bluetooth Module
int i=0;
int state = 0;

void setup() {
  Serial.begin(9600);						//Set BAUD rate for serial port
  delay(2000); while (!Serial);
	//Set transmitter and receiver terminal to INPUT and OUTPUT respectively
  pinMode(tx,OUTPUT);						
  pinMode(rx,INPUT);

}

void loop() {
    if(Serial.available() > 0){ // Checks whether data is comming from the serial port
      /*
			** The Android application sends elements in IR code as 2-Byte numbers with the last element being 1000.
			** Bluetooth module receives data 1 Byte at a time.
			** Reconstruct data by multiplying every second data received by 256 and summing it with preceeding data.
			** Store reconstructed code in sample vector.
			** If reconstructed data is 1000, call send_code() function.
			** After send_code() is completed, clear sample vector memory.	
		*/
    while(Serial.available()>0){

      if(i%2==0){
        state=(int) Serial.read();
      }
      else{
        state+=(int) Serial.read() * 256;
        sample.push_back(state);    
        state=0;
      }
      i++;
    Serial.println(i);
    Serial.println();
      delay(10);
      }
               
    } 
    if(sample[(i/2)-1]==1000){
        send_code();
        sample.clear();
        i=0;
      }   
    }

/*
Purpose:	This function formats the received infrared code into a format supported by the emitter library
					and then blasts the IR code at a rate of 36Hz.

args: None

Pre-conditions:	* sample vector is non-empty
								* IR emitter is up and connected to digital pin 3

Post-condition:	* IR signal is blasted through the IR emitter.
*/
void send_code(){
  uint16_t code[sample.size()];
  copy(sample.begin(),sample.end(),code);	//copy contents of sample vector into code array
  mySender.send(code,sample.size(),36);		//Pass the buffer,length, optionally frequency
//  Serial.println(F("sent"));
  delay(40); 
}
