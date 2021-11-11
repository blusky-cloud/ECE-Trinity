#ifndef EMAIL_H
#define EMAIL_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>

#include "../../src/debug/debugger.h"

#define MAX_EMAIL_SUBJECT_LENGTH 256

void emailCommand(int argc, char* argv[], Debugger* dbg);

class EmailClient {
  public:
    EmailClient(Debugger* dbg);
    void begin(void);
    void refresh(void);
    bool hasEmails(void);
    void getLatestSubject(char* whereToPut);
  private:
    static bool busy;
};

#endif