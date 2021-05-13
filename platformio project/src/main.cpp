//ESP32 WIRING
// BUSY -> 4, RST -> 16, DC -> 17, CS -> SS(5), CLK -> SCK(18), DIN -> MOSI(23), GND -> GND, 3.3V -> 3.3V

#define ENABLE_GxEPD2_GFX 0
#include <Wire.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBoldOblique24pt7b.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSansBold50pt7b.h>
#include <Fonts/FreeSansBold100pt7b.h>
#include <Fonts/Lato_Bold_40.h>
#include <esp_wifi.h>
#include <esp_bt.h>
#include <WiFi.h>
#include <string> 
#include <iostream>
#include <sstream>
using namespace std;

//#include <Fonts/FAT.h>

#include <WiFi.h>
#include <HTTPClient.h>
#if defined(ESP32)

GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display(GxEPD2_420(/*CS=5*/ SS, /*DC=*/17, /*RST=*/16, /*BUSY=*/4));

#endif

#include "GxEPD2_boards_added.h"

#if !defined(__AVR) && !defined(_BOARD_GENERIC_STM32F103C_H_)

#endif
const char *ssid = "openplotter";
const char *password = "freewifi";
const char three[] = "123";
const char four[] = "1234";
const char seven[] = "0123456";
const char tsleep[] = " ";
const int buttonPin = 15; // the number of the pushbutton pin
int buttonState = 0;	  // variable for reading the pushbutton status

int cmp = 0;

int mode = 0;
int oldmode=1;

void dualScreen(int val1, int val2, String str1, String str2)
{

	display.setPartialWindow(0, 0, display.width(), display.height());
	display.setRotation(0);
	display.setFont(&FreeSansBold50pt7b);
	display.setTextColor(GxEPD_BLACK);

	
	do
	{
		display.fillScreen(GxEPD_WHITE);

		display.setCursor(20, 170);
		display.print(val1);
		display.setCursor(200, 170);
		display.print(val2);
		display.setFont(&FreeMonoBoldOblique24pt7b);
		display.setCursor(40, 235);
		display.print(str1);
		display.setCursor(240, 235);
		display.print(str2);

	} while (display.nextPage());
}

void quadScreen(int val1, int val2, int val3, int val4, String str1, String str2,String str3, String str4)
{

	display.setPartialWindow(0, 0, display.width(), display.height());
	display.setRotation(0);
	display.setFont(&Lato_Bold_40);
	display.setTextColor(GxEPD_BLACK);

	do
	{
		display.fillScreen(GxEPD_WHITE);
		display.setCursor(40, 100);
		display.print(val1);
		display.setCursor(220, 100);
		display.print(val2);
		display.setCursor(20, 250);
		display.print(val3);
		display.setCursor(200, 250);
		display.print(val4/100);
		display.setCursor(280, 250);
		display.print(":");
		display.setCursor(295, 250);
		display.print(val4%100);

		display.setFont(&FreeMonoBoldOblique12pt7b);

		display.setCursor(100, 140);
		display.print(str1);
		display.setCursor(240, 140);
		display.print(str2);
		display.setCursor(100, 290);
		display.print(str3);
		display.setCursor(300, 290);
		display.print(str4);

	} while (display.nextPage());




	
}

void fullScreen(int value,String str1)
{
	display.setPartialWindow(0, 0, display.width(), display.height());
	display.setFont(&FreeSansBold100pt7b);
	display.setTextColor(GxEPD_BLACK);

	do
	{
		display.fillScreen(GxEPD_WHITE);
		display.setCursor(20, -40);
		display.print(value);
		display.setFont(&FreeMonoBoldOblique24pt7b);
		display.setCursor(200, 290);
		display.print(str1);
	} while (display.nextPage());
	
}


int getSpeed()
{
	if ((WiFi.status() == WL_CONNECTED))
	{ //Check the current connection status

		HTTPClient http;

		http.begin("http://10.10.10.1:3000/signalk/v1/api/vessels/self/navigation/speedOverGround/value"); //Specify the URL
		int httpCode = http.GET();																		   //Make the request

		if (httpCode > 0)
		{ //Check for the returning code

			String payload = http.getString();
			int n = payload.length();

			// declaring character array
			char char_array[n + 1];

			// copying the contents of the
			// string to char array
			strcpy(char_array, payload.c_str());
			http.end(); //Free the resources
			return ((int)(atoi(char_array) * 1.943844));
		}

		else
		{
			return (-1);
		}

		http.end(); //Free the resources
	}
	return (-2);
}

int getDate()
{
	if ((WiFi.status() == WL_CONNECTED))
	{ //Check the current connection status

		HTTPClient http;

		http.begin("http://10.10.10.1:3000/signalk/v1/api/vessels/self/navigation/magneticVariationAgeOfService/value"); //Specify the URL
		int httpCode = http.GET();																		   //Make the request

		if (httpCode > 0)
		{ //Check for the returning code

			String payload = http.getString();
			int n = payload.length();

			// declaring character array
			char char_array[n + 1];

			// copying the contents of the
			// string to char array
			strcpy(char_array, payload.c_str());
			http.end(); //Free the resources
			int date = (int)(atoi(char_array));
			return (date);
		}

		else
		{
			return (-1);
		}

		http.end(); //Free the resources
	}
	return (-2);
}

int convertDate(){
	int date = getDate()+3600*2;
	date = date%86400;

	int hours = date/3600;
	date = date%3600;
	int minutes = date/60;
	return(100*hours+minutes);
}


int getMagneticCourse()
{
	if ((WiFi.status() == WL_CONNECTED))
	{ //Check the current connection status

		HTTPClient http;

		http.begin("http://10.10.10.1:3000/signalk/v1/api/vessels/self/navigation/courseOverGroundTrue/value"); //Specify the URL
		int httpCode = http.GET();																					//Make the request

		if (httpCode > 0)
		{ //Check for the returning code

			String payload = http.getString();
			int n = payload.length();

			// declaring character array
			char char_array[n + 1];

			// copying the contents of the
			// string to char array
			strcpy(char_array, payload.c_str());
			http.end(); //Free the resources
      float value = (atof(char_array)*57.2958);
			return ((int)value);
		}

		else
		{
			return (-1);
		}

		http.end(); //Free the resources
	}
	return (-2);
}


int getMagneticBoussoleCourse()
{
	if ((WiFi.status() == WL_CONNECTED))
	{ //Check the current connection status

		HTTPClient http;

		http.begin("http://10.10.10.1:3000/signalk/v1/api/vessels/self/navigation/headingMagnetic/value"); //Specify the URL
		int httpCode = http.GET();																					//Make the request

		if (httpCode > 0)
		{ //Check for the returning code

			String payload = http.getString();
			int n = payload.length();

			// declaring character array
			char char_array[n + 1];

			// copying the contents of the
			// string to char array
			strcpy(char_array, payload.c_str());
			http.end(); //Free the resources
      float value = (atof(char_array)*57.2958);
			return ((int)value);
		}

		else
		{
			return (-1);
		}

		http.end(); //Free the resources
	}
	return (-2);
}

void setup()
{

	//WiFi.setTxPower(WIFI_POWER_MINUS_1dBm);
	
	setCpuFrequencyMhz(80);

	display.init(115200);

	pinMode(buttonPin, INPUT);

	display.clearScreen();
	delay(400);


	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(100);
	}
}

void loop()
{
	delay(300);
	cmp++;
    buttonState = LOW;
	buttonState = digitalRead(buttonPin);
	if (buttonState == HIGH){
		mode = mode+1;
	}

	buttonState = LOW;
	if (mode >= 4)
		mode = 0;


	if (mode == 0){
		if (oldmode!=mode){
			display.clearScreen();
			delay(200);		
		}
		fullScreen(getSpeed(),"KNOTS");
		
	}


	if (mode == 1){
		if (oldmode!=mode){
			display.clearScreen();
			delay(200);
		}
		dualScreen(getSpeed(), getMagneticCourse(), "KNOTS", "COG");
		
	}

	if (mode == 2){
		if (oldmode!=mode){
			display.clearScreen();
			delay(200);
		}
		dualScreen(getSpeed(), getMagneticBoussoleCourse(), "KNOTS", "COMP");
		
	}

	if (mode == 3){
		if (oldmode!=mode){
			display.clearScreen();
			delay(200);
		}
		quadScreen(getSpeed(), getMagneticBoussoleCourse(), getMagneticCourse(), convertDate(), "KNOTS", "COMPASS", "COG", "TIME");
		
	}


	if (cmp>100) {
		cmp=0;
		display.clearScreen();
	}
	oldmode=mode;
}