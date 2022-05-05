//********************************Variables para Configuración AccessPoint*********************
#include <WiFi.h>
const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";
WiFiServer server(80);
String header;

void setup() {
  Serial.begin(115200);
  accessPointInit();
}

void loop(){
  visionPelotas();//Busca centro de pelota, 0-centro,1-derecha,2-izquierda, 3-sin objetivo
}
void accessPointInit(){
  WiFi.softAP(ssid, password);
  Serial.print("AccessPoint IP address: ");
  Serial.println(WiFi.softAPIP());
  server.begin();
}

int visionPelotas()
{
int ball;
WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
     
            if (header.indexOf("right") >= 0){ 
              Serial.println("Pelota a la derecha");
              ball=1;
            }
            if (header.indexOf("left") >= 0){ 
              Serial.println("Pelota a la izquierda");
              ball=2;
            }
            if (header.indexOf("null") >= 0) {
              Serial.println("NO se detecta pelota");
              ball=3;
            }
            if (header.indexOf("center") >= 0){
              Serial.println("Pelota en el centro");
              ball=0;
            }
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  return ball;
} 
