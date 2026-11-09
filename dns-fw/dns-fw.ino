#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <lwip/napt.h>            // 用于开启 NAT 转发

// ================= 配置区 =================
// 1. 你家路由器的 WiFi 信息（ESP8266 要连这个）
const char* homeSSID = "CMCC-2uhz"; 
const char* homePASS = "11111111"; 

// 2. ESP8266 自己发出的热点信息（手机连这个）
const char* apSSID   = "MyDNS_AP";
const char* apPASS   = "12345678";

// 3. 劫持目标
const byte DNS_PORT = 53;
IPAddress hijackIP(38, 147, 187, 122);  // 所有域名都会被解析到这个 IP
// ==========================================

DNSServer dnsServer;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("\n\n--- ESP8266 DNS Hijack Booting ---");

  // 1. 开启 AP+STA 混合模式
  WiFi.mode(WIFI_AP_STA);
  
  // 2. 启动热点
  WiFi.softAP(apSSID, apPASS);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  // 3. 连接你家 WiFi
  WiFi.begin(homeSSID, homePASS);
  Serial.print("Connecting to home WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nSTA IP: " + WiFi.localIP().toString());

  // 4. 核心：劫持所有域名 -> hijackIP
  dnsServer.start(DNS_PORT, "*", hijackIP);
  Serial.println("DNS hijack -> " + hijackIP.toString());

  // 5. 开启 NAT 转发，让连热点的设备能访问公网
  ip_napt_enable(WiFi.localIP(), 1);
  Serial.println("NAPT enabled");

  Serial.println("--- System Ready! ---");
}

void loop() {
  // 必须持续处理 DNS 请求
  dnsServer.processNextRequest();
}
