/* Source code file defaultcmds.cpp
 * 
 * This file contains the code for the default debugger commands.
 *
 * Author: Benjamin Crall 
 */

#include "./defaultcmds.h"
#include "./debugger.h"

#include <WiFi.h>

void help(int argc, char* argv[], Debugger* dbg) {
  Serial.printf("There are %d commands:\n");
  for(int i = 0; i < dbg->numDebugCommands; i++) {
    Serial.printf("\t%s\n", dbg->commands[i].name);
  }
}

/*
 * Debug command to send text back to the user.
 * Prints all arguments after the first to DEBUG_SERIAL with a space between.
 */
void echo(int argc, char* argv[], Debugger* dbg) {
  for(int num = 1; num < argc; num++) {
    if(num != 0) {
      DEBUG_SERIAL.print(' ');
    }
    DEBUG_SERIAL.print(argv[num]);
  }
  DEBUG_SERIAL.println();
}

/*
 * Turns on the builtin LED probably on pin 13.
 * On the Feaather it is the red one by the USB port
 */
void lumos(int argc, char* argv[], Debugger* dbg) {
  char* arg[] = {"light", "on"};
  light(2, arg, dbg);
}

/*
 * Turns off the builtin LED probably on pin 13.
 * On the Feaather it is the red one by the USB port
 */
void nox(int argc, char* argv[], Debugger* dbg) {
  char* arg[] = {"light", "off"};
  light(2, arg, dbg);
}

/*
 * Controls the builtin LED probably on pin 13.
 * Argument 1 must be 'on' or 'off' to turn the light on or off
 * On the Feaather it is the red one by the USB port
 */
void light(int argc, char* argv[], Debugger* dbg) {
  if(argc >= 2) {
    if(strcmp(argv[1], "off") == 0) {
      digitalWrite(LED_BUILTIN, LOW);
      DEBUG_SERIAL.println("Turning light off");
      return;
    } else if(strcmp(argv[1], "on") == 0) {
      digitalWrite(LED_BUILTIN, HIGH);
      DEBUG_SERIAL.println("Turning light on");
      return;
    }
  }
  DEBUG_SERIAL.println("I don't know what you want me to do with my light!");
}

/*
 * Gets the MAC address of the ESP32. Needed for registering
 * devices to some WiFi networks
 */
void mac_address(int argc, char* argv[], Debugger* dbg) {
  DEBUG_SERIAL.print("ESP Board MAC Address:  ");
  DEBUG_SERIAL.println(WiFi.macAddress());
}
