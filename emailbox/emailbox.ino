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

#define CHECK_MAILBOX_DELAY 1000
unsigned long nextCheck = 0;
String prev_subj("N/A");
bool flag_is_raised = false;

// No additional setup is required.
void setup() {
  setupPeripherals();
  
  // no email errors here please
  email.begin(); // This must be called here
  email.connect(); // This probably should be called here, although is not required

  Serial.println("Ready");

  // Everything else is optional, and is an example of email usage
  /*
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
  */
  nextCheck = millis();
  //reset_flag(90);
  
}

// Make sure to call debug.read() occasionally so that the debugger can do its thing
void loop() {
  debug.read();

  if(nextCheck < millis()) {
    nextCheck += CHECK_MAILBOX_DELAY;
    email.refresh();
    if (email.hasUnseen()) {
      char subj[MAX_EMAIL_SUBJECT_LENGTH];
      email.getLatestSubject(subj);
      char from[MAX_EMAIL_SUBJECT_LENGTH];
      email.getLatestFrom(from);

      //raise the flag if it's not yet raised
      if (!flag_is_raised) {
        raise_flag();
        flag_is_raised = true;
      }

      // scroll latest unread subject
      String banner("New Msg From: ");
      banner.concat(from);
      banner.concat(", Subject: ");
      banner.concat(subj);
      scroll_banner(banner, 1);

      // check to make sure we aren't repeating a notification
      /*
      String test(subj);
      if (!prev_subj.equals(test)) {
          // copy char arrays to String class for notify argument
          String banner("New Msg From: ");
          banner.concat(from);
          banner.concat(", Subject: ");
          banner.concat(subj);

          notify(banner);
          prev_subj = subj;
      }
      */
    }
    else {
      if (flag_is_raised) {
        lower_flag();
        flag_is_raised = false;
      }
    }
  }
  delay(50);
}