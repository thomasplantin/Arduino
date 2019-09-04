/*Code for the Arduino of the Mailbox Transmitter Module of Papa's gift. 
 * This is the final version of the code for this project. It checks if there is mail in the mailbox, and if there is, it sends a signal to the module in the house to notify to user.
 * It also probes the level of the battery, and if it is low, it alerts the user by sending a signal to the module in the house.
 * This code has been configured in order to minimize the power consumption, such that the battery life can be optimized.
 * 
 * 
 * Written by Thomas Plantin
 * Started on October 27th, 2018
 * Finished on December 29th, 2018
 * 
 * For low power, refer to: https://www.youtube.com/watch?v=urLSDi7SD8M
 * 
 */
 
#include <SoftwareSerial.h> // Include the library to control the HC-12 module.
SoftwareSerial HC12(10,11); // Define the pins of the HC-12 module.

#define mailerLimit 2       // Limit switch of the user.
#define userLimit 3     // Limit switch of the mailman.
#define setHC12 4         // Set pin of the HC-12 module.
#define lowBattery 5      // Pin of the low battery signal from the op amp to the Arduino.

bool boxFull = false;     // Declaration of the Boolean logic to keep track of the state of the box.
bool lastBoxFull = false;

bool batFlag = false;     // Declaration of the Boolean logic to keep track of the state of the battery.
bool lastBatFlag = false;

bool mailInbound = false;     // Boolean variable to signal the presence of the mailman.
bool messageReceived = false; // Boolean variable to signal successful communication between the transmitter module and the receiver module.

int i_limits = 0;             // Integer variable to keep track of the number of times that the door of the user has been opened.
int sentMessage = 0;          // Data to be sent to the house module.

int data_In;                  // Data received from the house module.

int currentMillis = 0;
int initMillis = 0;


void setup() {

  HC12.begin(9600);         //Serial Port to the HC-12.

  pinMode(setHC12, OUTPUT);       // Declare the pin modes of the essential signals.
  pinMode(lowBattery, INPUT);
  pinMode(userLimit, INPUT);
  pinMode(mailerLimit, INPUT);

  digitalWrite(setHC12, HIGH);  // Setting the set pin of the HC-12 HIGH puts it in run mode. Pulling it LOW puts it in set mode.
  
  pinMode(0, OUTPUT);     //Set the rest of the pins as outputs and set them either low or high to save power.
  pinMode(1, OUTPUT);
  digitalWrite(0, LOW);
  digitalWrite(1, LOW);

  for(int i=6; i<10; i++){
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  
  for(int i=12; i<19; i++){
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  
  HC12_sleep();                                  // Function to sleep HC-12 module to save power.
  attachInterrupt(0, digitalInterrupt, RISING); // Attach interrupt pin to wake the Arduino up from its sleep. Interrupt 0 goes to digital pin 2 (user).
  attachInterrupt(1, digitalInterrupt, RISING); // Interrupt 1 goes to digital pin 3 (mailer).
  flash_D13();                                  // Function to flash the LED on pin 13 to signal that the system has been setup properly.
  sleep_chip();                                 // Function to sleep Arduino to save power.
  
}



void loop() {
  
  if(messageReceived){   // If the message has been delivered successfully, put the Arduino to sleep.
    messageReceived = false;
    sleep_chip();
  }

  
  assessLimits();    // Function to evaluate the state of the limit switches and proceed with appropriate logic.
  checkBattery();    // Function to evaluate the state of the battery and proceed with appropriate logic.
  

  if(batFlag == LOW && boxFull == LOW){ // If battery is low and if no mail has been received, send 1.
    sentMessage = 1;
  }

  if(batFlag == LOW && boxFull == HIGH){  // If battery is low and if mail has been received, send 2.
    sentMessage = 2;
  }

  if(batFlag == HIGH && boxFull == LOW){  // If battery is good and if no mail has been received, send 3.
    sentMessage = 3;
  }

  if(batFlag == HIGH && boxFull == HIGH){ // If battery is good and if mail has been received, send 4.
    sentMessage = 4;
  }
  

  if(lastBoxFull != boxFull || lastBatFlag != batFlag){ // If there has been a change between the previous state of the mail box or of the battery.
    HC12_wake();                                        // Function to wake up the HC-12 module.
    delay(50);                                          // Wait for the HC-12 to initialize.
    HC12.println(sentMessage);                          // Send out the appropriate message according to the logic in the loop.
    initMillis = millis();
    
    while(!HC12.available()){                           // Wait for a reply from the house module.
      // DO NOTHING
      currentMillis = millis() - initMillis;

      if(currentMillis >= 1000){
        HC12.println(sentMessage);
        currentMillis = 0;
        initMillis =+ 1000;
      }
    }
    
    data_In = HC12.parseInt();  // Process the data received.

    if(data_In == 100){         // Ensure that the data received is correct.
      flash_D13();              // Function to flash the LED on pin 13 to signal that the right data has been received.
      messageReceived = true;   // Flag to notify that the right message has been received.
    }
    
    HC12.flush();               // Flush out all of the data contained in the HC-12 mocule.
    HC12_sleep();               // Function to sleep HC-12 module to save power.
    delay(20);                  // Temporize with a delay
  }

  lastBoxFull = boxFull;        // Update the state of the box.
  lastBatFlag = batFlag;        // Update the state of the battery.
  
  delay(50);                    // Temporize with a delay
  
}






//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////FUNCTIONS/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void assessLimits(){                                                                  // Function to gauge the level of the limit switches in order to evaluate the state of the system.
  
  if(digitalRead(mailerLimit) == HIGH && digitalRead(userLimit) == LOW && !boxFull){
    mailInbound = true;                                                               //Postman here. No mail in yet.
//    delay(20);
  }

  while(mailInbound){                                                                 // Wait until the postman inserts the mail in the box and closes the lid. 
    if(digitalRead(mailerLimit) == LOW && digitalRead(userLimit) == LOW){
      boxFull = true;                                                                 //When the lid is closed, set a flag to notify that the box is full and that the mail is no longer inbound.
      mailInbound = false;
//      delay(20);              
    }
  }

  if(boxFull && digitalRead(mailerLimit) == LOW && digitalRead(userLimit) == HIGH && i_limits == 0){  // If the user opens the door to collect the mail and the index is equal to 0.
    i_limits ++;                                                                                      // Increment the index from 0 to 1.
//    delay(20);
  }

  if(digitalRead(mailerLimit) == LOW && digitalRead(userLimit) == LOW && i_limits == 1){          // Wait until the user closes the door and the index is equal to 1.
    boxFull = false;                                                                              // Once the door is closed, set a flag to notify that the box is empty.
    i_limits = 0;                                                                                // Reset the index to zero.
//    delay(20);
  }

/////////ALL POSSIBILITIES TO SLEEP CHIP

  if(digitalRead(mailerLimit) == HIGH && digitalRead(userLimit) == HIGH){                           // If both the postman and the user open the mailbox, then put the chip to sleep to save power.
    sleep_chip();
  }

  if(boxFull && digitalRead(mailerLimit) == HIGH && digitalRead(userLimit) == LOW){                 // If the box is full and more mail is inserted, then put the chip to sleep to save power.
    sleep_chip();
  }

  if(!boxFull && digitalRead(mailerLimit) == LOW && digitalRead(userLimit) == HIGH){               // If the box is empty and the user still checks the box, then put the chip to sleep to save power.
    sleep_chip();
  }

  //////////
  
}



void checkBattery(){                                                // Function to check the level of the battery.
  if(digitalRead(lowBattery) == HIGH && batFlag == false){          // If the op amp signals a low battery and the flag for the low battery is false.
    batFlag = true;                                                 // Set that flag true.
  }

  if(digitalRead(lowBattery) == LOW && batFlag == true){            // If the op amp signals a high battery and the flag for the low battery is true.
    batFlag = false;                                                // Set that flag false.
  }
}

void HC12_wake(){                   // Function to awaken the HC-12 module.
  digitalWrite(setHC12, LOW);       // Put the HC-12 in set mode.
  delay(50);
  HC12.println("AT+DEFAULT");       // Command to put the HC-12 in standard mode.
  delay(50);
  digitalWrite(setHC12, HIGH);     // Put the HC-12 in run mode.
}

void HC12_sleep(){                  // Function to put the HC-12 module asleep.
  digitalWrite(setHC12, LOW);       // Put the HC-12 in set mode.
  delay(50);
  HC12.println("AT+SLEEP");        // Command to put the HC-12 asleep.
  delay(50);
  digitalWrite(setHC12, HIGH);     // Put the HC-12 in run mode.
}

void sleep_chip(){          // Function to put the Arduino in sleep mode.
                      //DISABLE ADC
    ADCSRA &= ~(1 << 7);
    
                      //ENABLE SLEEP
    SMCR |= (1 << 2);       // Power down mode.
    SMCR |= 1;              // Enable sleep.
    
                      //BOD DISABLE
    MCUCR |= (3 << 5);                       // Set both BODS and BODSE at the same time.
    MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6);  // Then set the BODS bit clear and the BODSE bit at the same time.
    
    __asm__ __volatile__("sleep");           // ASSEMBLY command to put the Arduino in sleep mode.
}

void flash_D13(){                 // Function to flash the LED on pin 13.
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  delay(100);
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  delay(100);
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  delay(100);
}

void digitalInterrupt(){        // Function to interrupt the deep sleep mode of the Arduino.
}
