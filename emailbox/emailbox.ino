/* Email mailbox thingy
 *
 * This requires the 'ESP Mail Client' library version 1.5.6 by Mobzit
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
  email.updateEmails();
}

// Make sure to call debug.read() occasionally so that the debugger can do its thing
void loop() {
  delay(10);
  debug.read();
}