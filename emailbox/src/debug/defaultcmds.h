/* Header file defaultcmds.h
 * 
 * This file contains the prototypes for the default debugger commands.
 *
 * Author: Benjamin Crall 
 */

#ifndef DEFAULT_COMMANDS_H
#define DEFAULT_COMMANDS_H

#include "./debugger.h"

void echo(int argc, char* argv[], Debugger* dbg);        // Print stuff back to the user
void lumos(int argc, char* argv[], Debugger* dbg);       // Turn the light on
void nox(int argc, char* argv[], Debugger* dbg);         // Turn the light off
void light(int argc, char* argv[], Debugger* dbg);       // Control the light
void mac_address(int argc, char* argv[], Debugger* dbg); // Get the MAC address of the device
void help(int argc, char* argv[], Debugger* dbg);

#endif
