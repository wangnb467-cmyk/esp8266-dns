#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <lwip/napt.h>

const char* homeSSID = "CMCC-2uhz";
const char* homePASS = "11111111";
const char* apSSID   = "MyDNS_AP";
const char* apPASS   = "12345678";

const byte DNS_PORT = 53;
IPAddress hijackIP(38, 147, 187, 122);  // 所有域名都解析到这里

DNSServer dnsServer;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);

  // 1. 开热点
  WiFi.softAP(apSSID, apPASS);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  // 2. 连家里WiFi
  WiFi.begin(homeSSID, homePASS);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nSTA IP: " + WiFi.localIP().toString());

  // 3. 核心：劫持所有域名 -> hijackIP
  dnsServer.start(DNS_PORT, "*", hijackIP);
  Serial.println("DNS hijack -> " + hijackIP.toString());

  // 4. 开NAT，让连热点的设备能真正访问到 38.147.187.122
  ip_napt_enable(WiFi.localIP(), 1);
  Serial.println("NAPT enabled");
}

void loop() {
  dnsServer.processNextRequest();
}
