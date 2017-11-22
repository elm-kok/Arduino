//Acelerometro com ESP8266 NodeMCU
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
const int MPU_ADDR =      0x68;
const int WHO_AM_I =      0x75;
const int PWR_MGMT_1 =    0x6B;
const int GYRO_CONFIG =   0x1B;
const int ACCEL_CONFIG =  0x1C;
const int ACCEL_XOUT =    0x3B;
const int sda_pin = D5;
const int scl_pin = D6;
const int  buttonPin = D0;
int buttonPushCounter = 0;
int buttonState = 0;
int lastButtonState = 0;
double delT = 0.1, x, y, sx = 0, sy = 0;
int16_t AcX, AcY, AcZ;
const char* SSID = "G_6339";
const char* PASSWORD = "aassddaa";
const char* rpiHost = "192.168.43.213";
WiFiClient client;
HTTPClient http;
StaticJsonBuffer<300> jsonBuffer;
JsonObject& object = jsonBuffer.createObject();
JsonObject& data = object.createNestedObject("data");
JsonObject& accel = data.createNestedObject("accel");
JsonObject& accelX = accel.createNestedObject("accelX");
JsonObject& accelY = accel.createNestedObject("accelY");
JsonObject& accelZ = accel.createNestedObject("accelZ");
void initI2C()
{
  Wire.begin(sda_pin, scl_pin);
}
void writeRegMPU(int reg, int val)
{
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission(true);
}
uint8_t readRegMPU(uint8_t reg)
{
  uint8_t data;
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 1);
  data = Wire.read();
  return data;
}
void findMPU(int mpu_addr)
{
  Wire.beginTransmission(MPU_ADDR);
  int data = Wire.endTransmission(true);

  if (data == 0)
  {
    Serial.print("OK-> 0x");
    Serial.println(MPU_ADDR, HEX);
  }
  else
  {
    Serial.println("cannot find MPU!");
  }
}
void checkMPU(int mpu_addr)
{
  findMPU(MPU_ADDR);

  int data = readRegMPU(WHO_AM_I);

  if (data == 104)
  {
    Serial.println("MPU6050 OK! (104)");

    data = readRegMPU(PWR_MGMT_1);

    if (data == 64) Serial.println("MPU6050 SLEEP! (64)");
    else Serial.println("MPU6050 ACTIVE!");
  }
  else Serial.println("inACTIVE MPU6050");
}
void initMPU()
{
  setSleepOff();
  setAccelScale();
}

void setSleepOff()
{
  writeRegMPU(PWR_MGMT_1, 0);
}
void setAccelScale()
{
  writeRegMPU(ACCEL_CONFIG, 0);
}
void readRawMPU()
{
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(ACCEL_XOUT);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14);

  AcX = Wire.read() << 8;
  AcX |= Wire.read();
  AcY = Wire.read() << 8;
  AcY |= Wire.read();
  AcZ = Wire.read() << 8;
  AcZ |= Wire.read();
  //sx =  AcX * delT * delT * 0.5;
  //sy =  AcY * delT * delT * 0.5;
  sy=-1*AcY/1000;
  sx=-1*AcX/1000;
  Serial.print("X = "); Serial.print(sx);
  Serial.print(" | Y = "); Serial.print(sy);
  x = (double)AcX * delT * delT * 0.5;
  y = (double)AcY * delT * delT * 0.5;
  Serial.println("");
  //delay(delT * 1000);
}
void reconnectWiFi()
{
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.println(SSID);
  Serial.print("IP : ");
  Serial.println(WiFi.localIP());
}

void initWiFi()
{
  delay(10);
  Serial.println(SSID);
  reconnectWiFi();
}

void populateJSON()
{
  object["nodeID"] = "mcu1";
  accel["accelX"] = AcX;
  accel["accelY"] = AcY;
  accel["accelZ"] = AcZ;
}

void makePOST()
{
  String PostData = "x=" + (String)sx + "&y=" + (String)sy;
  Serial.println(PostData);
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected");
    HTTPClient http;
    http.begin("http://192.168.43.213:3000/move-by");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST(PostData);
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    http.end();
  }
}
void leftCli() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected");
    HTTPClient http;
    http.begin("http://192.168.43.213:3000/left-click");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST("");
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    http.end();
  }
}


void setup() {
  pinMode(buttonPin, INPUT);
  Serial.begin(115200);
  initWiFi();
  initI2C();
  initMPU();
  checkMPU(MPU_ADDR);
}

void loop() {
  readRawMPU();
  populateJSON();
  makePOST();
  buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      buttonPushCounter++;
      Serial.print("number of button pushes: ");
      Serial.println(buttonPushCounter);
      leftCli();
    }
  }
  lastButtonState = buttonState;
}
