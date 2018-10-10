#include <SPI.h>
#include <RF24.h>
#include <LowPower.h>

#include "structs.h"

#define Wind 4; //Anschlusspin Windmesser
#define SLEEP_STAT 300;
#define IO_LED_INT 2
#define ClientNummer 1
#define RadioChannel 1
#define AutoAck 0

message msg;

static const uint64_t pipes[6] = {0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL, 0xF0F0F0F0C3LL, 0xF0F0F0F0B4LL, 0xF0F0F0F0A5LL, 0xF0F0F0F096LL};
//byte ClientNummer = 1;
//int RadioChannel = 1;
//int AutoAck = 0;
int WindDir_old = 0;
int WindSpeed_old = 0;
int BatStat_old = 0;

volatile bool ledflag = false;


//int WindSpeed = 12; //in m/s
//int WindDir = 180; //In Deg
//float Temp = 22.22; // in Grad Celsius
//bool StatBat = 1; // 1 = Akku voll; 0 = Akku leer



RF24 radio(6,7);

void setup() {

	msg.msg_id = 1;
	msg.WindSpeed = 12;
	msg.WindDir = 120;
	msg.Temp = 22.21;
	msg.BatStat = 1;
	// put your setup code here, to run once:
	Serial.begin(115200);
	if (initRadio())
		Serial.println("ERFOLG");
	else
		Serial.println("FAIL"); 


  //Batteriemessung starten
	ADMUX |= (1<<REFS0); //VCC als Referenzspannung für den AD-Wandler
	ADMUX |= (1<<MUX3) | (1<<MUX2) | (1<<MUX1);  //1.1V Referenzspannung als Eingang für ADC 
	delay(10);  //warten bis sich die Referenzspannung eingestellt hat
	ADCSRA |= (1<<ADEN);   //ADC aktivieren
	Serial.println("Spannungsmessung aktiviert");
}

void loop() {
	/*if (get_BatStat() < 2800) //wenn Spannung kleiner als 5V 
		msg.BatStat = 0;  
	else //wenn größer oder gleich 5V
		msg.BatStat = 1;
	//*/
	LowPower.powerDown(SLEEP_8S,ADC_OFF, BOD_OFF);
	send_msg('s');
	
	
	
	msg.BatStat = (get_BatStat() < 2800) ? 0 : 1;
	

	if (msg.WindDir != WindDir_old || 
		msg.WindSpeed != WindSpeed_old || 
		msg.BatStat != BatStat_old){
			send_msg('u');
			delay(2000);
	}
  
}

bool initRadio()
{
	if (!radio.begin())
		return 0;
	else
	{
		delay(20);
		radio.setChannel(RadioChannel);
		radio.setAutoAck(AutoAck);
		radio.setRetries(15,15);
		radio.setPALevel(RF24_PA_HIGH);

		radio.openWritingPipe(pipes[ClientNummer-1]);
		radio.openReadingPipe(1,pipes[0]);

		radio.startListening();
		delay(20);
		return 1;
	}
}

long get_BatStat()
{
	int adc_low, adc_high;  //Zwischenspeicher für die Ergebnisse des ADC
	long adc_result;  //Gesamtergebnis der Messung des ADC
	long vcc = 0;
	//Batteriespannung messen
	ADCSRA |= (1<<ADSC);  //Messung starten

	while (bitRead(ADCSRA, ADSC));  //warten bis Messung beendet ist
	//Ergebnisse des ADC zwischenspeichern. Wichtig: zuerst ADCL auslesen, dann ADCH
	adc_low = ADCL;
	adc_high = ADCH;

	adc_result = (adc_high<<8) | adc_low; //Gesamtergebniss der ADC-Messung 
	vcc = 1125300L / adc_result;  //Versorgungsspannung in mV berechnen (1100mV * 1023 = 1125300)
	return vcc;
}

void send_msg(char status)
{
	  initRadio();
	  delay(20);
	  radio.stopListening();
	  Serial.println("Daten senden");
	  msg.status = status;
	  Serial.println(msg.BatStat);
	  radio.write(&msg, sizeof(msg));
	  msg.msg_id++;
	  radio.startListening();
	  WindDir_old = msg.WindDir;
	  WindSpeed_old = msg.WindSpeed;
	  BatStat_old = msg.BatStat;
}
