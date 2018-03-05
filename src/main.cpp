#include <ESP8266WebServer.h>
#include <Wire.h>
#include <SPI.h>
#include <U8g2lib.h>

U8G2_SSD1306_64X48_ER_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE); // oled instance
WiFiServer server(80); // wifi server instance

//global valriables
const char *ssid = "MyEsp8266";
const char *password = "Password";
int SCREEN_WIDTH = 64;
int SCREEN_HEIGHT = 48;
int SHAPE_SIZE = 600;

float d = 3;
float px[] = {-d, d, d, -d, -d, d, d, -d};
float py[] = {-d, -d, d, d, -d, -d, d, d};
float pz[] = {-d, -d, -d, -d, d, d, d, d};
float p2x[8] = {0};
float p2y[8] = {0};
float r[3] = {0};


void drawCube() // draw the cube on oled
{
    r[0] = r[0] + PI / 180.0;
    r[1] = r[1] + PI / 180.0;
    r[2] = r[2] + PI / 180.0;
    if (r[0] >= 360.0 * PI / 180.0)
    {
        r[0] = 0;
    }
    if (r[1] >= 360.0 * PI / 180.0)
    {
        r[1] = 0;
    }
    if (r[2] >= 360.0 * PI / 180.0)
    {
        r[2] = 0;
    }

    for (int i = 0; i < 8; i++)
    {
        float px2 = px[i];
        float py2 = cos(r[0]) * py[i] - sin(r[0]) * pz[i];
        float pz2 = sin(r[0]) * py[i] + cos(r[0]) * pz[i];

        float px3 = cos(r[1]) * px2 + sin(r[1]) * pz2;
        float py3 = py2;
        float pz3 = -sin(r[1]) * px2 + cos(r[1]) * pz2;

        float ax = cos(r[2]) * px3 - sin(r[2]) * py3;
        float ay = sin(r[2]) * px3 + cos(r[2]) * py3;
        float az = pz3 - 150;

        p2x[i] = SCREEN_WIDTH / 2 + ax * SHAPE_SIZE / az;
        p2y[i] = SCREEN_HEIGHT / 2 + ay * SHAPE_SIZE / az;
    }

    oled.clearBuffer(); // clear the oled buffer
    for (int i = 0; i < 3; i++)
    {
        oled.drawLine(p2x[i], p2y[i], p2x[i + 1], p2y[i + 1]);
        oled.drawLine(p2x[i + 4], p2y[i + 4], p2x[i + 5], p2y[i + 5]);
        oled.drawLine(p2x[i], p2y[i], p2x[i + 4], p2y[i + 4]);
    }
    oled.drawLine(p2x[3], p2y[3], p2x[0], p2y[0]);
    oled.drawLine(p2x[7], p2y[7], p2x[4], p2y[4]);
    oled.drawLine(p2x[3], p2y[3], p2x[7], p2y[7]);
    oled.sendBuffer(); // dump the next cube frame on the screen
}

void blink() // blink function
{
    for (int i = 0; i < 8; i++) // Blink the led 4 times
    {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        delay(500);
    }
}

void wifiOps() // the wifi client operations
{
    WiFiClient client = server.available(); // listen for incoming clients

    if (client) // if you get a client,
    {
        String currentLine = "";   // make a String to hold incoming data from the client
        while (client.connected()) // loop while the client's connected
        {
            if (client.available()) // if there's bytes to read from the client,
            {
                char c = client.read(); // read a byte, then
                if (c == '\n')          // if the byte is a newline character
                {
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0)
                    {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();
                        // the content of the HTTP response follows the header:
                        client.print("<head><title>Test Led http protocol</title></head>");
                        client.print("<center><h1><b>ESP8266 Web Control!!</b></h1></center>");
                        client.print("<center><p><b>Blue LED</b><a href=\"ON1\"><button>ON</button></a>&nbsp;<a href=\"OFF1\"><button>OFF</button></a></p></center>");
                        client.print("<center><h1>Effects!!!!</h1></center>");
                        client.print("<center><p>Blink<b>(2 Secs Aprox)</b><a href=\"BLINK\"><button>ON</button></center>");
                        // The HTTP response ends with another blank line:
                        client.println();
                        // break out of the while loop:
                        break;
                    }
                    else
                    { // if you got a newline, then clear currentLine:
                        currentLine = "";
                    }
                }
                else if (c != '\r') // if you got anything else but a carriage return character,
                {
                    currentLine += c; // add it to the end of the currentLine
                }
                // Check to see if the client request was "GET /ON" or "GET /OFF":
                if (currentLine.endsWith("GET /ON1"))
                {
                    digitalWrite(LED_BUILTIN, LOW); // GET /ON turns the LED on
                }
                if (currentLine.endsWith("GET /OFF1"))
                {
                    digitalWrite(LED_BUILTIN, HIGH); // GET /OFF turns the LED off
                }
                if (currentLine.endsWith("GET /BLINK"))
                {
                    blink();
                }
            }
        }
        // close the connection:
        client.stop();
    }
}

void setup() // arduino setup
{
    pinMode(LED_BUILTIN, OUTPUT); // setting the led pin mode
    digitalWrite(LED_BUILTIN, HIGH); // make sure the LED is OFF

    WiFi.softAP(ssid, password); // Setting the Wifi Access Point mode
    server.begin(); // Starting the wifi Server engine

    oled.begin(); // Sarting the Oled
    oled.clearBuffer();// Making sure the oled buffer is empty
}

void loop()
{
    drawCube(); // draw the cube
    wifiOps(); // perform the wifi operations
}
