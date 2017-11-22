#include <WiFi.h>

// EDIT: Change the 'ssid' and 'password' to match your network
char ssid[] = "kok";  // wireless network name
char password[] = "12312567"; // wireless password
int status = WL_IDLE_STATUS;
WiFiClient client;

// EDIT: 'Server' address to match your domain
char server[] = "192.168.137.1"; // This could also be 192.168.1.18/~me if you are running a server on your computer on a local network.

// This is the data that will be passed into your POST and matches your mysql column
int yourarduinodata = 999;
String yourdatacolumn = "x=";
String yourdata;

void setup() {
  Serial.begin(115200);
  
  connectWifi();

  // You're connected now, so print out the status
  printWifiStatus();
  
  postData();
}

void loop() {
postData();
}

void connectWifi() {
  // Attempt to connect to wifi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, password);
    // Wait 10 seconds for connection
    delay(100);
  }
}

void printWifiStatus() {
  // Print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // Print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

// This method makes a HTTP connection to the server and POSTs data
void postData() {
  // Combine yourdatacolumn header (yourdata=) with the data recorded from your arduino
  // (yourarduinodata) and package them into the String yourdata which is what will be
  // sent in your POST request
  yourdata = yourdatacolumn + yourarduinodata;

  // If there's a successful connection, send the HTTP POST request
  if (client.connect(server, 3000)) {
    Serial.println("connecting...");

    // EDIT: The POST 'URL' to the location of your insert_mysql.php on your web-host
    client.println("POST /left-click HTTP/1.1");

    // EDIT: 'Host' to match your domain
    client.println("Host: 192.168.137.1:3000");
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.print("Content-Length: ");
    client.println(yourdata.length());
    client.println();
    //client.println(yourdata); 
  } 
  else {
    // If you couldn't make a connection:
    Serial.println("Connection failed");
    Serial.println("Disconnecting.");
    client.stop();
  }
}
