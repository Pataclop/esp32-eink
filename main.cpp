//ESP32 WIRING
// BUSY -> 4, RST -> 16, DC -> 17, CS -> SS(5), CLK -> SCK(18), DIN -> MOSI(23), GND -> GND, 3.3V -> 3.3V

#define ENABLE_GxEPD2_GFX 0
#include <Wire.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBoldOblique24pt7b.h>
#include <Fonts/FreeSansBold50pt7b.h>
#include <Fonts/FreeSansBold100pt7b.h>

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
const char seven[] = "0123456";
const char tsleep[] = " ";
const int buttonPin = 15; // the number of the pushbutton pin
int buttonState = 0;	  // variable for reading the pushbutton status

int cmp = 0;

int mode = 0;
int oldmode=1;

void dualScreen(int val1, int val2)
{

	display.setPartialWindow(0, 0, display.width(), 3*display.height()/4);
	display.setRotation(0);
	display.setFont(&FreeSansBold50pt7b);
	display.setTextColor(GxEPD_BLACK);

	// do this outside of the loop
	int16_t tbx, tby;
	uint16_t tbw, tbh;
	// center update text

	display.getTextBounds(three, 0, 0, &tbx, &tby, &tbw, &tbh);
	uint16_t umx = ((display.width() * 1 / 4) - tbw / 2) - tbx;
	uint16_t umy = ((display.height() - tbh) / 2) - tby;

	// center clean
	display.getTextBounds(three, 0, 0, &tbx, &tby, &tbw, &tbh);
	uint16_t hwx = ((display.width() * 3 / 4) - tbw / 2) - tbx;
	uint16_t hwy = ((display.height() - tbh) / 2) - tby;
	display.firstPage();
	do
	{
		display.fillScreen(GxEPD_WHITE);
		display.setCursor(hwx, hwy);
		display.print(val2);
		display.setCursor(umx, umy);
		display.print(val1);

	} while (display.nextPage());
}

void dualText(String str1, String str2)
{

	display.setPartialWindow(0, 3*display.height()/4, display.width(), display.height());
	display.setRotation(0);
	display.setFont(&FreeMonoBoldOblique24pt7b);
	display.setTextColor(GxEPD_BLACK);

	// do this outside of the loop
	int16_t tbx, tby;
	uint16_t tbw, tbh;
	// center update text

	display.getTextBounds(three, 0, 0, &tbx, &tby, &tbw, &tbh);
	uint16_t umx = ((display.width() * 1 / 4) - tbw / 2) - tbx;
	uint16_t umy = ((display.height()) - 1.5*tbh) - tby;

	// center clean
	display.getTextBounds(three, 0, 0, &tbx, &tby, &tbw, &tbh);
	uint16_t hwx = ((display.width() * 3 / 4) - tbw / 2) - tbx;
	uint16_t hwy = ((display.height()) - 1.5*tbh) - tby;
	display.firstPage();
	do
	{
		display.fillScreen(GxEPD_WHITE);
		display.setCursor(hwx, hwy);
		display.print(str2);
		display.setCursor(umx, umy);
		display.print(str1);

	} while (display.nextPage());
}

void fullScreen(int value)
{
	display.setPartialWindow(0, 0, display.width(), 3*display.height()/4);
	display.setRotation(0);
	display.setFont(&FreeSansBold100pt7b);
	display.setTextColor(GxEPD_BLACK);

	int16_t tbx, tby;
	uint16_t tbw, tbh;

	// center update mode
	display.getTextBounds(three, 0, 0, &tbx, &tby, &tbw, &tbh);
	uint16_t umx = ((display.width() - tbw) / 2) - tbx;
	uint16_t umy = ((display.height() * 2 / 4) - tbh / 2) - tby;
	// center clean
	display.firstPage();
	display.fillScreen(GxEPD_WHITE);
	do
	{
		display.fillScreen(GxEPD_WHITE);
		display.setCursor(umx, umy);
		display.print(value);
	} while (display.nextPage());
	
}

void FullText(String s){
	display.setPartialWindow(0, 3*display.height()/4, display.width(), display.height());
	display.setRotation(0);
	display.setFont(&FreeMonoBoldOblique24pt7b);
	display.setTextColor(GxEPD_BLACK);

	int16_t tbx, tby;
	uint16_t tbw, tbh;

	// center update mode
	display.getTextBounds(three, 0, 0, &tbx, &tby, &tbw, &tbh);
	uint16_t umx = (2*(display.width() - tbw) / 3) - tbx;
	uint16_t umy = ((display.height()) - 1.5*tbh) - tby;
	// center clean
	display.firstPage();
	display.fillScreen(GxEPD_WHITE);
	do
	{
		display.fillScreen(GxEPD_WHITE);
		display.setCursor(umx, umy);
		display.print(s);
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

void setup()
{

	//WiFi.setTxPower(WIFI_POWER_MINUS_1dBm);
	
	setCpuFrequencyMhz(80);

	display.init(115200);

	pinMode(buttonPin, INPUT);

	display.clearScreen();
	delay(1000);
	display.clearScreen();
	delay(1000);

	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(100);
	}
}

void loop()
{
	delay(2000);
	cmp++;
	//esp_sleep_enable_timer_wakeup(2000000); //5 seconds
    buttonState = LOW;
	buttonState = digitalRead(buttonPin);
	if (buttonState == HIGH){
		mode = mode+1;
	}

	buttonState = LOW;
	//int ret = esp_light_sleep_start();
	if (mode >= 2)
		mode = 0;
	if (mode == 0){
		if (oldmode!=mode){
			display.clearScreen();
			delay(200);
			FullText("NTS");
		}
		fullScreen(getSpeed());
		
	}


	if (mode == 1){
		if (oldmode!=mode){
			display.clearScreen();
			delay(200);
			dualText("NTS", "COG");
		}
		dualScreen(getSpeed(), getMagneticCourse());
		
	}


	if (cmp>30) {
		cmp=0;
		display.clearScreen();
		delay(1000);
	}
	oldmode=mode;
}
