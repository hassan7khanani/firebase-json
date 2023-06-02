#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <DHT.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#define WIFI_SSID "Virus"
#define WIFI_PASSWORD "12345678"
#define FIREBASE_HOST "espledonoff-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "ZnIORSB3cOXMVmPFZiQdBkSMsNgwhNhsvaKhM0QU"



const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
//WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
WiFiUDP udp;  
NTPClient timeClient(udp, "time.nist.gov", 14400, 60000);  // NTPClient object with GMT+4 timezone and update interval of 1 minute


#define DHTPIN1 D2
#define DHTPIN2 D4
#define DHTTYPE DHT11
DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

FirebaseData firebaseData;
FirebaseJson json;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  dht1.begin();
  dht2.begin();
  timeClient.begin();

 // Sync every 5 minutes
}

void loop() {
  timeClient.update();


  time_t epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
  
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Formatted Time: ");
  Serial.println(formattedTime);  

  int currentHour = timeClient.getHours();
  Serial.print("Hour: ");
  Serial.println(currentHour);  

  int currentMinute = timeClient.getMinutes();
  Serial.print("Minutes: ");
  Serial.println(currentMinute); 
   
  int currentSecond = timeClient.getSeconds();
  Serial.print("Seconds: ");
  Serial.println(currentSecond);  

  String weekDay = weekDays[timeClient.getDay()];
  Serial.print("Week Day: ");
  Serial.println(weekDay);    

  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  int monthDay = ptm->tm_mday;
  Serial.print("Month day: ");
  Serial.println(monthDay);

  int currentMonth = ptm->tm_mon+1;
  Serial.print("Month: ");
  Serial.println(currentMonth);

  String currentMonthName = months[currentMonth-1];
  Serial.print("Month name: ");
  Serial.println(currentMonthName);

  int currentYear = ptm->tm_year+1900;
  Serial.print("Year: ");
  Serial.println(currentYear);

  //Print complete date:
  String time_stamp = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay) + "  " +String(formattedTime) ;
  Serial.print("Current date is : ");
  Serial.println(time_stamp);
  Serial.println("");
//  float temperature1 = dht1.readTemperature();
//  float temperature2 = dht2.readTemperature();
   float temperature1=30;
   float temperature2=40;
  if (isnan(temperature1)) {
    Serial.println("Failed to read data from DHT sensor #01");
    return;
  }
  if (isnan(temperature2)) {
    Serial.println("Failed to read data from DHT sensor #02");
    return;
  }
  json.set("Ambient_Temperature",temperature1);
  json.set("Exhaust_Temperature",temperature2);
  json.set("timestamp",time_stamp);
  Serial.printf("Set json... %s\n", Firebase.push(firebaseData, F("/dht_readings"), json) ? "ok" : firebaseData.errorReason().c_str());


  //Wait for 5 seconds
  delay(5000);
}
