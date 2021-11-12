/* Primary file for the mailbox code.
 * The debugger will open its serial interface at 115200 baud
 *
 * Author: Benjamin Crall
 */

// Include the debugger stuff
#include "debug/debugger.h"

// This is the debugger object itself
Debugger debug;

// No additional setup is required.
void setup() {

}

// Make sure to call debug.read() occasionally so that the debugger can do its thing
void loop() {
  delay(10);
  debug.read();
}