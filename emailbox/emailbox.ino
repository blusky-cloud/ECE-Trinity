/* Email mailbox thingy
 *
 * This requires the 'SSLClient' library version 1.6.11 by Noah Koontz
 *
 * Author: Benjamin Crall
 */

// Include the debugger stuff
#include "src/debug/debugger.h"
#include "src/email/email.h"

// This is the debugger object itself
Debugger debug;
EmailClient email(&debug);

// No additional setup is required.
void setup() {
  // no email errors hree please
  email.begin();
  email.refresh();
}

// Make sure to call debug.read() occasionally so that the debugger can do its thing
void loop() {
  delay(10);
  debug.read();
}