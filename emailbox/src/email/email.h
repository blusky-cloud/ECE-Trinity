/* Header for the EmailClient object - email.h
 *
 * Creates an IMAP connection to a mail server (as specified in credentials.h)
 * and helps with a few basic uses, like getting the latest unread email.
 *
 * Author: Benjamin Crall 
 */

#ifndef EMAIL_H
#define EMAIL_H

#include <Arduino.h>
#include <WiFiClientSecure.h>

#include "../../src/debug/debugger.h"

// The below paramaters can be incresed, although this will require more memory.
// The maximum length of an email subject line and other included data
#define MAX_EMAIL_SUBJECT_LENGTH 256
// The maximum length of an IMAP command
#define MAX_IMAP_COMMAND_LENGTH 64
// The maximum length of an IMAP response
#define MAX_IMAP_RESPONSE_LENGTH 1024
// The line ending for the IMAP commands
#define END_IMAP_COMMAND "\r\n"

// Debug email command callback
void emailCommand(int argc, char* argv[], Debugger* dbg);

// The main Email Client object for email uses
class EmailClient {
  public:
    EmailClient(Debugger* dbg);
    void begin(void);                                 // Sets up the object. Should only be needed once
    bool isConnected(void);                               // Checks if the connection is still active
    bool connect(void);                                   // Connects to the IMAP server. Should be called anytime the server disconnects.
    void logout(void)                                 // Logs out of the IMAP server. You can later log in again with connect().
    void refresh(void);                               // Updates the latest unread subject variables
    bool hasUnseen(void);                             // Gets wether there are unseen messages in the inbox
    void getLatestSubject(char* whereToPut);          // Gets the subject line of the latest unseen message
    void issueCommand(char* tag, char* command);      // Issues an IMAP command. Does not wait for response
    bool executeCommand(char* tag, char* command);    // Issues an IMAP command and waits for the response
    void readLine(void);                              // Reads the next line of the IMAP response to the response variable
    void readPrintLine(void);                         // Prints the next line of the IMAP response
    bool readToEnd(char* tag);                        // Reads the rest of the IMAP response
    void setPrintIMAPresponses(bool toPrint);         // Sets wether to print the IMAP responses to debug
  private:
    WiFiClientSecure client;                          // The SSL connectoin object
    Debugger* dbg;                                    // A pointer to the system debugger object
    bool printIMAPresponses = false;                  // Wether or not to print the IMAP responses
    char response[MAX_IMAP_RESPONSE_LENGTH];          // The latest processed IMAP response
    bool hasUnread;                                   // If there are unread messages in the inbox
    char lastUnread[MAX_EMAIL_SUBJECT_LENGTH];        // The subject line of the latest unread message
    // Check if a string has been found. Used to find the end of the IMAP responses.
    bool found(const char* last, const char* search, int lastlength, int searchlength);
};

#endif