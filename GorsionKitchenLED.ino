// LiTtl3.1 Industries license agreement.
//
// Under no circumstances is this component (or portion thereof) to be in any
// way affected or brought under the terms of any Open Source License without
// the prior express written permission of LiTtl3.1 Industries.
//
// For the purpose of this clause, the term Open Source Software/Component
// includes:
//
// (i) any software/component that requires as a condition of use, modification
//     and/or distribution of such software/component, that such software/
//     component:
//     a. be disclosed or distributed in source code form;
//     b. be licensed for the purpose of making derivative works; and/or
//     c. can be redistributed only free of enforceable intellectual property
//        rights (e.g. patents); and/or
// (ii) any software/component that contains, is derived in any manner (in whole
//      or in part) from, or statically or dynamically links against any
//      software/component specified under (i).

#include <SPI.h>
#include <WiFiNINA.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
    #include <avr/power.h>
#endif

#include "arduino_secrets.h" 

#define PIN_LED     3

#define NUM_PIXELS  126

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(80);

// current state
//int state = 0;
int state = 2;  // auto-start -- testing

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, PIN_LED, NEO_GRB + NEO_KHZ800);

// CUSTOM COLORS
const uint32_t colores[]={
    pixels.Color(150,0,150),            //pink
    pixels.Color(29,219,191),           //light blue
    pixels.Color(245,77,77),            //light red
    pixels.Color(53,143,4),             //mostly green
    pixels.Color(26,143,125),           //mostly teal
    pixels.Color(49,245,96)             //mostly green
};
uint32_t black = pixels.Color(0,0,0);

// GLOBAL DELAY
int delayval = 50; // delay for half a second

// SNAKE CONFIG
int snakelen = 10;   // how long the snake should be
int snakedelay = 30; // regulates speed of the snake
bool finished = false;

// SPARKLE CONFIG 
#define SIZE_ONS 30
int ons[SIZE_ONS]; // random array to hold IDs of lit LEDs

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
      if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  pinMode(LED_BUILTIN, OUTPUT);   // PIN 13 LED

  pixels.begin();     // This initializes the NeoPixel library

  // initialize our ons array
  for (int i=0; i<SIZE_ONS; i++){
    ons[i]=0;
  } 

  Serial.begin(9600);                 // DEBUG

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }  

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status     
}

void loop() {
  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
  Serial.println("Head of loop");     // DEBUG

  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
      Serial.println("new client");           // print a message out the serial port
      String currentLine = "";                // make a String to hold incoming data from the client
      while (client.connected()) {            // loop while the client's connected
          if (client.available()) {             // if there's bytes to read from the client,
              char c = client.read();             // read a byte, then
              Serial.write(c);                    // print it out the serial monitor
              if (c == '\n') {                    // if the byte is a newline character

                  // if the current line is blank, you got two newline characters in a row.
                  // that's the end of the client HTTP request, so send a response:
                  if (currentLine.length() == 0) {
                      // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                      // and a content-type so the client knows what's coming, then a blank line:
                      client.println("HTTP/1.1 200 OK");
                      client.println("Content-type:text/html");
                      client.println();

                      // the content of the HTTP response follows the header:
                      client.print("Click <a href=\"/pattern1\">here</a> to run pattern1<br>");
                      client.print("Click <a href=\"/pattern2\">here</a> to run pattern2<br>");
                      client.print("Click <a href=\"/pattern3\">here</a> to run pattern3<br>");
                      client.print("Click <a href=\"/pattern4\">here</a> to run pattern4<br>");
                      client.print("Click <a href=\"/pattern5\">here</a> to run pattern5<br>");
                      client.print("Click <a href=\"/pattern6\">here</a> to run pattern6<br>");
                      client.print("Click <a href=\"/off\">here</a> turn the LEDs off<br>");

                      // The HTTP response ends with another blank line:
                      client.println();
                      // break out of the while loop:
                      break;
                  } else {    // if you got a newline, then clear currentLine:
                      currentLine = "";
                  }
              } else if (c != '\r') {  // if you got anything else but a carriage return character,
                  currentLine += c;      // add it to the end of the currentLine
              }

              finished = false; // on new request reset "finished" flag // DEBUG

              // Check to see if the client request was "GET /H" or "GET /L":
              if (currentLine.endsWith("GET /pattern1")) {
                  //client.print("Running pattern1");
                  //client.println();
                  state = 1;
              }
              if (currentLine.endsWith("GET /pattern2")) {
                  //client.print("Running pattern2");
                  //client.println();
                  state = 2;
              }            
              if (currentLine.endsWith("GET /pattern3")) {
                  //client.print("Running pattern3");
                  //client.println();
                  state = 3;
              }
              if (currentLine.endsWith("GET /pattern4")) {
                  //client.print("Running pattern4");
                  //client.println();
                  state = 4;
              }                                    
              if (currentLine.endsWith("GET /pattern5")) {
                  //client.print("Running pattern5");
                  //client.println();
                  state = 5;
              }
              if (currentLine.endsWith("GET /pattern6")) {
                  //client.print("Running pattern6");
                  //client.println();
                  state = 6;
              }                                
              if (currentLine.endsWith("GET /off")) {
                  //client.print("Turning LEDs off");
                  //client.println();
                  state = 0;
              }              
          }
      }

      // close the connection:
      client.stop();
      Serial.println("client disonnected");
  }
  
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);  

  if (state == 0) {
    Serial.println("Shutting things off"); 
    for (int i=0; i<=NUM_PIXELS;i++){
        uint32_t color=pixels.Color(0,0,0);
        pixels.fill(color,NUM_PIXELS/2,i/2);
        pixels.show();
        delay(5);
    }
    delay(1000);
  }   
  else if (state == 1) {
    Serial.println("Starting pattern1");
    pattern_1();
    delay(1000);
  }
  else if (state == 2) {
    Serial.println("Starting pattern2");  
    pattern_2();
    delay(1000);
  } 
  else if (state == 3) {
    Serial.println("Starting pattern3");  
    pattern_3();
    delay(1000);
  } 
  else if (state == 4) {
    Serial.println("Starting pattern4");  
    pattern_4();
    delay(1000);
  }   
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

// test pattern for now
// SNAKE
void pattern_1(){
  Serial.println("Snaking from start to end");
  uint32_t color1 = colores[random(0,6)];
  for (int i=0; i<=(NUM_PIXELS+snakelen);i++){
    pixels.clear();
    pixels.fill(color1,i-snakelen,snakelen);
    pixels.show();
    delay(snakedelay);
  }
}

// Stove lighting
void pattern_2(){
  Serial.println("Lighting up the stove");
  int midpoint = 98;
  uint32_t color1 = pixels.Color(255,255,200);
  if (finished == false){
    for (int i=0; i<20;i++){
      pixels.clear();
      pixels.fill(color1,midpoint-i,i*2);
      pixels.show();
      delay(snakedelay);
    }
    finished = true;
  }
}

// Sink & dishes lighting
void pattern_3(){
  Serial.println("Lighting up the sink");
  int midpoint = 35;
  int length = 55;
  uint32_t color1 = pixels.Color(0,0,0);
  if (finished == false){
    for (int i=0; i<255;i++){
      uint32_t color1 = pixels.Color(i,i,i);
      pixels.clear();
      pixels.fill(color1,midpoint-(length/2),length);
      pixels.show();
      delay(snakedelay);
    }
    finished = true;
  }
}

// Sparkle sparkle
void pattern_4(){
  Serial.println("Starting to sparkle");

  // set some cute color
  uint8_t color_r=232;
  uint8_t color_g=92;
  uint8_t color_b=60;
  
  // navigate through pixels
  //pic a random pixel to fire
  int in_pix = random(0,NUM_PIXELS);
  Serial.print("random pixel in :");
  Serial.println(in_pix);
  
  // pop a random pixel
  int out_pix_idx = random(0,SIZE_ONS);
  int out_pix = ons[out_pix_idx];
  Serial.print("random pixel out :");
  Serial.println(out_pix);
  
  // fade in next pixel as we fade out previous
  uint32_t color_in=pixels.Color(color_r,color_g,color_b);
  uint32_t color_out=pixels.Color(0,0,0);
  pixels.fill(color_in,in_pix,1); 
  pixels.fill(color_out,out_pix,1);
  pixels.show();

  // push new pixel in place of the one that was popped 
  ons[out_pix_idx] = in_pix;

  //DEBUG
  Serial.print("ons: ");
  for (int i=0; i<SIZE_ONS; i++){
    Serial.print(ons[i]);
    Serial.print(",");
  }
  Serial.println(" ");

  delay(50);  
}
