/* Header file defaultcmds.h
 * 
 * This file contains the prototypes for the default debugger commands.
 *
 * Author: Benjamin Crall 
 */

#ifndef DEFAULT_COMMANDS_H
#define DEFAULT_COMMANDS_H

void echo(int argc, char* argv[]);  // Print stuff back to the user
void lumos(int argc, char* argv[]); // Turn the light on
void nox(int argc, char* argv[]);   // Turn the light off
void light(int argc, char* argv[]); // Control the light

#endif