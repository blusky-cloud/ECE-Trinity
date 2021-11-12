#include "./email.h"

#include <Arduino.h>
#include <WiFiClientSecure.h>

#include "../../credentials.h"
#include "../../src/debug/debugger.h"
#include "../../imap.google.com.cert.h"
/*

When first connect:
  Login:
  <tag> LOGIN <email> <password>

  Select the inbox:
  <tag> SELECT INBOX

On Reload:
  Noop to update stuff:
  <tag> NOOP

  Find unread emails
  <tag> SEARCH UNSEEN

  Get email header
  <tab> FETCH <number> BODY.PEEK[HEADER.FIELDS (SUBJECT)]
*/

#define END_IMAP_COMMAND "\r\n"

// #define VERBOSE_DEBUG

EmailClient::EmailClient(Debugger* dbgr) {
  dbg = dbgr;
}

extern EmailClient email;

void emailCommand(int argc, char* argv[], Debugger* dbg) {
  if(argc < 2) {
    // email.updateEmails();
  } else {
    if(!strcmp(argv[1], "status")) {
      Serial.println("Getting status ...");
      // bool sts = imap.sendCustomCommand("A042 STATUS INBOX (UNSEEN)", &statusCommandCallback);
      // Serial.printf("%s %s", resp, (sts) ? "yes" : "no");
    // } else if(!strcmp(argv[1], "command")) {
      // if(argc < 4) {
      //   Serial.println("Not enough arguments.");
      // } else {
      //   char cmd[128];
      //   char* p = cmd;
      //   for(int i = 2; i < argc; i++) {
      //     char* c = argv[i];
      //     if(i != 0) {
      //       *p = 0x20;
      //       p++;           
      //     }
      //     while(*c >= 0x20) {
      //       *p = *c;
      //       p++; c++;
      //     }
      //   }
      //   *p = '\r'; p++;
      //   *p = '\n'; p++;
      //   *p = '\0';
      //   Serial.printf("C: > %s\n", cmd);

      //   const int max_result_len = 1024;
      //   char result[max_result_len];
      //   email.issueCommand(argv[2], cmd, result, max_result_len);

      //   Serial.println("Sending command ...");
      // }
    } else if(!strcmp(argv[1], "readline")) {
      Serial.println("Next line ---");
      email.readPrintLine();
      Serial.println("End line  ---");
    } else if(!strcmp(argv[1], "connect")) {
      Serial.println("Connecting ...");
      email.connect();
    } else if(!strcmp(argv[1], "refresh")) {
      Serial.println("Refreshing ...");
      email.refresh();
    } else {
      Serial.print("No can do da thingy "); 
      Serial.println(argv[1]); 
    }
  }
}

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

bool EmailClient::isConnected() {
  return client.connected();  
}

bool EmailClient::connect() {
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

bool EmailClient::executeCommand(char* tag, char* command) {
  issueCommand(tag, command);
  return readToEnd(tag);
}

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

void EmailClient::readPrintLine(void) {
  char c = 'q';
  while(c != '\n') {
    if(client.available()) {
      c = client.read();
      Serial.write(c);
    }
  }
}

bool found(const char* last, const char* search, int lastlength, int searchlength) {
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

void EmailClient::setPrintIMAPresponses(bool toPrint) {
  printIMAPresponses = toPrint;
}

// Tag MUST be 3 characters
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

bool EmailClient::hasUnseen(void) {
  return hasUnread;
}

void EmailClient::getLatestSubject(char* whereToPut) {
  strcpy(whereToPut, lastUnread);
}