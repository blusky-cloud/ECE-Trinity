#ifndef EMAIL_H
#define EMAIL_H

#include <Arduino.h>
#include <WiFiClientSecure.h>

#include "../../src/debug/debugger.h"

#define MAX_EMAIL_SUBJECT_LENGTH 256
#define MAX_IMAP_COMMAND_LENGTH 64
#define MAX_IMAP_RESPONSE_LENGTH 1024

void emailCommand(int argc, char* argv[], Debugger* dbg);

class EmailClient {
  public:
    EmailClient(Debugger* dbg);
    void begin(void);
    void refresh(void);
    bool hasUnseen(void);
    void getLatestSubject(char* whereToPut);
    bool connect();
    bool isConnected();
    void issueCommand(char* tag, char* command);
    bool executeCommand(char* tag, char* command);
    void readLine(void);
    void readPrintLine(void);
    bool readToEnd(char* tag);
    void setPrintIMAPresponses(bool toPrint);
  private:
    WiFiClientSecure client;
    Debugger* dbg;
    bool printIMAPresponses = false;
    char response[MAX_IMAP_RESPONSE_LENGTH];
    bool hasUnread;
    char lastUnread[MAX_EMAIL_SUBJECT_LENGTH];
};

bool found(const char* last, const char* search, int lastlength, int searchlength);

#endif