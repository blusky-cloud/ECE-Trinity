#include <WiFiClientSecure.h>

/*
 * From credentials.h
 */

// The SSID of the WiFi AP you want to connect to
#define WIFI_SSID ""
// The password of the WiFi AP you want to connect to
#define WIFI_PASSWORD ""

// The email address to check
#define EMAIL_ADDRESS ""
// The password to your email account
#define EMAIL_PASSWORD ""
// The imap host name e.g. imap.gmail.com for GMail or outlook.office365.com for Outlook
#define IMAP_HOST ""
// The imap port e.g. 143 or 993
#define IMAP_PORT 993

/*
 * End credentials.h
 */

const char* ssid     = WIFI_SSID;     // your network SSID 
const char* password = WIFI_PASSWORD; // your network password

const char*  server = IMAP_HOST;  // Server URL

/* use 
openssl s_client -showcerts -connect www.howsmyssl.com:443 </dev/null 
to get this certificate */
const char* ca_cert = \
R"==(-----BEGIN CERTIFICATE-----
MIIFYjCCBEqgAwIBAgIQd70NbNs2+RrqIQ/E8FjTDTANBgkqhkiG9w0BAQsFADBX
MQswCQYDVQQGEwJCRTEZMBcGA1UEChMQR2xvYmFsU2lnbiBudi1zYTEQMA4GA1UE
CxMHUm9vdCBDQTEbMBkGA1UEAxMSR2xvYmFsU2lnbiBSb290IENBMB4XDTIwMDYx
OTAwMDA0MloXDTI4MDEyODAwMDA0MlowRzELMAkGA1UEBhMCVVMxIjAgBgNVBAoT
GUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBMTEMxFDASBgNVBAMTC0dUUyBSb290IFIx
MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAthECix7joXebO9y/lD63
ladAPKH9gvl9MgaCcfb2jH/76Nu8ai6Xl6OMS/kr9rH5zoQdsfnFl97vufKj6bwS
iV6nqlKr+CMny6SxnGPb15l+8Ape62im9MZaRw1NEDPjTrETo8gYbEvs/AmQ351k
KSUjB6G00j0uYODP0gmHu81I8E3CwnqIiru6z1kZ1q+PsAewnjHxgsHA3y6mbWwZ
DrXYfiYaRQM9sHmklCitD38m5agI/pboPGiUU+6DOogrFZYJsuB6jC511pzrp1Zk
j5ZPaK49l8KEj8C8QMALXL32h7M1bKwYUH+E4EzNktMg6TO8UpmvMrUpsyUqtEj5
cuHKZPfmghCN6J3Cioj6OGaK/GP5Afl4/Xtcd/p2h/rs37EOeZVXtL0m79YB0esW
CruOC7XFxYpVq9Os6pFLKcwZpDIlTirxZUTQAs6qzkm06p98g7BAe+dDq6dso499
iYH6TKX/1Y7DzkvgtdizjkXPdsDtQCv9Uw+wp9U7DbGKogPeMa3Md+pvez7W35Ei
Eua++tgy/BBjFFFy3l3WFpO9KWgz7zpm7AeKJt8T11dleCfeXkkUAKIAf5qoIbap
sZWwpbkNFhHax2xIPEDgfg1azVY80ZcFuctL7TlLnMQ/0lUTbiSw1nH69MG6zO0b
9f6BQdgAmD06yK56mDcYBZUCAwEAAaOCATgwggE0MA4GA1UdDwEB/wQEAwIBhjAP
BgNVHRMBAf8EBTADAQH/MB0GA1UdDgQWBBTkrysmcRorSCeFL1JmLO/wiRNxPjAf
BgNVHSMEGDAWgBRge2YaRQ2XyolQL30EzTSo//z9SzBgBggrBgEFBQcBAQRUMFIw
JQYIKwYBBQUHMAGGGWh0dHA6Ly9vY3NwLnBraS5nb29nL2dzcjEwKQYIKwYBBQUH
MAKGHWh0dHA6Ly9wa2kuZ29vZy9nc3IxL2dzcjEuY3J0MDIGA1UdHwQrMCkwJ6Al
oCOGIWh0dHA6Ly9jcmwucGtpLmdvb2cvZ3NyMS9nc3IxLmNybDA7BgNVHSAENDAy
MAgGBmeBDAECATAIBgZngQwBAgIwDQYLKwYBBAHWeQIFAwIwDQYLKwYBBAHWeQIF
AwMwDQYJKoZIhvcNAQELBQADggEBADSkHrEoo9C0dhemMXoh6dFSPsjbdBZBiLg9
NR3t5P+T4Vxfq7vqfM/b5A3Ri1fyJm9bvhdGaJQ3b2t6yMAYN/olUazsaL+yyEn9
WprKASOshIArAoyZl+tJaox118fessmXn1hIVw41oeQa1v1vg4Fv74zPl6/AhSrw
9U5pCZEt4Wi4wStz6dTZ/CLANx8LZh1J7QJVj2fhMtfTJr9w4z30Z209fOU0iOMy
+qduBmpvvYuR7hZL6Dupszfnw0Skfths18dG9ZKb59UhvmaSGZRVbNQpsg3BZlvi
d0lIKO2d1xozclOzgjXPYovJJIultzkMu34qQb9Sz/yilrbCgj8=
-----END CERTIFICATE-----)==";

/* create an instance of WiFiClientSecure */
WiFiClientSecure client;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  /* waiting for WiFi connect */
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(100);
  }

  Serial.print("Connected to ");
  Serial.println(ssid);

  /* set SSL/TLS certificate */
  client.setCACert(ca_cert);

  Serial.printf("Connect to server via port %d\n", IMAP_PORT);
  if (!client.connect(server, IMAP_PORT)){
      Serial.println("Connection failed!");
      for(;;) {}
  } else {
      Serial.println("Connected to server!");
  }
}

String msg = "";

void loop() {
  /* if data is available then receive and print to Terminal */
  while (client.available()) {
      char c = client.read();
      Serial.write(c);
  }

  while (Serial.available()) {
      char c = Serial.read();
      if(c == '\n') {
        client.print(msg);
        client.print("\r\n");
        Serial.printf("c: > %s\n", msg.c_str());
        msg = "";
      } else {
        msg += c;
      }
  }

  // client.println("Connection: close");

  /* if the server disconnected, stop the client */
  if (!client.connected()) {
      Serial.println();
      Serial.println("Server disconnected");
      client.stop();
  }
}

