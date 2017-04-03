
/* Analog Input Example, Teensyduino Tutorial #4
   http://www.pjrc.com/teensy/tutorial4.html

   After uploading this to your board, use Serial Monitor
   to view the message.  When Serial is selected from the
   Tools > USB Type menu, the correct serial port must be
   selected from the Tools > Serial Port AFTER Teensy is
   running this code.  Teensy only becomes a serial device
   while this code is running!  For non-Serial types,
   the Serial port is emulated, so no port needs to be
   selected.

   This example code is in the public domain.
*/

void setup()
{                
  Serial.begin(38400);
  delay(1000);
}

int val;

void loop()                     
{
  Serial.println("---Delay 10");
  for (int i = 0; i < 100; i++) {
    val = analogRead(22);
    Serial.println(val);
    delay(10);
  }
  Serial.println("---Delay 1");
  for (int i = 0; i < 100; i++) {
    val = analogRead(22);
    Serial.println(val);
    delay(1);
  }
  Serial.println("---Delay 25");
  for (int i = 0; i < 100; i++) {
    val = analogRead(22);
    Serial.println(val);
    delay(25);
  }
  Serial.println("---Delay 50");
  for (int i = 0; i < 100; i++) {
    val = analogRead(22);
    Serial.println(val);
    delay(50);
  }
}

