/* Email mailbox thingy
 *
 * This requires the 'SSLClient' library version 1.6.11 by Noah Koontz
 *
 * Author: Benjamin Crall
 */

// Include the debugger stuff
#include "src/debug/debugger.h"
#include "src/email/email.h"
#include "a_flag_functions_oled_servo.h"

// This is the debugger object itself
Debugger debug;
EmailClient email(&debug);

//prototypes
void notify(String text);
void raise_flag();
void lower_flag();
void reset_flag(int reset);
void scroll_banner(String text, int reps);

// No additional setup is required.
void setup() {

  setupPeripherals();
  
  // no email errors here please
  email.begin(); // This must be called here
  email.connect(); // This probably should be called here, although is not required

  Serial.println("Ready");

  // Everything else is optional, and is an example of email usage
  email.refresh();
  if(email.hasUnseen()) {
    char subj[MAX_EMAIL_SUBJECT_LENGTH];
    email.getLatestSubject(subj);
    char from[MAX_EMAIL_SUBJECT_LENGTH];
    email.getLatestFrom(from);
    Serial.printf("Latest Unread: %s: %s\n", from, subj);
  } else {
    Serial.println("No unread messages.");
  }
}

// Make sure to call debug.read() occasionally so that the debugger can do its thing
void loop() {
  delay(10);
  debug.read();
}