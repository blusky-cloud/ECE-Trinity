/* Source code for the EmailClient object - email.cpp
 *
 * Creates an IMAP connection to a mail server (as specified in credentials.h)
 * and helps with a few basic uses, like getting the latest unread email.
 */

#include "./email.h"

#include <Arduino.h>
#include <WiFiClientSecure.h>

#include "../../credentials.h"
#include "../../src/debug/debugger.h"
#include "../../imap.google.com.cert.h"

/*
 * IMAP commands used here
 * 
 * When first connect:
 *   Login:
 *   <tag> LOGIN <email> <password>
 * 
 *   Select the inbox:
 *   <tag> SELECT INBOX
 * 
 * On Reload:
 *   Noop to update stuff:
 *   <tag> NOOP
 * 
 *   Find unread emails
 *   <tag> SEARCH UNSEEN
 * 
 *   Get email header
 *   <tab> FETCH <number> BODY.PEEK[HEADER.FIELDS (SUBJECT)]
*/

// Uncomment this line to print more verbose debug info
// #define VERBOSE_DEBUG

// Go look for the email object, it should be created in emailbox.ino
extern EmailClient email;

/*
 * Callback for the email command.
 *
 * Has options connect, refresh, logout, check, printIMAP, silentIMAP, help
 */
void emailCommand(int argc, char* argv[], Debugger* dbg) {
  if(argc < 2) {
    // email.updateEmails();
  } else {
    } else if(!strcmp(argv[1], "connect")) {
      Serial.println("Connecting ...");
      email.connect();
    } else if(!strcmp(argv[1], "refresh")) {
      Serial.println("Refreshing ...");
      email.refresh();
    } else if(!strcmp(argv[1], "logout")) {
      Serial.println("Logging out");
      email.logout();
    } else if(!strcmp(argv[1], "check")) {
      if(email.hasUnseen()) {
        char subj[MAX_EMAIL_SUBJECT_LENGTH];
        email.getLatestSubject(subj);
        Serial.printf("Latest Unread: %s\n", subj);
      } else {
        Serial.println("No unread messages.");
      }
    } else if(!strcmp(argv[1], "printIMAP")) {
      email.setPrintIMAPresponses(true);
    } else if(!strcmp(argv[1], "silentIMAP")) {
      email.setPrintIMAPresponses(false);
    } else if(!strcmp(argv[1], "help")) {
      Serial.println("email commands: connect, refresh, logout, check, printIMAP, silentIMAP, help");
    } else {
      Serial.print("No can do da thingy "); 
      Serial.println(argv[1]); 
    }
  }
}

/*
 *  Constructor for the Email Client
 */
EmailClient::EmailClient(Debugger* dbgr) {
  dbg = dbgr;
}

/*
 * Begins the EmailClient. Must be called first.
 * Seperated from constructor since some Arduino functions don't work before setup()
 *
 * Connects to the WiFi and sets up (but does not connect to)
 *    the TLS connection to the IMAP server. The WiFi credentials
 *    are stored in credentials.h
 */
void EmailClient::begin(void) {
  
  dbg->registerCommand("email", &emailCommand);

  Serial.print("Connecting to AP");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
      // Serial.print(WiFi.status());
      Serial.print('.');
      delay(250);
  }

  Serial.println("");
  Serial.printf("WiFi connected to %s\n", WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

/*
 * Checks if the connection is active. You may call connect() again
 *   if the connection isn't active to re-open it.
 */
bool EmailClient::isConnected(void) {
  return client.connected();  
}

/*
 * Connects to the IMAP server through TLS. The IMAP server credentials
 *    are stored in credentials.h. This can be called multiple times,
 *    and can be used to restore a dead connection.
 */
bool EmailClient::connect(void) {
  /* set SSL/TLS certificate */
  client.setCACert(ca_cert);

  Serial.printf("Connecting to %s via port %d\n", IMAP_HOST, IMAP_PORT);
  if (!client.connect(IMAP_HOST, IMAP_PORT)){
    Serial.println("Connection failed!");
    return false;
  } else {
    Serial.println("Connected to server!");
    readLine();
    Serial.println("Logging in ...");
    executeCommand("gah", "LOGIN " EMAIL_ADDRESS " " EMAIL_PASSWORD);
    executeCommand("ibx", "SELECT INBOX");
    return true;
  }
}

/*
 * Logs out of the IMAP server. You can log in again later with connect()
 */
void EmailClient::logout(void) {
  executeCommand("lgo", "LOGOUT");
  client.stop();
}

/*
 * Gets the latest emails from the server. Also updates the
 *    hasUnread and lastUnread fields.
 */
void EmailClient::refresh(void) {
  Serial.println("Reading messages ...");
  executeCommand("rfh", "NOOP");
  executeCommand("src", "SEARCH UNSEEN");
  char* p = response;

  while(*p++ != '\n' && *p != '\r') {}
  p--;
  int dts = 0;
  int id = 0;
  int mlt = 1;
  
  while(*p >= '0' && *p <= '9') {
    dts++;
    id += (*p - '0') * mlt;
    mlt *= 10;
    p--;
  }

  if(dts > 0) {
    hasUnread = true;
    // <tab> FETCH <number> 
    char cmd[MAX_IMAP_COMMAND_LENGTH];
    sprintf(cmd, "FETCH %d BODY.PEEK[HEADER.FIELDS (SUBJECT)]", id);
    executeCommand("fth", cmd);

    p = response;
    
    char tempsubj[MAX_EMAIL_SUBJECT_LENGTH];
    char* q = tempsubj;
    while(*p++ != '\n' && *p != '\r') {}
    while(*p++ == '\n' || *p == '\r') {}
    p += 9;
    while(*p != '\n' && *p != '\r') {
      *q = *p;
      q++; p++;
    }
    *q = '\0';
    sprintf(lastUnread, "%s", tempsubj);
  } else {
    hasUnread = false;
    lastUnread[0] = '\0';
  } 
}

/*
 * Gets wether there are unseen messages in the inbox
 */
bool EmailClient::hasUnseen(void) {
  return hasUnread;
}

/*
 * Gets the subject line of the latest unseen message.
 * Copies it to whereToPut
 */
void EmailClient::getLatestSubject(char* whereToPut) {
  strcpy(whereToPut, lastUnread);
}

/*
 * Issues a command. Returns as soon as the command has been issued.
 *
 * You must call readLine or readToEnd to get the response. If you don't,
 *    the next command executed may get the response from this one.
 * Also re-connects to the server if needed.
 * Prints to seril if printIMAPresponses is true
 *
 * tag must be a pointer c style string with 3 characters + a null.
 * command is a pointer to a c style string holding the rest of the command 
 */ 
void EmailClient::issueCommand(char* tag, char* command) {
  if(!isConnected()) {
    connect();
  }
  if(isConnected()) {
    char cmd[MAX_IMAP_COMMAND_LENGTH];
    sprintf(cmd, "%s %s" END_IMAP_COMMAND, tag, command);
    client.print(cmd);
    if(printIMAPresponses) {
      Serial.printf("C: > %s", cmd);
    }
  }
}

/*
 * Issues a command. Waits to return until the response has been recorded
 *
 * Returns true if the IMAP command returned "OK", false if it returne "BAD"
 * Stores the response to this object's response field.
 *    
 * Also re-connects to the server if needed.
 *
 * tag must be a pointer c style string with 3 characters + a null.
 * command is a pointer to a c style string holding the rest of the command 
 */ 
bool EmailClient::executeCommand(char* tag, char* command) {
  issueCommand(tag, command);
  return readToEnd(tag);
}

/*
 * Reads the next line of the IMAP response and stores it to this object's response field.
 * Prints to seril if printIMAPresponses is true
 */
void EmailClient::readLine(void) {
  char c = 'q';
  int savedChars = 0;

  while(c != '\n') {
    if(client.available()) {
      c = client.read();
    #ifdef VERBOSE_DEBUG
      Serial.write(c);
    #endif
      if(savedChars < MAX_IMAP_RESPONSE_LENGTH-1) {
        response[savedChars++] = c;
      }
    }
  }

  response[savedChars] = '\0';  

  if(printIMAPresponses) {
    Serial.print(response);
  }
}

/*
 * Reads the next line of the IMAP response and prints it to serial.
 */
void EmailClient::readPrintLine(void) {
  char c = 'q';
  while(c != '\n') {
    if(client.available()) {
      c = client.read();
      Serial.write(c);
    }
  }
}

/*
 * Reads the entire remaining IMAP response until tag and "OK" or "BAD" are found,
 *    then finishes that line. Stores result to this object's response field.
 *
 * tag must be a pointer c style string with 3 characters + a null.
 */ 
bool EmailClient::readToEnd(char* tag) {
  char findOK[7];
  strcpy(findOK, tag);
  strcat(findOK, " OK");

  char findBAD[8];
  strcpy(findBAD, tag);
  strcat(findBAD, " BAD");

  const int okLength = 6;
  const int badLength = 7;

  const int lastsize = 7;
  char lasttxt[lastsize+1];
  lasttxt[lastsize] = '\0';
#ifdef VERBOSE_DEBUG
  Serial.printf("{%s:%d\t\t%s:%d\t\t%d}\n", findOK, okLength, findBAD, badLength, lastsize);
#endif

  int linepos = 0;
  bool foundend = false;
  bool status = false;

  char c = 'q';

  int savedChars = 0;

  while(!foundend || c != '\n') {
    if(client.available()) {
      c = client.read();
#ifdef VERBOSE_DEBUG
      Serial.write(c);
#endif
      if(savedChars < MAX_IMAP_RESPONSE_LENGTH-1) {
        response[savedChars++] = c;
      }

      if(c == '\n' || c == '\r') {
        linepos = -1;
      } else {
        linepos++;
      }

      if(!foundend && linepos >= 0 && linepos <= lastsize+1) {
      
        for(int i = 0; i < lastsize-1; i++) {
          lasttxt[i] = lasttxt[i+1];
        }
        lasttxt[lastsize-1] = c;

        if(linepos == okLength || linepos+1 == okLength) {
          if(found(lasttxt, findOK, lastsize, okLength)) {
            foundend = true;
            status = true;
#ifdef VERBOSE_DEBUG
            Serial.print("($)");
#endif
          }
        }        
        if(!foundend && (linepos == badLength || linepos+1 == badLength)) {
          if(found(lasttxt, findBAD, lastsize, badLength)) {
            foundend = true;
#ifdef VERBOSE_DEBUG
            Serial.print("(!)");
#endif
          }
        }
      }
    }
  }

  response[savedChars] = '\0';  

  if(printIMAPresponses) {
    Serial.print(response);
  }

  return status;
}

/*
 * Sets wether or not to print the IMAP debugging info to serial
 */
void EmailClient::setPrintIMAPresponses(bool toPrint) {
  printIMAPresponses = toPrint;
}

/*
 * Private method used to determine if the end of the IMAP response has been found.
 */ 
bool EmailClient::found(const char* last, const char* search, int lastlength, int searchlength) {
#ifdef VERBOSE_DEBUG
  Serial.printf("[%s:%d\t\t%s:%d]", last, strlen(last), search, searchlength);
#endif

  if(searchlength <= lastlength) {
    int laststart = lastlength - searchlength;
    int count = 0;

    for(int i = 0; i < searchlength; i++) {
      if(last[laststart + i] != search[i]) {
        return false;
      }
    }

    return true;
  
  } else {
    return false;
  }
}