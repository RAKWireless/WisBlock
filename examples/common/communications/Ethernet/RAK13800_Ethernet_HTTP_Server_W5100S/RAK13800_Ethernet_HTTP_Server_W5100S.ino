/**
   @file RAK13800_Ethernet_HTTP_Server_W5100S.ino
   @author rakwireless.com
   @brief  A simple web server that shows the value of the analog input pins.
   @version 0.1
   @date 2021-11-02
   @copyright Copyright (c) 2021
**/

#include <SPI.h> 
#include <RAK13800_W5100S.h> // Click to install library: http://librarymanager/All#RAKwireless_W5100S

IPAddress ip(192, 168, 0, 177); // Set the static IP address to use if the DHCP fails to assign.

byte gateway[] = { 192, 168, 1, 1 };  // Internet access via router.

byte subnet[] = { 255, 255, 255, 0 }; // Subnet mask.

EthernetServer server(80); // Port where the server can be accessed.

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Set the MAC address, do not repeat in a network.

String g_readString; // Read the response form the user / client

void setup()
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH); // Enable power supply.

  pinMode(WB_IO3, OUTPUT);
  digitalWrite(WB_IO3, LOW);  // Reset Time.
  delay(100);
  digitalWrite(WB_IO3, HIGH);  // Reset Time.
    
  pinMode(LED_GREEN, OUTPUT);
  
  time_t timeout = millis();
  // Initialize Serial for debug output.
  Serial.begin(115200);
  while (!Serial)
  {
    if ((millis() - timeout) < 5000)
    {
      delay(100);
    }
    else
    {
      break;
    }
  }
  Serial.println("RAK13800 Ethernet HTTP Server example.");
  
  Ethernet.init( SS );  // Start Ethernet.
  Ethernet.begin(mac, ip, gateway, subnet); // Initialize ethernet.
  server.begin(); // Start the server
}
void loop()
{
  EthernetClient client = server.available(); // Create a client connection.

  if (client) // Check if someone is tried access the assigned IP address over a browser.
  {
    while (client.connected()) // If connected, continue checking if client is sending some message back to the server.
    {
      if (client.available()) 
      {
        char c = client.read(); // Read char by char HTTP request             
        
        if (g_readString.length() < 100) 
        {
          g_readString += c; // Store characters to string
        }
        
        if (c == 0x0D) // If HTTP request has ended– 0x0D is Carriage Return \n ASCII
        {
          client.println("HTTP/1.1 200 OK"); // Send new page
          client.println("Content-Type: text/html");
          client.println();
          client.println("<HTML>");
          client.println("<HEAD>");
          client.println("<TITLE> RAK13800 ETHERNET</TITLE>");
          client.println("</HEAD>");
          client.println("<BODY>");
          client.println("<hr>");
          client.println("<H1 style=\"color:green;text-align:center\">ETHERNET LED CONTROL</H1>");
          client.println("<hr>");
          client.println("<br>");
          
          client.println("<H2 style=\"text-align:center\"><a href=\"/?LEDON\"\">Turn On LED</a><br></H2>"); // Creating a link to redirect the user to turn on the light.
         
          client.println("<H2 style=\"text-align:center\"><a href=\"/?LEDOFF\"\">Turn Off LED</a><br></H2>"); // Creating a link to redirect the user to turn off the light
          client.println("<br>");
          client.println("</BODY>");
          client.println("</HTML>");
          delay(10);

          client.stop();//stopping client
          if(g_readString.indexOf("?LEDON") > -1) //checks for LEDON
          {
            digitalWrite(LED_GREEN, HIGH);  // control arduino pin with URL
          }
          else
          {
            if(g_readString.indexOf("?LEDOFF") > -1) //checks for LEDOFF
            {
              digitalWrite(LED_GREEN, LOW); // set pin low
            }
          }
          g_readString=""; //clearing string for next read
        }
      }
    }
  }
}
