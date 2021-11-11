#include "./email.h"

#include <Arduino.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>

#include "../../credentials.h"
#include "../../src/debug/debugger.h"

EmailClient::EmailClient(Debugger* dbgr) {
  dbg = dbgr;
}

void emailCommand(int argc, char* argv[], Debugger* dbg) {
  if(argc < 2) {
    email.updateEmails();
  } else {
    if(!strcmp(argv[1], "status")) {
      Serial.println("Getting status ...");
      bool sts = imap.sendCustomCommand("A042 STATUS INBOX (UNSEEN)", &statusCommandCallback);
      Serial.printf("%s %s", resp, (sts) ? "yes" : "no");
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
      delay(200);
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

}

// void emailCommand(int argc, char* argv[], Debugger* dbg);

// class EmailClient {
//   public:
//     EmailClient(Debugger* dbg);
//     void begin(void);
//     void refresh(void);
//     bool hasEmails(void);
//     void getLatestSubject(char* whereToPut);
//   private:
//     static bool busy;
// };

void EmailClient::refresh(void) {

}

bool EmailClient::hasEmails(void) {

}

void EmailClient::getLatestSubject(char* whereToPut) {

}