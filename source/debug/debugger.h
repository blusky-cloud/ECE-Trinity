/* Header file debugger.h
 * 
 * This file contains all the headers for the Debugger object.
 * See debug.ino for instructions and example usage, as well as
 * the configuration #defines.
 *
 * Author: Benjamin Crall 
 */
 
#include <Arduino.h>

#ifndef DEBUGGER_H
#define DEBUGGER_H
/*  __   __        ___    __  
 * /  ` /  \ |\ | |__  | / _` 
 * \__, \__/ | \| |    | \__) 
 */                           

// The maximum number of characters in each debug command. Limited to reduce RAM usage. Default 64
#define DEBUG_MAX_COMMAND_LENGTH 64
// The maximum number of arguments in each debug command, including the command itself. Default 32
#define DEBUG_MAX_COMMAND_ARGS 32
// The Serial interface to use for debug commands. Default Serial
#define DEBUG_SERIAL Serial
// The baud rate for debug commands. Default 115200
#define DEBUG_SERIAL_BAUD 115200
// The amount of time in ms to wait after enabling serial. Comment line to disable compleatly. Default 100
#define DEBUG_SERIAL_DELAY 100
// The maximum number of allowed Debug Commands. Soft limit to not take up too much ram. Default 16
#define DEBUG_MAX_COMMANDS 16

/*
 * Structure to hold the debug commands.
 * Holds the name with a pointer to the function to call.
 */
typedef struct DebugCommand_s {
  char* name;
  void (*exec)(int, char**);  
} DebugCommand;

/*
 * The main debugger class. See debugger.cpp for detailed descriptions.
 */
class Debugger {
  public:
    Debugger(void);           // Constructor
    void read(void);          // Read and process commands
    void print(char* str);    // Prints to DEBUG_SERIAL
    void println(char* str);  // Prints to DEBUG_SERIAL with a newline
    void registerCommand(char* name, void(*command)(int, char**)); // Registers a new debug command
  private:
    char cmd_buff[DEBUG_MAX_COMMAND_LENGTH+1];  // Buffer to hold part of incomming command
    int cmdBuffPtr;                             // The current writehead in the buffer
    DebugCommand commands[DEBUG_MAX_COMMANDS];  // The array of possible commands
    int numDebugCommands = 0;                   // The number of possible commands
};

#endif