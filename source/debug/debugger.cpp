/* Source code file debugger.cpp
 * 
 * This file contains all the main code for the Debugger object.
 * See debug.ino for instructions and example usage.
 *
 * Debug configuration can be found in debugger.h
 *
 * Author: Benjamin Crall 
 */

#include <Arduino.h>

#include "debugger.h"
#include "defaultcmds.h"

/*
 * Initializes the debugger.
 * Sets up the selected serial port at DEBUG_SERIAL_BAUD
 * Also sets up a few default commands that may be useful
 */
Debugger::Debugger(void) {
  DEBUG_SERIAL.begin(DEBUG_SERIAL_BAUD);
  
#ifdef DEBUG_SERIAL_DELAY
  // Wait for serail to connect if requested
  delay(DEBUG_SERIAL_DELAY);
#endif

  // Make sure the command buffer is clear
  memset(cmd_buff,'\0',DEBUG_MAX_COMMAND_LENGTH+1);

  // Load the default set of commands
  pinMode(LED_BUILTIN, OUTPUT);
  debug.registerCommand("echo", &echo);
  debug.registerCommand("light", &light);
  debug.registerCommand("lumos", &lumos);
  debug.registerCommand("nox", &nox);
  
  // Let's go!
  DEBUG_SERIAL.println("Debugger running");
}

/* 
 * Reads the serial port and processes the commands.
 * Needs to be called occasionally and may take some time to run.
 * Possibly should be called from the main thread to make sure
 *    everything is thread safe.
 */
void Debugger::read(void) {
  // Read everything from the serial buffer
  while(DEBUG_SERIAL.available()) {
    // Copy data from the hardware serial buffer to its own buffer
    char c = DEBUG_SERIAL.read();
    bool process = false;
    if(c >= ' ') {
      cmd_buff[cmdBuffPtr] = c;
      cmdBuffPtr++;
    } else {
      process = true;
    }
    
    // Don't let the local buffer overfill. Throw a warning if it's about to, try to go ahead anyway
    if(cmdBuffPtr > DEBUG_MAX_COMMAND_LENGTH) {
      process = true;
      DEBUG_SERIAL.println("WARNING: Command buffer was about to overflow, so the data was split.");
    }

    // Execute the command
    if(process) {
      // Prep some stuff
      cmd_buff[cmdBuffPtr] = '\0';
      cmdBuffPtr = 0;
      char* parts[DEBUG_MAX_COMMAND_ARGS];
      char* here = cmd_buff;
      int num = 0;
      parts[num++] = here++;
      // Split the command for easier processing in the handlers
      while(*here != '\0' && num < DEBUG_MAX_COMMAND_ARGS) {
        // Look for the space between args or any control character to split at
        if(*here <= ' ') {
          *here = '\0';
          parts[num++] = ++here;
        }
        here++;
      }

      // Find and execute the command
      for(int i = 0; i < numDebugCommands; i++) {
        DebugCommand dc = commands[i];
        if(strcmp(dc.name, parts[0]) == 0) {
          dc.exec(num, parts);
          return; // Leave. There may be some stuff left in the hardware buffer, but that's fine. We'll get to it next time
        }
      }
      // If it gets here, it didn't find the command
      DEBUG_SERIAL.println("I'm sorry, I'm afraid I can't do that.");
    }
  }
}

/*
 * Prints a string to the default debug interface.
 * You must call DEBUG_SERIAL.print(arg) if you want to print something that's not a string
 */
void Debugger::print(char* str) {
  DEBUG_SERIAL.print(str);
}

/*
 * Prints a string to the default debug interface, ending with a newline.
 * You must call DEBUG_SERIAL.print(arg) if you want to print something that's not a string
 */
void Debugger::println(char* str) {
  DEBUG_SERIAL.println(str);
}

/*
 * Registers a command to possibly be run.
 * Takes the name of the command, and a function to run when the command is called.
 * Function must take 2 arguments, the number of arguments in the command (including the command itself)
 *    and the array of strings that hold the arguments (arg[0] is the command itself,
 *    which allows for multiple commands to use the same handler function if you want)
 * See defaultcmds.cpp for example implementations.
 */
void Debugger::registerCommand(char* name, void(*exec)(int, char**)) {
  if(numDebugCommands < DEBUG_MAX_COMMANDS) {
    DebugCommand dc;
    dc.name = name;
    dc.exec = exec;
    commands[numDebugCommands++] = dc;
  } else {
    DEBUG_SERIAL.println("I'm sorry, I'm afraid I can't do that. (debug command storage full, change the size in debugger.h)");
  }
}