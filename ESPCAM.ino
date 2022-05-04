#include <WebServer.h>
#include <WiFi.h>
#include <esp32cam.h>

#define PE1 digitalRead(pe1)
#define PE2 digitalRead(pe2)
const int pe1 = 12;
const int pe2 = 13;
const int pe3 = 15;
const int pe4 = 14;
const int pintrigger =2 ;
const int LED=33;
unsigned int distancia1, distancia2,distancia3,distancia4;
 
const char* WIFI_SSID = "Megacable_7980";
const char* WIFI_PASS = "7777225690";
 
WebServer server(80);
 
 
static auto loRes = esp32cam::Resolution::find(320, 240);
static auto midRes = esp32cam::Resolution::find(350, 530);
static auto hiRes = esp32cam::Resolution::find(800, 600);

static int FLASH=4;
void serveJpg()
{
  auto frame = esp32cam::capture();
  if (frame == nullptr) {
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");
    return;
  }
  Serial.printf("CAPTURE OK %dx%d %db\n", frame->getWidth(), frame->getHeight(),
                static_cast<int>(frame->size()));
 
  server.setContentLength(frame->size());
  server.send(200, "image/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);
}
 
void handleJpgLo()
{
  if (!esp32cam::Camera.changeResolution(loRes)) {
    Serial.println("SET-LO-RES FAIL");
  }
  serveJpg();
}
 
void handleJpgHi()
{
  if (!esp32cam::Camera.changeResolution(hiRes)) {
    Serial.println("SET-HI-RES FAIL");
  }
  serveJpg();
}
 
void handleJpgMid()
{
  if (!esp32cam::Camera.changeResolution(midRes)) {
    Serial.println("SET-MID-RES FAIL");
  }
  serveJpg();
}
 

void sensoresObstaculos(){
   // ENVIAR PULSO DE DISPARO EN EL PIN "TRIGGER"
  digitalWrite(pintrigger, LOW);
  delayMicroseconds(2);
  digitalWrite(pintrigger, HIGH);
  // EL PULSO DURA AL MENOS 10 uS EN ESTADO ALTO
  delayMicroseconds(10);
  digitalWrite(pintrigger, LOW);
//Serial.println("Pulso Enviado");
unsigned int y1 = 0;
unsigned int y2 = 0;
unsigned int y3 = 0;
unsigned int y4 = 0;


  b00:
  if(PE1 == LOW && PE2 == LOW){
    delayMicroseconds(1);
    //Serial.println("Esperando rebote");
    goto b00;
  }

  bi:
    if(PE1 == HIGH){
      y1++;
    }
    if(PE2 == HIGH){
      y2++;
    }
    //Descomentar para usar cuatro sensores
    /*if(PE3 == HIGH){
      y3++;
    }
    if(PE4 == HIGH){
      y4++;
    }*/

    if(PE1 == LOW && PE2 == LOW){
    //Descomentar para usar cuatro sensores
    //if(PE1 == LOW && PE2 == LOW && PE3 == LOW && PE4 == LOW){ 
      goto bf;
    }
  
  delayMicroseconds(1);
  goto bi;
  bf:

  //Por calibración, 20 fue el mejor valor, al menos de manera preliminar... 
  distancia1 = y1 / 20;
  distancia2 = y2 / 20; 
 
  //Serial
  Serial.print(distancia1);
  Serial.print(" --cm[1] ");
  Serial.print(distancia2);
  Serial.println(" --cm[2] ");
  if(y1<10)
    digitalWrite(LED,LOW);
  delay(200);
 
  }
 
void  setup(){
  Serial.begin(115200);
  Serial.println();
  pinMode(FLASH, OUTPUT);
  pinMode(LED,OUTPUT);
  {
    using namespace esp32cam;
    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(hiRes);
    cfg.setBufferCount(2);
    cfg.setJpeg(80);
 
    bool ok = Camera.begin(cfg);
    Serial.println(ok ? "CAMERA OK" : "CAMERA FAIL");
  }
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.print("http://");
  Serial.println(WiFi.localIP());
  Serial.println("  /cam-lo.jpg");
  Serial.println("  /cam-hi.jpg");
  Serial.println("  /cam-mid.jpg");
 
  server.on("/cam-lo.jpg", handleJpgLo);
  server.on("/cam-hi.jpg", handleJpgHi);
  server.on("/cam-mid.jpg", handleJpgMid);
 
  server.begin();
  digitalWrite(FLASH,HIGH);

  pinMode(pe1, INPUT);
  pinMode(pe2, INPUT);
  pinMode(pintrigger, OUTPUT);
}
 
void loop()
{
  //digitalWrite(LED,HIGH);
  server.handleClient();
  //sensoresObstaculos();
}
