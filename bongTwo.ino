
//https://api.tidesandcurrents.noaa.gov/api/prod/datagetter?product=predictions&application=Custom&date=today&datum=MLLW&station= 9455920 &time_zone=lst_ldt&units=english&interval=hilo&format=xml

#include <WiFiClientSecure.h>
#define CONFIG_NOAA_STATION "9455920"
#define CONFIG_USER_AGENT "someone@example.com"
#define CONFIG_OFFSET_FROM_UTC (-9)
int lockPoint = 0;
#include "FastLED.h"
#define LED_PIN     17
//#define CLK_PIN     D4
#define BRIGHTNESS  255
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
 
#define NUM_LEDS 60
 
CRGB leds[NUM_LEDS];

float masterTimer;
int lightLevel = 20;
long hour;
long minute;
 int hourCurrent;
  int minuteCurrent;
  bool fLow;
const int CCH = 128;
char rgch[CCH];
bool ConnectToWiFi(void)
{
  const char *ssid = "werner";
  const char *password = "";
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

  if (!client.connect(host, httpsPort))
  {
    Serial.println("connection failed");
    return;
  }

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

  // Skip "Date Time, Prediction, Type" column headers
  client.readStringUntil('\n');
}
// Gradient palette "bhw1_sunset2_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_sunset2.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.

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
  CRGBPalette16 currentPalette(CRGB::Black);
   CRGBPalette16 targetPalette(xkcd_bath_gp);
   CRGBPalette16 otherPalette(bhw1_sunset2_gp);
void setup(){
   
   Serial.begin(115200);
   LEDS.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds,NUM_LEDS);                // WS2812
   
   if (!ConnectToWiFi())
    {
      Serial.print("notconnected");
    }
 masterTimer = millis();
}
void loop()
{
  //fill_grad();
   fill_time();
  fillnoise8(); 
   EVERY_N_MILLIS(10) {
    nblendPaletteTowardPalette(currentPalette, targetPalette, 48);          // Blend towards the target palette over 48 iterations.
  } 
  EVERY_N_SECONDS(5) {                                                      // Change the target palette to a random one every 5 seconds.
    uint8_t baseC=random8();
    targetPalette = CRGBPalette16(xkcd_bath_gp);
  }
   LEDS.show(); 
  if((millis() - masterTimer) > (1000 * 60 * 5)){
    masterTimer = millis();
    lockPoint ++;
    if(lockPoint == 255)lockPoint = 0;
    ConnectToNoaa(client, /*out*/ hourCurrent, /*out*/ minuteCurrent);
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
      
/*
      if (fLow)
      {
        // Draw time
        snprintf(rgch, CCH, "%i:%02i %cM", (0 == hour % 12) ? 12 : hour % 12, minute, hour < 12 ? 'A' : 'P');
        DrawCentered(paint, y, rgch, fontBig);
        y += fontBig.Height;

        // Draw level
        snprintf(rgch, CCH, "%i ft.", level);
        DrawCentered(paint, y, rgch, fontSmall);
        y += fontSmall.Height + 8;
      }
      */
    }
  Serial.print("*bool");
  Serial.println(fLow);
  Serial.print("*tide hour");
  Serial.println(hour);
  Serial.print("*currentHour");
  Serial.print(hourCurrent);
  Serial.print("*TideMinute");
  Serial.println(minute); 
  //if(minute > 30 )hour ++;
  //if(minuteCurrent > 30) hourCurrent ++;
  
  if(hourCurrent < hour) hourCurrent = hourCurrent + 24;
  hour = (hour * 60) + minute;
  hourCurrent = (hourCurrent * 60) + minuteCurrent;
  int timeToGo = hourCurrent - hour;
  if(fLow) lightLevel = map(timeToGo, 0,360, 0,60);
  else lightLevel = map(timeToGo, 0,360, 60,0);
  Serial.print("lightLevel");
  Serial.println(lightLevel);
  }
}
void fillnoise8() {
 
  #define scale 30                                                          // Don't change this programmatically or everything shakes.
  
  for(int i = 0; i < lightLevel; i++) {                                       // Just ONE loop to fill up the LED array as all of the pixels change.
    uint8_t index = inoise8(i*scale, millis()/10+i*scale);                   // Get a value from the noise function. I'm using both x and y axis.
    leds[i] = ColorFromPalette(currentPalette, index, 255, LINEARBLEND);    // With that value, look up the 8 bit colour palette value and assign it to the current LED.
  }
 
} // fillnoise8()
void fill_time(){
  #define scale 30                                                          // Don't change this programmatically or everything shakes.
  
   
   
  for(int i = lightLevel - 1; i < 60; i++) {                                       // Just ONE loop to fill up the LED array as all of the pixels change.
    uint8_t index = inoise8(i*scale, millis()/10+i*scale);                   // Get a value from the noise function. I'm using both x and y axis.
    leds[i] = ColorFromPalette(otherPalette, index, 255, LINEARBLEND);    // With that value, look up the 8 bit colour palette value and assign it to the current LED.
  }
 
}
void fill_grad() {
  
  uint8_t starthue = beatsin8(5, 0, 255);
  uint8_t endhue = beatsin8(7, 0, 0);
  
  if (starthue < endhue) {
    fill_gradient(leds, NUM_LEDS, CHSV(starthue,255,255), CHSV(endhue,255,255), FORWARD_HUES);    // If we don't have this, the colour fill will flip around. 
  } else {
    fill_gradient(leds, NUM_LEDS, CHSV(starthue,255,255), CHSV(endhue,255,255), BACKWARD_HUES);
  }
  
} // fill_grad()
