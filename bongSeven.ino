
//https://api.tidesandcurrents.noaa.gov/api/prod/datagetter?product=predictions&application=Custom&date=today&datum=MLLW&station= 9455920 &time_zone=lst_ldt&units=english&interval=hilo&format=xml

#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
int failConnect = 1;
#define CONFIG_NOAA_STATION "9455920"
#define CONFIG_USER_AGENT "someone@example.com"
#define CONFIG_OFFSET_FROM_UTC (-9)
String city = "Anchorage";
String countryCode = "US";
String openWeatherMapApiKey = "e28ba1db8ba3c57983a446d6afbcb55b";
int lockPoint = 0;
const int API_TIMEOUT = 10000;
int temp = 50;
int callSnow = 0;
unsigned long minorTimer;
#include "FastLED.h"
#define LED_PIN     17
//#define CLK_PIN     D4
#define BRIGHTNESS  255
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
int timeIndex = 1;
#define NUM_LEDS 70
int timerLight = 0;
CRGB leds[NUM_LEDS];
unsigned long timerDelay = 60000;
unsigned long lastTime = 0;
unsigned long masterTimer;
int lightLevel = 20;
long hour;
long minute;
int hourCurrent;
int minuteCurrent;
bool fLow;
unsigned long weatherTimer;
const int CCH = 128;
String jsonBuffer;
char rgch[CCH];
bool ConnectToWiFi(void)
{
  const char *ssid = "werner";
  const char *password = "9073456071";
  int count = 0;

  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.print(".");
    if (++count > 60)
      return false;
  }
  Serial.println("\nWiFi connected");
  return true;
}
WiFiClientSecure client;



void ConnectToNoaa(
    WiFiClientSecure &client,
    int &hour,
    int &minute)
{
  const char *host = "api.tidesandcurrents.noaa.gov";
  const int httpsPort = 443;
  static const char noaa_rootCA_cert[] PROGMEM =
      "-----BEGIN CERTIFICATE-----\n"
      "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n"
      "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
      "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
      "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n"
      "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
      "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n"
      "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n"
      "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"
      "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n"
      "43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n"
      "T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n"
      "gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n"
      "BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n"
      "TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n"
      "DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n"
      "hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n"
      "06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n"
      "PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n"
      "YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n"
      "CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n"
      "-----END CERTIFICATE-----\n";

  client.setCACert(noaa_rootCA_cert);
  delay(100);
  if (!client.connect(host, httpsPort))
  {
    Serial.println("connection failed");
    failConnect = 1;
    return;
  }
  failConnect = 5;
  
  
  const char url[] = "/api/prod/datagetter?product=predictions&application=Custom&date=recent&datum=MLLW&station=" CONFIG_NOAA_STATION "&time_zone=lst_ldt&units=english&interval=hilo&format=csv";

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: " CONFIG_USER_AGENT "\r\n" +
               "Connection: close\r\n\r\n");
  /* 
    Returned text looks like:
    Date Time, Prediction, Type
    2018-12-25 00:54,-3.304,L
    2018-12-25 08:06,16.223,H
    2018-12-25 13:54,7.533,L
    2018-12-25 18:35,13.596,H  
  */

  while (client.connected())
  {
    const int CCH = 128;
    char rgch[CCH];

    client.readBytesUntil('\n', rgch, CCH);
    if (rgch[0] == '\r')
    {
      // A blank line indicates the end of the HTTP headers
      break;
    }
    // Get current time from HTTP header
    if (0 == strncmp(rgch, "Date:", 5))
    {
      // Skip 5 spaces to get to the time
      int cSpaces = 0;
      char *pch = rgch + 10;
      while (*pch && cSpaces < 4)
      {
        if (*pch++ == ' ')
        {
          cSpaces++;
        }
      }
      hour = atoi(pch) + CONFIG_OFFSET_FROM_UTC;
      hour = (hour + 24) % 24;

      // Now find the minute
      while (*pch++ != ':')
        ;
      minute = atoi(pch);
    }
  }
   client.readStringUntil('\n');
}

extern const TProgmemRGBGradientPalettePtr gGradientPalettes[];
   CRGBPalette16 greenPalette(gGradientPalettes[5]);
   CRGBPalette16 targetPalette( gGradientPalettes[0]);

void setup(){
   
   Serial.begin(115200);
   LEDS.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds,NUM_LEDS);                // WS2812
   
   client.setTimeout(API_TIMEOUT);
   if (!ConnectToWiFi())
    {
      Serial.print("notconnected");
    }
 masterTimer = millis();
 minorTimer = millis();
 //weatherTimer = millis();
}
void loop()
{
  /*
  if((millis() - weatherTimer) > 1000 * 60 * 30){
    tempTime(); 
    weatherTimer = millis();
    
  }
  */
   EVERY_N_MINUTES(2){
    fallTemp();
    if (callSnow >599 || callSnow < 621) snowstorm();
    minorTimer = millis();
  }
    fillnoise8(); 
    fill_time();
    if(fLow)leds[59] = CRGB::Red;
    else leds[0] = CRGB::Red;
    LEDS.show(); 
    
    EVERY_N_MINUTES(5){
      
    
      Serial.println("in here");
     
    
    if(ConnectToWiFi()){
    ConnectToNoaa(client, /*out*/ hourCurrent, /*out*/ minuteCurrent);
    if(client.connected()){
      failConnect = 5;
    while (client.connected())
    {
      
      long year = client.parseInt();
      client.read(); // '-'
      long month = client.parseInt();
      client.read(); // '-'
      long day = client.parseInt();
      hour = client.parseInt();
      minute = client.parseInt();
      long level = client.parseInt();
      client.readBytesUntil(',', rgch,CCH);
      fLow = ('L' == client.read());
      client.readBytesUntil('\n', rgch, CCH);
      /*
      Serial.print("year");
      Serial.println(year);
      Serial.print("month");
      Serial.println(month);
     
      Serial.print("day");
      Serial.println(day);
      Serial.print("level");
      Serial.println(level);
      Serial.print("hour");
      Serial.println(hour);
      Serial.print("current hour:  ");
      Serial.println(hourCurrent);
      Serial.print("minute:");
      Serial.println(minuteCurrent);
      */
     

    }
    
  client.stop();  
  Serial.print("*bool");
  Serial.println(fLow);
  //Serial.print("*tide hour");
  Serial.print("minute");
  Serial.println(minuteCurrent);
  Serial.println(hour);
  Serial.print("*currentHour");
  Serial.print(hourCurrent);
  Serial.print("*TideMinute");
  Serial.println(minute); 
  
  timerLight = hourCurrent;
  if(hourCurrent < hour) hourCurrent = hourCurrent + 24;
  hour = (hour * 60) + minute;
  hourCurrent = (hourCurrent * 60) + minuteCurrent;
  int timeToGo = hourCurrent - hour;
  if(fLow) lightLevel = map(timeToGo, 0,360, 1,60);
  else lightLevel = map(timeToGo, 0,360, 60,1);
  lightLevel = constrain(lightLevel,  1, 60);
  Serial.print("lightLevel");
  Serial.println(lightLevel);
  if((timerLight < 8) || (timerLight > 20)) timeIndex = 2;
  else if(timerLight < 11)  timeIndex = 1;
  else if(timerLight > 14) timeIndex = 4;
  else timeIndex = 3;
  Serial.print("timeIndex");
  Serial.println(timeIndex); 
  tempTime(); 
  fallTemp();
  //
      }
    }
  } 
}
void fillnoise8() {
 
  #define scale 30                                                          // Don't change this programmatically or everything shakes.
  //lightLevel = 30;
  for(int i = 0; i < lightLevel; i++) {                                       // Just ONE loop to fill up the LED array as all of the pixels change.
    uint8_t index = inoise8(i*scale, millis()/10+i*scale);                   // Get a value from the noise function. I'm using both x and y axis.
    leds[i] = ColorFromPalette(targetPalette, index, 255, LINEARBLEND);
 
    // With that value, look up the 8 bit colour palette value and assign it to the current LED.
  }
 
} // fillnoise8()
void fill_time(){
  #define scale 30
 // Don't change this programmatically or everything shakes.
   CRGBPalette16 otherPalette(gGradientPalettes[timeIndex]);
  for(int i = lightLevel - 1; i < 60; i++) {                                       // Just ONE loop to fill up the LED array as all of the pixels change.
    uint8_t index = inoise8(i*scale, millis()/10+i*scale);                   // Get a value from the noise function. I'm using both x and y axis.
    leds[i] = ColorFromPalette(otherPalette, index, 255, LINEARBLEND);    // With that value, look up the 8 bit colour palette value and assign it to the current LED.
  }
 
}
void tempTime(){
  if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
    
      Serial.print("JSON object = ");
      Serial.println(myObject);
      Serial.print("Temperature: ");
      //Serial.println(myObject["main"]["temp"]);
      temp =int( myObject["main"]["temp"]);
      temp = temp -  273;
      temp = int ((float( temp ) * 9/5) + 32);
      Serial.println(temp);
      Serial.print("Pressure: ");
      Serial.println(myObject["main"]["pressure"]);
      Serial.print("Humidity: ");
      Serial.println(myObject["main"]["humidity"]);
      Serial.print("Wind Speed: ");
      Serial.println(myObject["wind"]["speed"]);
      Serial.print("weather ID");
      callSnow = int(myObject["weather"][0]["id"]);
      Serial.println(myObject["weather"][0]["id"]);
      callSnow = 600;
    }
    else {
      Serial.println("WiFi Disconnected");
    }
   
}


String httpGETRequest(const char* serverName) {
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
void fallTemp(){
  
  int george = 0;
  int pMax = 0;
  int i = 0;
  float tCycle;
  float h;
  int pos = 0;
  long tLast = millis();
  int first = temp/10;
  int next = temp - (temp/10) * 10;
  Serial.print("first=");
  Serial.println(first);
  Serial.print("next:");
  Serial.println(next);
  FastLED.clear();
  
  for(int total = 0; total < first; total ++){
   george = 0;
   pMax = (total +1) * 5;
  while(!george){
 
   tCycle=millis()-tLast;
   h=0.5*2.0*pow(tCycle/1000,2.0);
 
  pos=(60-(round(h*60)));
  if(pos +5 > 60)pos = pos - 4;
  if (pos <=  pMax) {
    
    tLast=millis();
    george=1;
  }
    for(int j=0;j<5;j++){
    leds[pos + j] = CRGB::Red;
    
    }
    FastLED.show();
    delay(30);
    for(int j=0;j<5;j++){
    int posColor = map(pos, 0,60, 255,100);
    leds[pos+j] = ColorFromPalette(greenPalette, posColor, 255, LINEARBLEND);  
    }
    FastLED.show();
  }
   for(int j = 0 ;j < pMax;j++)leds[j] = CRGB::Red;
   for(int j = 0 ; j <= pMax; j = j +5)leds[j] = CRGB::Green;
    FastLED.show();
   for(int j = pMax ;j < 60;j++)leds[j] =  CRGB::Black;
  }
  
 delay(2000);
   tLast = millis();
   FastLED.clear();
  for(int total = 0; total < next; total ++){
  george = 0;
  pMax = (total +1) * 5;
  while(!george){
  
  tCycle=millis()-tLast;
  h=0.5*2.0*pow(tCycle/1000,2.0);
  pos=(60-(round(h*60)));
  if(pos + 4 > 60)pos = pos - 4;
  if (pos <=  pMax) {    
    tLast=millis();
    george=1;
  }
    for(int j=0;j<5;j++){
    leds[pos + j] = CRGB::Blue;
    }
    FastLED.show();
    delay(30);
    for(int j=0;j<5;j++){
    int posColor = map(pos, 0,60, 255,100);
    leds[pos+j] = ColorFromPalette(greenPalette, posColor, 255, LINEARBLEND); 
    }
     FastLED.show();
  }
   for(int j = 0 ;j < pMax;j++)leds[j] = CRGB::Blue;
   for(int j = 0 ; j <= pMax; j = j +5)leds[j] = CRGB::Green;
    
    FastLED.show();
    for(int j = pMax ;j < 60;j++)leds[j] =  CRGB::Black;
  }
  
 delay(2000);
  
  } 
  void snowstorm(){
    #define  NUM_BALLS  6
    int pos[NUM_BALLS];
    int george = 0;
    float tCycle[NUM_BALLS];
    float h[NUM_BALLS];
    int COR[NUM_BALLS];
    float GRAVITY[NUM_BALLS] = {2, 0.4, 0.6, 0.3, 1.2, 0.1};
    int pMax = 0;
   unsigned long snowLast = millis();
    long tLast[NUM_BALLS];
    for(int i=0; i<NUM_BALLS;i++){    //this is the setup for the falling rain/snow animation
    tLast[i]=millis();
    h[i]=0;
    COR[i]=random(2,6);
    }
  while(pMax <20){
  for(int i=0;i<NUM_BALLS;i++){
  tCycle[i]=millis()-tLast[i];
  h[i]=0.5*GRAVITY[i]*pow(tCycle[i]/1000,2.0);
  //Serial.println(h[i]);
  pos[i]=(60-(round(h[i]*60)))-COR[i];
  //Serial.println(pos[i]);
  if (pos[i]<pMax) {
    tLast[i]=millis();
    george=george+1;
    pMax=round(george/10);
  }
  }
  
 
  //for(int i=0;i<60;i++)leds[i] = CRGB::DarkBlue;
  for(int i=0;i<60;i++){
  int posColor = map(i, 0,60, 255,100);
  leds[i] = ColorFromPalette(targetPalette, posColor, 255, LINEARBLEND);
  }
  for(int i=0;i<pMax;i++)leds[i] = CRGB::White;
  for(int i=0;i<NUM_BALLS;i++){
    //strip.setPixelColor(pos[i],255,255,255,255);
    leds[pos[i]] = CRGB::White;
    //strip.setPixelColor(pos[i]+1,0,0,200);
    leds[pos[i] + 1] = CRGB::DarkBlue;
    
  }
  FastLED.show();
  for (int i = 0 ; i < NUM_BALLS ; i++) {
    //strip.setPixelColor(pos[i],0,0,0);
    leds[pos[i]] = CRGB::Black;
    //strip.setPixelColor(pos[i]+1,0,0,0);
    leds[pos[i] + 1] = CRGB::Black;
  }
 if(pMax>30)george=0;
    }
 
} 
// Gradient palette "revisiting_dreams_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/angelafaye/tn/revisiting_dreams.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 40 bytes of program space.







DEFINE_GRADIENT_PALETTE( revisiting_dreams_gp ) {
    0,   3,  3,  4,
   51,   3,  3,  4,
   51,   6,  8,  8,
  102,   6,  8,  8,
  102,   8, 22, 16,
  153,   8, 22, 16,
  153,  21, 55, 25,
  204,  21, 55, 25,
  204,  43, 91, 44,
  255,  43, 91, 44};

DEFINE_GRADIENT_PALETTE( bhw1_sunset3_gp ) {
   0, 227,237, 56,
  33, 186, 67,  1,
  71, 163, 21,  1,
  81, 157, 13,  1,
 188,  39, 21, 18,
 234,  12,  7,  4,
 255,  12,  7,  4};
DEFINE_GRADIENT_PALETTE( bhw1_sunset1_gp ) {
   0,  33, 21, 25,
  38, 125, 29, 20,
  71, 222, 59, 30,
 145, 190,147,127,
 178,  88,136,203,
 255,   3, 24, 78};

DEFINE_GRADIENT_PALETTE( bhw1_sunset2_gp ) {
   0, 255,175,  8,
  81, 237, 29, 10,
 137, 148, 57, 42,
 165,  68, 54, 54,
 255,  18, 23, 29};

DEFINE_GRADIENT_PALETTE( xkcd_bath_gp ) {
   0,   1,  1,  1,
   2,   1,  1,  1,
   4,   1,  1,  1,
   6,   1,  1,  1,
   8,   1,  1,  1,
  10,   1,  1,  1,
  12,   1,  1,  1,
  14,   1,  1,  1,
  16,   1,  1,  1,
  18,   1,  1,  1,
  20,   1,  1,  1,
  22,   1,  1,  1,
  24,   1,  1,  1,
  26,   1,  1,  1,
  28,   1,  1,  1,
  30,   1,  1,  1,
  32,   1,  1,  1,
  34,   1,  1,  1,
  36,   1,  1,  1,
  38,   1,  1,  1,
  40,   1,  1,  1,
  42,   1,  1,  1,
  44,   1,  1,  1,
  46,   1,  1,  1,
  48,   1,  1,  1,
  51,   1,  1,  1,
  53,   1,  1,  1,
  55,   1,  1,  1,
  57,   1,  1,  1,
  59,   1,  1,  1,
  61,   1,  1,  1,
  63,   1,  1,  2,
  65,   1,  1,  2,
  67,   1,  1,  2,
  69,   1,  1,  2,
  71,   1,  1,  2,
  73,   2, 23, 67,
 157,   2, 24, 72,
 159,   2, 26, 77,
 161,   2, 27, 80,
 163,   2, 29, 84,
 165,   2, 31, 88,
 167,   3, 32, 93,
 169,   3, 33, 98,
 171,   3, 36,102,
 173,   4, 38,106,
 175,   4, 41,112,
 177,   4, 44,117,
 179,   5, 47,123,
 181,   5, 50,128,
 183,   5, 53,133,
 185,   6, 56,138,
 187,   7, 60,144,
 189,   7, 63,149,
 191,   7, 68,155,
 193,   8, 72,160,
 195,   9, 78,164,
 197,  10, 82,170,
 199,  10, 86,174,
 201,  11, 90,178,
 204,  12, 96,182,
 206,  12,100,186,
 208,  13,104,190,
 210,  14,109,194,
 212,  15,117,199,
 214,  16,122,203,
 216,  17,128,205,
 218,  18,133,210,
 220,  19,138,212,
 222,  20,144,216,
 224,  21,149,221,
 226,  21,154,223,
 228,  22,159,226,
 230,  24,166,228,
 232,  25,171,233,
 234,  26,178,235,
 236,  27,182,237,
 238,  27,186,240,
 240,  28,189,242,
 242,  30,193,247,
 244,  30,199,250,
 246,  31,203,252,
 248,  31,205,252,
 250,  32,209,255,
 252,  32,209,255,
 255,  33,211,255};
const TProgmemRGBGradientPalettePtr gGradientPalettes[] = {
  xkcd_bath_gp,
  bhw1_sunset2_gp,
 revisiting_dreams_gp,
  bhw1_sunset1_gp,
  bhw1_sunset3_gp,
  revisiting_dreams_gp};
