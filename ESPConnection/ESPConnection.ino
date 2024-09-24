#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>

#include "secrets.h"  // add WLAN Credentials in here.

#include <FS.h>        // File System for Web Server Files
#include <LittleFS.h>  // This file system is used.

#define UNUSED __attribute__((unused))

#define TRACE(...) Serial.printf(__VA_ARGS__)

// Name of the server. You reach it using http://webserver
#define HOSTNAME "webserver"

// Local time zone definition (Berlin)
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"

// WiFi Static IP Configuration
IPAddress local_IP(192, 168, 0, 2);  // Set your desired IP address here
IPAddress gateway(192, 168, 1, 1);     // Set your network gateway
IPAddress subnet(255, 255, 255, 0);    // Set your network subnet mask
IPAddress primaryDNS(8, 8, 8, 8);      // Optional: set your preferred DNS server
IPAddress secondaryDNS(8, 8, 4, 4);    // Optional: set your backup DNS server

WebServer server(80);
#include "builtinfiles.h"

#define CUSTOM_ETAG_CALC

void handleRedirect() {
  TRACE("Redirect...\n");
  String url = "/index.htm";
  if (!LittleFS.exists(url)) {
    url = "/$upload.htm";
  }
  server.sendHeader("Location", url, true);
  server.send(302);
}

void handleListFiles() {
  File dir = LittleFS.open("/", "r");
  String result;
  result += "[\n";
  while (File entry = dir.openNextFile()) {
    if (result.length() > 4) {
      result += ",\n";
    }
    result += "  {";
    result += "\"type\": \"file\", ";
    result += "\"name\": \"" + String(entry.name()) + "\", ";
    result += "\"size\": " + String(entry.size()) + ", ";
    result += "\"time\": " + String(entry.getLastWrite());
    result += "}";
  }
  result += "\n]";
  server.sendHeader("Cache-Control", "no-cache");
  server.send(200, "text/javascript; charset=utf-8", result);
}

void handleSysInfo() {
  String result;
  result += "{\n";
  result += "  \"Chip Model\": " + String(ESP.getChipModel()) + ",\n";
  result += "  \"Chip Cores\": " + String(ESP.getChipCores()) + ",\n";
  result += "  \"Chip Revision\": " + String(ESP.getChipRevision()) + ",\n";
  result += "  \"flashSize\": " + String(ESP.getFlashChipSize()) + ",\n";
  result += "  \"freeHeap\": " + String(ESP.getFreeHeap()) + ",\n";
  result += "  \"fsTotalBytes\": " + String(LittleFS.totalBytes()) + ",\n";
  result += "  \"fsUsedBytes\": " + String(LittleFS.usedBytes()) + ",\n";
  result += "}";
  server.sendHeader("Cache-Control", "no-cache");
  server.send(200, "text/javascript; charset=utf-8", result);
}

class FileServerHandler : public RequestHandler {
public:
  FileServerHandler() {
    TRACE("FileServerHandler is registered\n");
  }

  bool canHandle(HTTPMethod requestMethod, String UNUSED uri) override {
    return ((requestMethod == HTTP_POST) || (requestMethod == HTTP_DELETE));
  }

  bool canUpload(String uri) override {
    return (uri == "/");
  }

  bool handle(WebServer &server, HTTPMethod requestMethod, String requestUri) override {
    String fName = requestUri;
    if (!fName.startsWith("/")) {
      fName = "/" + fName;
    }

    TRACE("handle %s\n", fName.c_str());

    if (requestMethod == HTTP_POST) {
      // all done in upload. no other forms.
    } else if (requestMethod == HTTP_DELETE) {
      if (LittleFS.exists(fName)) {
        TRACE("DELETE %s\n", fName.c_str());
        LittleFS.remove(fName);
      }
    }
    server.send(200);
    return (true);
  }

  void upload(WebServer UNUSED &server, String UNUSED _requestUri, HTTPUpload &upload) override {
    static size_t uploadSize;
    if (upload.status == UPLOAD_FILE_START) {
      String fName = upload.filename;
      if (!fName.startsWith("/")) {
        fName = "/" + fName;
      }
      TRACE("start uploading file %s...\n", fName.c_str());
      if (LittleFS.exists(fName)) {
        LittleFS.remove(fName);
      }
      _fsUploadFile = LittleFS.open(fName, "w");
      uploadSize = 0;
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (_fsUploadFile) {
        size_t written = _fsUploadFile.write(upload.buf, upload.currentSize);
        if (written < upload.currentSize) {
          TRACE("  write error!\n");
          _fsUploadFile.close();
          String fName = upload.filename;
          if (!fName.startsWith("/")) {
            fName = "/" + fName;
          }
          LittleFS.remove(fName);
        }
        uploadSize += upload.currentSize;
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      TRACE("finished.\n");
      if (_fsUploadFile) {
        _fsUploadFile.close();
        TRACE(" %d bytes uploaded.\n", upload.totalSize);
      }
    }
  }

protected:
  File _fsUploadFile;
};

void setup(void) {
  delay(3000);
  Serial.begin(115200);
  Serial.setDebugOutput(false);

  TRACE("Starting WebServer example...\n");

  TRACE("Mounting the filesystem...\n");
  if (!LittleFS.begin()) {
    TRACE("could not mount the filesystem...\n");
    delay(2000);
    TRACE("formatting...\n");
    LittleFS.format();
    delay(2000);
    TRACE("restart.\n");
    delay(2000);
    ESP.restart();
  }

  WiFi.setHostname(HOSTNAME);

  // Configure static IP
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    TRACE("Static IP Configuration failed!\n");
  }

  WiFi.mode(WIFI_STA);
  if (strlen(ssid) == 0) {
    WiFi.begin();
  } else {
    WiFi.begin(ssid, passPhrase);
  }

  TRACE("Connect to WiFi...\n");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    TRACE(".");
  }
  TRACE("connected.\n");

  configTzTime(TIMEZONE, "pool.ntp.org");

  TRACE("Register redirect...\n");
  server.on("/", HTTP_GET, handleRedirect);

  TRACE("Register service handlers...\n");
  server.on("/$upload.htm", []() {
    server.send(200, "text/html", FPSTR(uploadContent));
  });

  server.on("/api/list", HTTP_GET, handleListFiles);
  server.on("/api/sysinfo", HTTP_GET, handleSysInfo);

  TRACE("Register file system handlers...\n");
  server.addHandler(new FileServerHandler());
  server.enableCORS(true);

#if defined(CUSTOM_ETAG_CALC)
  server.enableETag(true, [](FS &fs, const String &path) -> String {
    File f = fs.open(path, "r");
    String eTag = String(f.getLastWrite(), 16);
    f.close();
    return (eTag);
  });
#else
  server.enableETag(true);
#endif

  server.serveStatic("/", LittleFS, "/");

  TRACE("Register default (not found) answer...\n");
  server.onNotFound([]() {
    server.send(404, "text/html", FPSTR(notFoundContent));
  });

  server.begin();
  TRACE("open <http://%s> or <http://%s>\n", WiFi.getHostname(), WiFi.localIP().toString().c_str());
}

void loop(void) {
  server.handleClient();
}