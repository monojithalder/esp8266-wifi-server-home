/* 
 ESP8266 BlinkWithoutDelay by Simon Peter
 Blink the blue LED on the ESP-01 module
 Based on the Arduino Blink without Delay example
 This example code is in the public domain
 
 The blue LED on the ESP-01 module is connected to GPIO1 
 (which is also the TXD pin; so we cannot use Serial.print() at the same time)
 
 Note that this sketch uses LED_BUILTIN to find the pin with the internal LED
*/
#include <stdlib.h>
#include <RestClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include<SoftwareSerial.h> //Included SoftwareSerial Library
//Started SoftwareSerial at RX and TX pin of ESP8266
SoftwareSerial s(2,3);
int receiveddata =0;
ESP8266WebServer server(80);
const char* ssid     = "WEBTECHLINK";
const char* password = "web2013techlink";
IPAddress ip(192,168,1, 150);
IPAddress gateway(192,168,1,1); // set gateway to match your network
IPAddress subnet(255, 255, 255, 0);
void setup() {
// Connect to Wi-Fi network with SSID and password
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  server.on("/specificArgs", handleSpecificArg);
  server.on("/status", handleStatus);
  server.on("/processRequest",processRequest);
  server.begin();
  delay(500);
  //Serial1.begin(9600);
  s.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
  if(s.available()) {
      //message += "Pin is "+ s.read();
      String response = "";
      int statusCode;
      RestClient client = RestClient("192.168.1.6");
      statusCode = client.post("/?foo=bar","" , &response);
  }
  server.handleClient(); 
}

void handleSpecificArg() { 

  String message = "";  
  if (server.arg("Temperature")== ""){     //Parameter not found
  message = "Temperature Argument not found";
  }
  else{     //Parameter found
  message = "Temperature Argument = ";
  char post_data_char[2];
  String post_data_string = "";
  int post_data = 0;
  String response = "";
  int statusCode;
  char response_data[500];
  char post_data_body[100] = "foo=bar"; 
  RestClient client = RestClient("192.168.1.6");
 
  post_data_string = server.arg("Temperature");
  post_data_string.toCharArray(post_data_char,2);
  post_data = atoi(post_data_char);
  statusCode = client.post("/?foo=bar","" , &response);
  response.toCharArray(response_data,500);
  s.write(post_data);
  message += server.arg("Temperature");     //Gets the value of the query parameter
  char test_data = 2;
  message += " Status Code ";
  message += statusCode;
  message += " Response ";
  message += response_data;
  }
  delay(100);
  server.send(200, "text/plain", message);          //Returns the HTTP response

}
void handleStatus() {
  String message = "";
  if(server.arg("item_no") == "") {
    message = "{'err_msg' : 'Invalid Item No'}";
  }
  else {
    message = "";
    char post_data_char[2];
    String post_data_string = "";
    String reccived_data = "";
    int post_data = 0;
 
    post_data_string = server.arg("item_no");
    post_data_string.toCharArray(post_data_char,2);
    post_data = atoi(post_data_char);
    int pin_status = EEPROM.read(post_data);
    message = "{'pin_status': ";
    message += pin_status;
    message += " }";
  }
  server.send(200, "application/json", message); 
}

void processRequest() {
  String message = "";
  if(server.arg("item_no") == "") {
    message = "{'err_msg' : 'Invalid Item No'}";
  }
  else {
    String post_data_string = "";
    char post_data_char[2];
    int post_data = 0;
    int pin_status = 0;
    post_data_string = server.arg("item_no");
    post_data_string.toCharArray(post_data_char,2);
    post_data = atoi(post_data_char);
    s.write(post_data);
    if(EEPROM.read(post_data) == 0) {
      pin_status = 1;
    }
    EEPROM.write(post_data,pin_status);
    message = "{'success' : '1'}";
  }
  server.send(200, "application/json", message); 
}

