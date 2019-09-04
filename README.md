# Arduino Noël 2018 Papa

This is a personal project that I gave to my Father as a present for Christmas 2018. The main goal of this project is to create a mail alert system - a system that notifies the household when the mailbox outside is full. That way instead of walking outside to check the mail, the receiver inside of the house tells you when mail has been received. This device also warns you when the battery is low and needs to be changed. 

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
