/* Basic example to of the debug command interface.
 * The debugger will open its serial interface at 115200 baud
 *
 * Author: Benjamin Crall
 */

// Include the debugger stuff
#include "src/debug/debugger.h"
#include "src/email/email.h"

// This is the debugger object itself
Debugger debug;
EmailClient ec(debug);

// No additional setup is required.
void setup() {
  ec.begin();
  ec.updateEmails();
}

// Make sure to call debug.read() occasionally so that the debugger can do its thing
void loop() {
  delay(10);
  debug.read();
}