/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com
*********/

// Load Wi-Fi library
#include <WiFi.h>

// pin defs
#define MOTOR1_DIR1 26
#define MOTOR1_DIR2 25
#define MOTOR2_DIR1 17
#define MOTOR2_DIR2 16
#define EN1 27
#define EN2 14

// Replace with your network credentials
const char* ssid     = "AndroidAP";
const char* password = "parammal";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String left = "off";
String right = "off";
String forward = "off";

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(MOTOR1_DIR1, OUTPUT);
  pinMode(MOTOR1_DIR2, OUTPUT);
  pinMode(MOTOR2_DIR1, OUTPUT);
  pinMode(MOTOR2_DIR2, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  // Set outputs to LOW
  digitalWrite(MOTOR1_DIR1, LOW);
  digitalWrite(MOTOR1_DIR2, LOW);
  digitalWrite(MOTOR2_DIR1, LOW);
  digitalWrite(MOTOR2_DIR2, LOW);
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
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
            if (header.indexOf("GET /forward/on") >= 0)
            {
              forward = "on";
            }
            else if (header.indexOf("GET /forward/off") >= 0)
            {
              forward = "off";
            }

            if (forward == "on")
            {
              if (header.indexOf("GET /left/on") >= 0) {
                Serial.println("left on");
                left = "on";
                digitalWrite(MOTOR1_DIR1, HIGH);
                digitalWrite(MOTOR1_DIR2, LOW);
                right = "off";
                digitalWrite(MOTOR2_DIR1, HIGH);
                digitalWrite(MOTOR2_DIR2, HIGH);
              } else if (header.indexOf("GET /left/off") >= 0) {
                Serial.println("left off move");
                left = "off";
                digitalWrite(MOTOR1_DIR1, HIGH);
                digitalWrite(MOTOR1_DIR2, LOW);
                right = "off";
                digitalWrite(MOTOR2_DIR1, HIGH);
                digitalWrite(MOTOR2_DIR2, LOW);
              } else if (header.indexOf("GET /right/on") >= 0) {
                Serial.println("right on");
                left = "off";
                digitalWrite(MOTOR1_DIR1, HIGH);
                digitalWrite(MOTOR1_DIR2, HIGH);
                right = "on";
                digitalWrite(MOTOR2_DIR1, HIGH);
                digitalWrite(MOTOR2_DIR2, LOW);
              } else if (header.indexOf("GET /right/off") >= 0) {
                Serial.println("right off move");
                left = "off";
                digitalWrite(MOTOR1_DIR1, HIGH);
                digitalWrite(MOTOR1_DIR2, LOW);
                right = "off";
                digitalWrite(MOTOR2_DIR1, HIGH);
                digitalWrite(MOTOR2_DIR2, LOW);
              }
              else
              {
                Serial.println("move");
                left = "off";
                digitalWrite(MOTOR1_DIR1, HIGH);
                digitalWrite(MOTOR1_DIR2, LOW);
                right = "off";
                digitalWrite(MOTOR2_DIR1, HIGH);
                digitalWrite(MOTOR2_DIR2, LOW);
              }
            }
            else
            {
              Serial.println("stop");
              left = "off";
              digitalWrite(MOTOR1_DIR1, HIGH);
              digitalWrite(MOTOR1_DIR2, HIGH);
              right = "off";
              digitalWrite(MOTOR2_DIR1, HIGH);
              digitalWrite(MOTOR2_DIR2, HIGH);
              forward = "off";
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>button{background-color: #4CAF50;border: none;color: white;padding: 16px 40px;text-decoration: none;font-size: 30px;margin: 2px;cursor: pointer;}");
            client.println(".btn{position: absolute;bottom: 0}");
            client.println(".left{left: 0;}");
            client.println(".right{right: 0;}</style></head>");

            // Web Page Heading
            client.println("<body><h1 align=\"center\">SRA Remote Control</h1>");

            if (forward == "off")
              client.println("<a href=\"/forward/on\"><button class=\"forward\" style=\"float:right\">MOVE</button></a>");
            else
              client.println("<a href=\"/forward/off\"><button class=\"forward\" style=\"float:right\">STOP</button></a>");

            if (left == "off")
              client.println("<div><a href=\"/left/on\"><button class=\"btn left\">LEFT</button></a>");
            else
              client.println("<div><a href=\"/left/off\"><button class=\"btn left\">MOVE</button></a>");

            if (right == "off")
              client.println("<a href=\"/right/on\"><button class=\"btn right\">RIGHT</button></a></div>");
            else
              client.println("<a href=\"/right/off\"><button class=\"btn right\">MOVE</button></a></div>");

            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // turns the GPIOs on and off

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
}
