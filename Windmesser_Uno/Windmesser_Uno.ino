#include <SPI.h>
#include <RF24.h>
#include "structs.h"
 
RF24 radio(6,7); 

//byte ClientNummer = 1;
int RadioChannel = 1;
int AutoAck = 0;
static const uint64_t pipes[6] = {0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL, 0xF0F0F0F0C3LL, 0xF0F0F0F0B4LL, 0xF0F0F0F0A5LL, 0xF0F0F0F096LL};
bool radioErfolg = false;
 
uint8_t ClientNummer;
message msg;

/*int WindSpeed; //in m/s
int WindDir; //In Deg
float Temp; // in Grad Celsius
bool StatBat;// 1 = Akku voll; 0 = Akku leer
*/
void setup() {
  // put your setup code here, to run once:
	Serial.begin(115200);

	Serial.print("Empfänger wird gestartet...");
	if (!radio.begin())
	{
		delay(20);
		Serial.println("  FAIL");
	}
	else
	{
		delay(20);
		Serial.println("  ERFOLG");
		radioErfolg = true;
  
		radio.setChannel(RadioChannel);
		Serial.print("Channel: "); Serial.println(RadioChannel);
  
		radio.setAutoAck(AutoAck);
		Serial.print("AutoAck: "); Serial.println(AutoAck);
  
		radio.setPALevel(RF24_PA_HIGH);
  
		radio.openReadingPipe(0, pipes[0]);
		Serial.println("openReadingPipe No.: 0");
  
		radio.openReadingPipe(1, pipes[1]);
		Serial.println("openReadingPipe No.: 1");
  
		radio.openReadingPipe(2, pipes[2]);
		Serial.println("openReadingPipe No.: 2");
  
		radio.openReadingPipe(3, pipes[3]);
		Serial.println("openReadingPipe No.: 3");
  
		radio.openReadingPipe(4, pipes[4]);
		Serial.println("openReadingPipe No.: 4");
  
		radio.openReadingPipe(5, pipes[5]);
		Serial.println("openReadingPipe No.: 5");
  
		radio.startListening();
		delay(20);
		Serial.println("Empfänger gestartet");
	}
}

void loop() {
	if (radioErfolg)
		if (radio.available(&ClientNummer))
		{
			radio.read(&msg, sizeof(msg));
			Serial.println();
			
			Serial.print("Neue Nachricht:");
			Serial.println(msg.msg_id);
      
			Serial.print(F("Clien No.: "));
			Serial.println(ClientNummer+1);
      
			Serial.print(F("Windgeschwindigkeit: "));
			Serial.println(msg.WindSpeed);
  
			Serial.print(F("Windrichtung: "));
			Serial.println(msg.WindDir);
      
			Serial.print(F("Temperatur: "));
			Serial.println(msg.Temp);
  
			Serial.print(F("Batteriestatus: "));
			if (msg.BatStat)
			{
				Serial.println(F("Batterie OK"));
				Serial.println(msg.BatStat);
			}
			else
			{
				Serial.println(F("Batterie fast leer"));
				Serial.println(msg.BatStat);
			}
		}
		
}
