/*Code for the Arduino Nano of the House's Receiver Module of Papa's gift.
 * This is the final version of the code for this project. If mail has arrived, it receives a message and then lights up an LED to notify the user.
 * It also receives a signal from the module outside if the battery level is low, and if it is low, it lights up an LED to alert the user.
 * 
 * 
 * Written by Thomas Plantin
 * Started on October 27th, 2018
 * Finished on December 29th, 2018
 * 
 */

#include <SoftwareSerial.h>   // Include the library to control the HC-12 module.
SoftwareSerial HC12(10, 11);  //Define the pins of the HC-12 module.

#define mail_LED 6            // LED of the mail.
#define lowBattery_LED 5      // LED of the low battery.

int data_In;                  // Data received from the garden module.

bool messageReceived = false; // Boolean variable to signal successful communication between the transmitter module and the receiver module.

void setup() {

  HC12.begin(9600);                   //Serial Port to the HC-12.
  
  pinMode(mail_LED, OUTPUT);          // Declare the pin modes of the essential signals.
  pinMode(lowBattery_LED, OUTPUT);    

  digitalWrite(mail_LED, HIGH);
  digitalWrite(lowBattery_LED, HIGH);
  delay(100);
  digitalWrite(mail_LED, LOW);
  digitalWrite(lowBattery_LED, LOW);
  delay(100);
  digitalWrite(mail_LED, HIGH);
  digitalWrite(lowBattery_LED, HIGH);
  delay(100);
  digitalWrite(mail_LED, LOW);
  digitalWrite(lowBattery_LED, LOW);
  delay(100);
  digitalWrite(mail_LED, HIGH);
  digitalWrite(lowBattery_LED, HIGH);
  delay(100);
  digitalWrite(mail_LED, LOW);
  digitalWrite(lowBattery_LED, LOW);
  delay(100);
 
}

void loop() {

  while(!HC12.available()){             // Wait for a signal from the garden module.
    // DO NOTHING.
  }
  
  data_In = HC12.parseInt();            // Read serial input.
  
  if(data_In == 1) { 
    digitalWrite(lowBattery_LED, LOW);
    digitalWrite(mail_LED, LOW);
    messageReceived = true;
  }

  if (data_In == 2) { 
    digitalWrite(lowBattery_LED, LOW);
    digitalWrite(mail_LED, HIGH);
    messageReceived = true;
  }

  if (data_In == 3) {
    digitalWrite(lowBattery_LED, HIGH);
    digitalWrite(mail_LED, LOW);
    messageReceived = true;
  }

  if (data_In == 4) {
    digitalWrite(lowBattery_LED, HIGH);
    digitalWrite(mail_LED, HIGH);
    messageReceived = true;
  }

  if(messageReceived == true){            // If the message has been received.
    HC12.println(100);                    // Send a confirmation signal to the module in the garden.
    messageReceived = false;              // Set the message received flag to false.
  }
  
  HC12.flush();                         // Flush the data out of the HC-12 module.
  delay(20);                            // Temporize.
}
