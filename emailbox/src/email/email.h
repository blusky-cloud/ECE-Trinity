#ifndef EMAIL_H
#define EMAIL_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>

#include "../../src/debug/debugger.h"

#define MAX_EMAIL_SUBJECT_LENGTH 256

/* The imap host name e.g. imap.gmail.com for GMail or outlook.office365.com for Outlook */
#define IMAP_HOST "imap.gmail.com"

/** The imap port e.g. 
 * 143  or esp_mail_imap_port_143
 * 993 or esp_mail_imap_port_993
*/
#define IMAP_PORT esp_mail_imap_port_993

void emailCommand(int argc, char* argv[], Debugger* dbg);

class EmailClient {
  public:
    EmailClient(Debugger* dbg);
    void begin();
    void updateEmails(void);
    bool hasEmails(void);
    void getLatestSubject(char* whereToPut);
  private:
    static bool busy;
    Debugger* dbg;
};
/* Print the list of mailbox folders */
void printAllMailboxesInfo(IMAPSession &imap);

/* Print the selected folder info */
void printSelectedMailboxInfo(SelectedFolderInfo sFolder);

/* Print all messages from the message list */
void printMessages(std::vector<IMAP_MSG_Item> &msgItems, bool headerOnly);

/* Print all attachments info from the message */
void printAttacements(std::vector<IMAP_Attach_Item> &atts);

/* Callback function to get the Email reading status */
void imapCallback(IMAP_Status status);

#endif