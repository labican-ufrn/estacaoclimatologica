#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <ThingSpeak.h>

// definindo os pinos da esp

#define DHTPIN 4      // definindo pino do sensor
#define DHTTYPE DHT22 // definindo qual modelo do sensor

WiFiClient client;
DHT dht(DHTPIN, DHTTYPE); // Sensor that reads the temperature and humidity
unsigned long myChannelNumber = 1;
const char *myWriteAPIKey = "CHANNELKEY";

// conect wifi
const char *ssid = "WIFINAME";    // Enter your WIFI ssid
const char *password = "WIFEKEY"; // Enter your WIFI password

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 360000; // tempo de delay 6 min

// Variable to hold temperature readings
float temperatureC;
float humidity;
float sensorLdr;

void setup()
{
  dht.begin();
  Serial.begin(115200); // Initialize serial
  WiFi.mode(WIFI_STA);

  ThingSpeak.begin(client); // Initialize ThingSpeak
}

void loop()
{
  if ((millis() - lastTime) > timerDelay)
  {

    // Connect or reconnect to WiFi
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.print("Attempting to connect");
      while (WiFi.status() != WL_CONNECTED)
      {
        WiFi.begin(ssid, password);
        delay(5000);
      }
      Serial.println("\nConnected.");
    }

    // Get a new temperature reading
    temperatureC = dht.readTemperature();
    Serial.print("Temperature (ºC): ");
    Serial.println(temperatureC);
    humidity = dht.readHumidity();
    Serial.print("Humidity (%): ");
    Serial.println(humidity);

    sensorLdr = analogRead(A0);
    Serial.print("Luminous (%): ");
    Serial.println(sensorLdr);

    // uncomment if you want to get temperature in Fahrenheit
    /*temperatureF = 1.8 * bme.readTemperature() + 32;
    Serial.print("Temperature (ºC): ");
    Serial.println(temperatureF);*/

    // set the fields with the values
    ThingSpeak.setField(1, temperatureC);
    ThingSpeak.setField(2, humidity);
    ThingSpeak.setField(3, sensorLdr);

    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    // uncomment if you want to get temperature in Fahrenheit
    // int x = ThingSpeak.writeField(myChannelNumber, 1, temperatureF, myWriteAPIKey);

    if (x == 200)
    {
      Serial.println("Channel update successful.");
    }
    else
    {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
  }
}
