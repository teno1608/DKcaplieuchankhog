
/*
    Name:       DKcaplieuchankhogv2_3nut.ino
    Created:	26/01/2021 15:21:54
    Author:     FAIRYZBOOK\Fairy
*/

/*void setup()
{
	pinMode(Relay1, OUTPUT);
	pinMode(Relay2, OUTPUT);
	pinMode(Relay3, OUTPUT);
	pinMode(Relay4, OUTPUT);
	digitalWrite(Relay1, 0);
	digitalWrite(Relay2, 0);
	digitalWrite(Relay3, 0);
	digitalWrite(Relay4, 0);
}
*/

/*void loop()
{
	digitalWrite(Relay1, 1);
	delay(2000);
	digitalWrite(Relay2, 1);
	delay(2000);
	digitalWrite(Relay3, 1);
	delay(2000);
	digitalWrite(Relay4, 1);
	delay(2000);
	digitalWrite(Relay1, 0);
	delay(2000);
	digitalWrite(Relay2, 0);
	delay(2000);
	digitalWrite(Relay3, 0);
	delay(2000);
	digitalWrite(Relay4, 0);
	delay(2000);

}
*/
/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
	 OR
	 Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port

  Created for arduino-esp32 on 04 July, 2018
  by Elochukwu Ifediora (fedy0)
*/

#include <WiFi.h>
#include <EEPROM.h>

#define Relay1 4
#define Relay2 2
#define Relay3 12
#define Relay4 13
//#define Relay1 2
//#define Relay2 2
//#define Relay3 13
//#define Relay4 13


// Client variables
char linebuf[80];
int charcount = 0;

int time_on = 0, time_off = 0, time_on_boost = 0, time_off_boost = 0;

int time_on_run = 0, time_off_run = 0, time_all = 0;

int setmills = 0, first_start = 0;

bool lap_run = 0, Boost_mode = 0;

// HTML web page to handle 3 input fields (input1, input2, input3)

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>
  <form action="/get">
    Time_On: <input type="text" name="input1">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    Time_Off: <input type="text" name="input2">
    <input type="submit" value="Submit">
  </form><br>
</body>
</html>
)rawliteral";

// Set these to your desired credentials.
const char *ssid = "AcePoi19216841";
const char *password = "0987654321";

WiFiServer server(80);

void setup()
{
	//Set pin mode out
	pinMode(Relay1, OUTPUT);
	pinMode(Relay2, OUTPUT);
	pinMode(Relay3, OUTPUT);
	pinMode(Relay4, OUTPUT);

	digitalWrite(Relay1, 0);
	digitalWrite(Relay2, 0);
	digitalWrite(Relay3, 0);
	digitalWrite(Relay4, 0);

	EEPROM.begin(128);

	Serial.begin(115200);
	Serial.println();
	Serial.println("Configuring access point...");

	// You can remove the password parameter if you want the AP to be open.
	WiFi.softAP(ssid, password);
	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
	server.begin();

	Serial.println("Server started");
}


void loop()
{
	WiFiClient client = server.available(); // listen for incoming clients

	if (client)
	{									 // if you get a client,
		Serial.println("New_request.."); // print a message out the serial port
		memset(linebuf, 0, sizeof(linebuf));
		charcount = 0;
		String currentLine = ""; // make a String to hold incoming data from the client
		boolean currentLineIsBlank = true;

		while (client.connected())
		{ // loop while the client's connected
			if (client.available())
			{
				char c = client.read();
				Serial.write(c);
				//read char by char HTTP request
				linebuf[charcount] = c;
				if (charcount < sizeof(linebuf) - 1)
					charcount++;

				// if you've gotten to the end of the line (received a newline
				// character) and the line is blank, the http request has ended,
				// so you can send a reply

				if (c == '\n' && currentLineIsBlank)
				{
					client.println();
					client.println("<!DOCTYPE HTML><html><head>");
					client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>");
					client.println("<h1>ESP32-WebServer</h1>");
					client.println("<p>Motor: <a href=\"on1\"><button>ON</button></a>&nbsp;<a href=\"off1\"><button>OFF</button></a></p>");
					client.println("</html>");
					client.println(index_html);
					break;
				}

				if (c == '\n')
				{
					// you're starting a new line
					currentLineIsBlank = true;
					if (strstr(linebuf, "GET /on1") > 0)
					{
						Serial.println("Motor ON");
						time_off = 0;
						time_on = 1;
					}
					else if (strstr(linebuf, "GET /off1") > 0)
					{
						Serial.println("Motor OFF");
						time_off = 1;
						time_on = 0;
					}
					else if (strstr(linebuf, "GET /get?input1") > 0)
					{

						for (int i = 1; i < 100; i++)
						{
							char chss[10];
							char chss2[50] = "GET /get?input1=";
							itoa(i, chss, 10);
							strcat(chss2, chss);
							if (strstr(linebuf, chss2) > 0)
							{
								time_on = i;
							}
						}

						EEPROM.write(8, time_on);
						EEPROM.commit();
					}
					else if (strstr(linebuf, "GET /get?input2") > 0)
					{

						for (int i = 1; i < 100; i++)
						{
							char chss[10];
							char chss2[50] = "GET /get?input2=";
							itoa(i, chss, 10);
							strcat(chss2, chss);
							if (strstr(linebuf, chss2) > 0)
							{
								time_off = i;
							}
						}

						EEPROM.write(16, time_off);
						EEPROM.commit();
					}
					
					// you're starting a new line
					currentLineIsBlank = true;
					memset(linebuf, 0, sizeof(linebuf));
					charcount = 0;
				}
				else if (c != '\r')
				{
					// you've gotten a character on the current line
					currentLineIsBlank = false;
				}
			}
		}
		delay(1);
		// close the connection:
		client.stop();
		Serial.println("Client Disconnected.");
	}

	if (first_start == 0)
	{
		setmills = millis();
		time_on = EEPROM.read(8);
		time_off = EEPROM.read(16);
		first_start = 1;
	}
	if (first_start == 1 && millis() - setmills > 1000)
	{
		first_start = 2;
		Serial.println("Begin_Running.....");
	}

	if (first_start == 2)
	{

		if (time_on == 1 && time_off == 0)
		{
			time_all = 1;
			digitalWrite(Relay1, 1);
			digitalWrite(Relay3, 1);
		}

		if (time_on == 0 && time_off == 1)
		{
			time_all = 1;
			digitalWrite(Relay1, 0);
			digitalWrite(Relay3, 0);
		}

		if (lap_run == 0 && time_on != 0 && time_off != 0)
		{
			time_on_run = time_on * 1000;
			time_off_run = time_off * 1000;
			time_all = time_on_run + time_off_run;
			setmills = millis();
			lap_run = 1;
		}

		if (lap_run == 0 && time_on_boost != 0 && time_off_boost != 0)
		{
			time_on_run = time_on_boost * 1000;
			time_off_run = time_off_boost * 1000;
			time_all = time_on_run + time_off_run;
			setmills = millis();
			lap_run = 1;
		}

		if (millis() - setmills < time_all)
		{
			if (millis() - setmills < time_on_run)
			{
				digitalWrite(Relay1, 1);
				digitalWrite(Relay3, 1);
			}
			else
			{
				digitalWrite(Relay1, 0);
				digitalWrite(Relay3, 0);
			}
		}
		else
		{
			lap_run = 0;
		}
	}
}
