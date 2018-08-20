/*
     __  __     ______     ______     __  __
    /\ \/ /    /\  == \   /\  __ \   /\_\_\_\
    \ \  _"-.  \ \  __<   \ \ \/\ \  \/_/\_\/_
     \ \_\ \_\  \ \_____\  \ \_____\   /\_\/\_\
       \/_/\/_/   \/_____/   \/_____/   \/_/\/_/

  The MIT License

  Copyright (c) 2018 Thomas Sarlandie thomas@sarlandie.net

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <KBoxLogging.h>
#include <KBoxLoggerStream.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <elapsedMillis.h>
#include <ArduinoJson.h>
#include <Seasmart.h>
#include <esp_partition.h>
#include <SPIFFSEditor.h>
#include <SPIFFS.h>
#include <SD_MMC.h>
#include <Update.h>
#include <ESPmDNS.h>
#include <driver/sdmmc_host.h>
#include <driver/rtc_io.h>
#include <esp_vfs_fat.h>
#include <sdmmc_cmd.h>
#include <Adafruit_GFX.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>

#include "NMEA2000Task.h"
#include "SDLogging.h"
#include "NMEAServer.h"
#include "NMEASerialTask.h"
#include "GraphicsBenchmark.h"
#include "IOExpander.h"

//#define CAN32MODULE
//#define WROVERKIT
//#define M5STACK

//#define ALDISONE_v10
#define ALDISONE_v11
#define ALDISONE

#ifdef CAN32MODULE
#define USE_SDSPI
#include <SD.h>
#define SDCARD_CS_PIN   ((gpio_num_t) 17)
#define USE_NMEA2000
static const uint8_t serial1_tx = 14;
static const uint8_t serial1_rx = 39;
static const uint8_t serial2_tx = 27;
static const uint8_t serial2_rx = 36;
static const gpio_num_t n2k_rx = (gpio_num_t)4;
static const gpio_num_t n2k_tx = (gpio_num_t)5;
#define GPIO_CAN_DISABLE ((gpio_num_t)16)

#elif defined(WROVERKIT)
//#define USE_SDMMC
#define USE_SCREEN

#include <SD_MMC.h>
#include <WROVER_KIT_LCD.h>

static const gpio_num_t n2k_rx = (gpio_num_t) 35;
static const gpio_num_t n2k_tx = (gpio_num_t) 5;

#elif defined(M5STACK)
#include <WROVER_KIT_LCD.h>
#define USE_SCREEN

#elif defined(ALDISONE)
#define USE_SDMMC
#define USE_BMP280
#define USE_SERIAL
#define USE_NMEA2000
#include <SD_MMC.h>

static const uint8_t serial1_tx = 32;
static const uint8_t serial1_rx = 36;
static const uint8_t serial2_tx = 33;
static const uint8_t serial2_rx = 39;
static const gpio_num_t n2k_rx = (gpio_num_t)35;
static const gpio_num_t n2k_tx = (gpio_num_t)5;

#if defined(ALDISONE_v10)
static const int exp_n2k_en = 0;
static const int exp_nmeatx_en = 2;
#elif defined(ALDISONE_v11)
static const int exp_n2k_en = 0;
static const int exp_nmeatx_en = 1;
#endif

#endif

#ifdef USE_NMEA2000
#include <ESP32_CAN_def.h>
#include <N2kMessages.h>

#endif

AsyncWebServer webServer(80);
AsyncWebSocket ws("/signalk/v1/stream");
NMEA2000Task nmea2000Task;

FS *nullFS = new fs::FS(nullptr);
SDLogging sdLoggingTask(*nullFS);
NMEAServer nmeaServer;

#ifdef USE_BMP280
Adafruit_BMP280 bmp280;
#endif

#ifdef ALDISONE
IOExpander ioExpander(0x20);
#endif

#ifdef USE_SERIAL
NMEASerialTask serialTask1(1, serial1_tx, serial1_rx, 38400);
NMEASerialTask serialTask2(2, serial2_tx, serial2_rx, 38400);
#endif

#ifdef USE_SCREEN
WROVER_KIT_LCD tft;
#endif

static bool restartRequired = false;

static int s_countClients = 0;
static const char *s_vesselURN = "myVesselURN";

static uint8_t *ramForUpdate = 0;
static size_t ramUpdateSize = 32 * 1024;
static size_t updateLen = 0;

bool initializeCard() {
#ifdef USE_SDMMC
  /*
  // This might be needed to go into 1line mode. not sure ...
  // Should try speed in 4 line mode. Assuming they are connected on WROVER-KIT module - CONFIRMED!
  sdmmc_host_t host = SDMMC_HOST_DEFAULT();

  // To use 1-line SD mode, uncomment the following line:
  host.flags = SDMMC_HOST_FLAG_1BIT;
  host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;

  // This initializes the slot without card detect (CD) and write protect (WP) signals.
  // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

  // Options for mounting the filesystem.
  // If format_if_mount_failed is set to true, SD card will be partitioned and formatted
  // in case when mounting fails.
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
    .format_if_mount_failed = false,
    .max_files = 5
  };

  {
    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc_mount is an all-in-one convenience function.
    // Please check its source code and implement error recovery when developing
    // production applications.
    sdmmc_card_t* card;
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK) {
      if (ret == ESP_FAIL) {
        ESP_LOGE(TAG, "Failed to mount filesystem. If you want the card to be formatted, set format_if_mount_failed = true.");
      } else {
        ESP_LOGE(TAG, "Failed to initialize the card (%d). Make sure SD card lines have pull-up resistors in place.", ret);
      }
    }
    else {
      DEBUG("SDCard mounted");
      // Card has been initialized, print its properties
      sdmmc_card_print_info(stdout, card);
    }
  }
*/
  if(!SD_MMC.begin()){
    DEBUG("Card Mount failed");
  }
  else {
    DEBUG("Card mount successful!");
  }
  uint8_t cardType = SD_MMC.cardType();
  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  sdLoggingTask.setFS(SD_MMC);
#elif defined(USE_SDSPI)
  if(!SD.begin(SDCARD_CS_PIN)){
    DEBUG("Card Mount failed");
  }
  uint8_t cardType = SD.cardType();
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  sdLoggingTask.setFS(SD);
#else
  DEBUG("Not using any card.");
  uint8_t cardType = CARD_NONE;
  uint64_t cardSize = 0;
  sdLoggingTask.setFS(nullFS);
#endif

  if(cardType == CARD_NONE){
    DEBUG("No SD Card attached");
    return false;
  }

  const char* card;
  if(cardType == CARD_MMC){
    card = "MMC";
  } else if(cardType == CARD_SD){
    card = "SDSC";
  } else if(cardType == CARD_SDHC){
    card = "SDHC";
  } else {
    card = "Unknown";
  }
  DEBUG("Card type: %s", card);
  DEBUG("SD Card Size: %lluMB\n", cardSize);
  return true;
}

// FIXME: We are going to have some threading problems here because the onEvent
// will be called on a network thread and KBoxLogging is not thread-safe
void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type,
             void * arg, uint8_t *data, size_t len){
  if (type == WS_EVT_CONNECT) {
    DEBUG("%u: New connection from %s to %s", client->id(), client->remoteIP().toString().c_str(), server->url() );
    s_countClients++;

    StaticJsonBuffer<500> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["version"] = "1.0.2";
    //root["timestamp"] = "";
    root["self"] =  s_vesselURN;

    char buffer[256];
    root.printTo(buffer, sizeof(buffer));

    client->printf(buffer);
  }
  else if (type == WS_EVT_DISCONNECT) {
    DEBUG("%u: Disconnected", client->id());
    s_countClients--;
  }
  else if (type == WS_EVT_ERROR) {
    //  os_printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
    DEBUG("[%s][%u]: Error", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
    s_countClients--;
  }
  else if (type == WS_EVT_PONG) {
    DEBUG("%u: PONG", client->id());
  }
  else if (type == WS_EVT_DATA) {
    DEBUG("%u: DATA", client->id());
  }
  else {
    DEBUG("%u: Un-handled event with type=%i", client->id(), type);
  }
}

class WebSocketSender : public NMEA2000WriterInterface {
  public:
    bool write(const tN2kMsg &msg) override {
      char buf[400];
      if (N2kToSeasmart(msg, xTaskGetTickCount(), buf, sizeof(buf)) == 0) {
        ERROR("n2k conversion error");
        return false;
      }
      else {
        ws.textAll(buf);
        return true;
      }
    }
};

void kboxMDNSSetup() {
  MDNS.begin("kbox32");
  MDNS.addService("http","tcp",80);

  const char *services[] = { "signalk-http", "signalk-ws" };
  for (auto service : services) {
    MDNS.addService(service,"tcp",80);
    MDNS.addServiceTxt(service, "tcp", "txtvers", "1");
    MDNS.addServiceTxt(service, "tcp", "roles", "master,main");
    MDNS.addServiceTxt(service, "tcp", "self", "urn:mrn:signalk:uuid:52586A48-E2BE-41B6-8F3E-AEA344245593");
    MDNS.addServiceTxt(service, "tcp", "swname", "kbox32");
    MDNS.addServiceTxt(service, "tcp", "swvers", "0.0.1");
  }
}

void printPartitionInfo() {
  size_t ul;
  esp_partition_iterator_t _mypartiterator;
  const esp_partition_t *_mypart;
  ul = spi_flash_get_chip_size(); Serial.print("Flash chip size: "); Serial.println(ul);
  Serial.println("Partiton table:");
  _mypartiterator = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
  if (_mypartiterator) {
    do {
      _mypart = esp_partition_get(_mypartiterator);
      printf("%x - %x - %x - %x - %s - %i\n", _mypart->type, _mypart->subtype, _mypart->address, _mypart->size, _mypart->label, _mypart->encrypted);
    } while ((_mypartiterator = esp_partition_next(_mypartiterator)));
  }
  esp_partition_iterator_release(_mypartiterator);
  _mypartiterator = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);
  if (_mypartiterator) {
    do {
      _mypart = esp_partition_get(_mypartiterator);
      printf("%x - %x - %x - %x - %s - %i\n", _mypart->type, _mypart->subtype, _mypart->address, _mypart->size, _mypart->label, _mypart->encrypted);
    } while ((_mypartiterator = esp_partition_next(_mypartiterator)));
  }
  esp_partition_iterator_release(_mypartiterator);
}

#ifdef ALDISONE_v10
static volatile bool needToReadExpander = true;
void IRAM_ATTR handleExpanderInterrupt() {
  ESP_LOGE("expander", "Expander interrupt triggered.");
  needToReadExpander = true;
}
#endif
#ifdef ALDISONE_v11
static volatile bool sdcardInterrupt = true;
void IRAM_ATTR handleSdcardInterrupt() {
  ESP_LOGE("sdcard", "SDCard interrupt triggered.");
  sdcardInterrupt = true;
}

void IRAM_ATTR handleUserButtonInterrupt() {
  ESP_LOGE("user", "User button interrupt triggered.");
}

#endif

void setup() {
  Serial.begin(115200);

  Wire.begin(SDA, SCL, 800*1000);

  KBoxLogging.setLogger(new KBoxLoggerStream(Serial));

  DEBUG("KBox Starting");

  WiFi.onEvent([](WiFiEvent_t event){
    DEBUG("WiFi Ready");
  }, WiFiEvent_t::SYSTEM_EVENT_WIFI_READY);

  WiFi.onEvent([](WiFiEvent_t event){
      DEBUG("WiFi Station Start");
  }, WiFiEvent_t::SYSTEM_EVENT_STA_START);

  WiFi.onEvent([](WiFiEvent_t event){
      DEBUG("WiFi Station Connected");
  }, WiFiEvent_t::SYSTEM_EVENT_STA_CONNECTED);

  WiFi.onEvent([](WiFiEvent_t event){
      DEBUG("WiFi Station DisConnected");
  }, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);

  WiFi.onEvent([](WiFiEvent_t event){
      DEBUG("WiFi Station Got IP %s", WiFi.localIP().toString().c_str());
      kboxMDNSSetup();
  }, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);

  WiFi.begin("SFR-a478", "DESE4HPUH1J6");


  webServer.onNotFound([](AsyncWebServerRequest *request) {
      DEBUG("404: %s", request->url().c_str());
      request->send(404, "text/plain", "No bounty for you here sailor. Keep looking. (404)");
  });
  // attach AsyncWebSocket
  ws.onEvent(onEvent);

  webServer.on("/update", HTTP_POST, (ArRequestHandlerFunction)[](AsyncWebServerRequest *request){
      // the request handler is triggered after the upload has finished...
      // create the response, add header, and send response
      AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", (Update.hasError())?"FAIL":"OK");
      response->addHeader("Connection", "close");
      response->addHeader("Access-Control-Allow-Origin", "*");
      restartRequired = true;  // Tell the main loop to restart the ESP
      request->send(response);
  },(ArUploadHandlerFunction)[](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
      //Upload handler chunks in data
      if(!index){ // if index == 0 then this is the first frame of data
        DEBUG("Starting upload of %s", filename.c_str());
        if(!Update.begin()){//start with max available size
          DEBUG("Unable to start update %d", Update.getError());
          Update.printError(Serial);

          AsyncResponseStream *response = request->beginResponseStream("text/plain");
          response->setCode(500);
          response->printf("Failed to start upload: %d", Update.getError());
          request->send(response);
        }
        else {
          if (ramForUpdate == 0) {
            // Reserve lots of RAM for OTA updates - because why not - we are not using it now anyway.
            ramForUpdate = static_cast<uint8_t*>(malloc(ramUpdateSize));
          }
          if (!ramForUpdate) {
            DEBUG("malloc for ramForUpdate failed");
            request->send(500, "text/plain", "Malloc failed - where is the RAM?");
          }
          updateLen = 0;
        }
      }

      // Write when buffer is full - or when we are done.
      if (updateLen + len >= ramUpdateSize ) {
        elapsedMicros updateWriteTimer = 0;
        if (Update.write(ramForUpdate, updateLen) != updateLen) {
          DEBUG("Failed writing the update: %d", Update.getError());
          AsyncResponseStream *response = request->beginResponseStream("text/plain");
          response->setCode(500);
          response->printf("Failed to start upload: %d", Update.getError());
          request->send(response);
        }
        else {
          DEBUG("Wrote %i bytes of update in %.2f ms", updateLen, updateWriteTimer/1000);
          updateLen = 0;
        }
      }

      if (ramForUpdate != 0 &&  updateLen + len < ramUpdateSize) {
        memcpy(ramForUpdate + updateLen, data, len);
        updateLen += len;
      }
      else {
        DEBUG("update out of memory");
        request->send(500, "text/plain", "seems we are out of memory - this should not happen");
      }

      if(final){ // if the final flag is set then this is the last frame of data
        // Write anything leftover in buffer.
        if (Update.write(ramForUpdate, updateLen) != updateLen) {
          DEBUG("Failed writing the update: %d", Update.getError());
          AsyncResponseStream *response = request->beginResponseStream("text/plain");
          response->setCode(500);
          response->printf("Failed to start upload: %d", Update.getError());
          request->send(response);
        }
        else {
          free(ramForUpdate);
          ramForUpdate = 0;

          if(Update.end(true)){ //true to set the size to the current progress
            DEBUG("Update success: %u B", index + len);
          } else {
            DEBUG("Update error %d", Update.getError());
            Update.printError(Serial);

            AsyncResponseStream *response = request->beginResponseStream("text/plain");
            response->setCode(500);
            response->printf("Failed to start upload: %d", Update.getError());
            request->send(response);
          }
        }
      }
  });

#if 0
  // OTA
  webServer.on("/update-old", HTTP_POST, [](AsyncWebServerRequest *request){
      // the request handler is triggered after the upload has finished...
      // create the response, add header, and send response
      AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", (Update.hasError())?"FAIL":"OK");
      response->addHeader("Connection", "close");
      response->addHeader("Access-Control-Allow-Origin", "*");
      restartRequired = true;  // Tell the main loop to restart the ESP
      request->send(response);
  },[](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
      //Upload handler chunks in data
      if(!index){ // if index == 0 then this is the first frame of data
        DEBUG("Starting upload of %s", filename.c_str());
        if(!Update.begin()){//start with max available size
          DEBUG("Unable to start update %d", Update.getError());
          Update.printError(Serial);

          AsyncResponseStream *response = request->beginResponseStream("text/plain");
          response->setCode(500);
          response->printf("Failed to start upload: %d", Update.getError());
          request->send(response);
        }
      }

      //Write chunked data to the free sketch space
      if(Update.write(data, len) != len){
        DEBUG("Error writing update data %d", Update.getError());
        Update.printError(Serial);

        AsyncResponseStream *response = request->beginResponseStream("text/plain");
        response->setCode(500);
        response->printf("Failed to start upload: %d", Update.getError());
        request->send(response);
      }
      else {
        DEBUG("Wrote %i bytes of update", len);
      }

      if(final){ // if the final flag is set then this is the last frame of data
        if(Update.end(true)){ //true to set the size to the current progress
          DEBUG("Update success: %u B", index + len);
        } else {
          DEBUG("Update error %d", Update.getError());
          Update.printError(Serial);

          AsyncResponseStream *response = request->beginResponseStream("text/plain");
          response->setCode(500);
          response->printf("Failed to start upload: %d", Update.getError());
          request->send(response);
        }
      }
  });
#endif

  webServer.addHandler(&ws);

  webServer.on("/reboot", HTTP_POST, (ArRequestHandlerFunction) [](AsyncWebServerRequest *request) {
    restartRequired = true;
    request->send(200, "text/plain", "be right back!");
  });

  webServer.on("/test-sdcard-speed-spiram", HTTP_GET, (ArRequestHandlerFunction) [](AsyncWebServerRequest *request) {
    static const size_t dumpSize = 512*1024;
    size_t written = 0;

    uint8_t *pointer = static_cast<uint8_t*>(ps_malloc(dumpSize));

    if (!pointer) {
      request->send(500, "text/plain", "malloc failed");
    }
    DEBUG("Dumping memory from pointer: %p", pointer);

    elapsedMicros writeTimer = 0;

#ifdef USE_SDMMC
    File file = SD_MMC.open("/randomram.dump", FILE_WRITE);
#elif USE_SDSPI
    File file = SD.open("/randomram.dump", FILE_WRITE);
#else
    File file = File();
#endif

    while (written < 10 * 1024 * 1024 && writeTimer < 1e6) {
      written += file.write(pointer, dumpSize);
    }

    file.close();

    uint64_t timeSpent = writeTimer;

    DEBUG("Wrote %i bytes in %.2f ms - %.2f kB/s", written, timeSpent/1e3, written * 1e6/ timeSpent / 1024);

    AsyncResponseStream *response = request->beginResponseStream("text/plain");
    response->setCode(200);
    response->printf("Wrote %i bytes in %.2f ms - %.2f kB/s", written, timeSpent/1e3, written * 1e6/ timeSpent /
                                                                                       1024);
    request->send(response);

    free(pointer);
  });

  webServer.on("/test-sdcard-speed-sram", HTTP_GET, (ArRequestHandlerFunction) [](AsyncWebServerRequest *request) {
      static const size_t dumpSize = 128*1024;
      size_t written = 0;

      // SRAM1
      uint8_t *pointer = (uint8_t*)0x3FFE0000;

      DEBUG("Dumping memory from: %p", pointer);

      elapsedMicros writeTimer = 0;

#ifdef USE_SDMMC
      File file = SD_MMC.open("/randomram.dump", FILE_WRITE);
#elif USE_SDSPI
      File file = SD.open("/randomram.dump", FILE_WRITE);
#else
      File file = File();
#endif

      while (written < 10 * 1024 * 1024 && writeTimer < 1e6) {
        written += file.write(pointer, dumpSize);
      }

      file.close();

      uint64_t timeSpent = writeTimer;

      DEBUG("Wrote %i bytes in %.2f ms - %.2f kB/s", written, timeSpent/1e3, written * 1e6/ timeSpent / 1024);

      AsyncResponseStream *response = request->beginResponseStream("text/plain");
      response->setCode(200);
      response->printf("Wrote %i bytes in %.2f ms - %.2f kB/s", written, timeSpent/1e3, written * 1e6/ timeSpent /
                                                                                        1024);
      request->send(response);
  });

  webServer.on("/status", HTTP_GET, (ArRequestHandlerFunction) [](AsyncWebServerRequest *request) {
      DynamicJsonBuffer jsonBuffer;
      JsonObject &root = jsonBuffer.createObject();
      JsonObject &system = root.createNestedObject("system");
      system["heap"] = ESP.getFreeHeap();
#ifdef ALDISONE
      system["ioexpander-outputs"] = ioExpander.readRegister(1);
      system["ioexpander-inputs"] = ioExpander.readInput();
#endif
      system["cpufreq"] = ESP.getCpuFreqMHz();
      system["uptime"] = (uint32_t) (esp_timer_get_time() / 1000000);
      JsonObject &environment = root.createNestedObject("environment");

#ifdef USE_BMP280
      environment["temperature"] = bmp280.readTemperature();
      environment["pressure"] = bmp280.readPressure();
#endif

#ifdef USE_NMEA2000
      JsonObject &can = root.createNestedObject("canbus");
      can["MOD"] = MODULE_CAN->MOD.U;
      can["RMC"] = MODULE_CAN->RMC.U;
      can["SR"] = MODULE_CAN->SR.U;
      can["RXERR"] = MODULE_CAN->RXERR.U;
      can["TXERR"] = MODULE_CAN->TXERR.U;
#endif

      JsonObject &pins = root.createNestedObject("pins");
      pins["svp"] = digitalRead(36);
      pins["svn"] = digitalRead(39);
      pins["rtc-svp"] = rtc_gpio_get_level(GPIO_NUM_36);
      pins["rtc-svn"] = rtc_gpio_get_level(GPIO_NUM_36);

      AsyncResponseStream *response = request->beginResponseStream("text/json");
      root.printTo(*response);
      request->send(response);
  });

#ifdef ALDISONE
  webServer.on("/enabler", HTTP_GET, (ArRequestHandlerFunction) [](AsyncWebServerRequest *request) {
#ifdef ALDISONE_v10
    if (request->hasParam("nmearx")) {
      if (request->getParam("nmearx")->value() == "off") {
        // disabled when high.
        DEBUG("disabling nmearx");
        ioExpander.setOutputPin(1, true);
      }
      else {
        DEBUG("enabling nmearx");
        ioExpander.setOutputPin(1, false);
      }
    }
#endif
    if (request->hasParam("nmeatx")) {
      if (request->getParam("nmeatx")->value() == "off") {
        // disabled when high.
        DEBUG("disabling nmeatx");
        ioExpander.setOutputPin(exp_nmeatx_en, true);
      }
      else {
        DEBUG("enabling nmeatx");
        ioExpander.setOutputPin(exp_nmeatx_en, false);
      }
    }
    if (request->hasParam("n2k")) {
      if (request->getParam("n2k")->value() == "off") {
        DEBUG("disabling n2k");
        ioExpander.setOutputPin(exp_n2k_en, true);
      }
      else {
        DEBUG("enabling n2k");
        DEBUG("expander output: %x", ioExpander.readRegister(1));
        ioExpander.setOutputPin(exp_n2k_en, false);
        DEBUG("expander output: %x", ioExpander.readRegister(1));
      }
    }
    if (request->hasParam("pin") && request->hasParam("state")) {
      int pinId = request->getParam("pin")->value().toInt();
      bool value = request->getParam("state")->value() == "off" ? false : true;

      if (pinId >= 0 && pinId <= 39) {
        DEBUG("Making pin %i an output and writing value %i", pinId, value);
        pinMode(pinId, OUTPUT);
        digitalWrite(pinId, value);
      }
    }
    request->send(200, "text/plain", "k");
  });

  webServer.on("/led", HTTP_GET, (ArRequestHandlerFunction) [](AsyncWebServerRequest *request) {
    if (request->hasParam("value")) {
      uint8_t ledMask = 0x0f & request->getParam("value")->value().toInt();

      // invert because low is LED on
      ledMask = ~ledMask;
      ledMask = ledMask << 4;

      uint8_t outputs = ioExpander.readRegister(1);
      outputs = (ledMask & 0xf0) | (outputs & 0x0f);

      DEBUG("New LED mask: 0x%x out: 0x%x", ledMask, outputs);

      ioExpander.setOutput(outputs);
    }
    request->send(200, "text/plain", "k");
  });
#endif

  webServer.on("/remountcard", HTTP_GET, (ArRequestHandlerFunction) [](AsyncWebServerRequest *request) {
    initializeCard();
    request->send(200, "text/plain", "done");
  });

  // On Aldis One we only initialize if the card is present. See loop()
  initializeCard();

#ifdef USE_SDMMC
  webServer.addHandler(new SPIFFSEditor(SD_MMC, "kbox", "test"));
#elif defined(USE_SDSPI)
  webServer.addHandler(new SPIFFSEditor(SD, "kbox", "test"));
#else
  webServer.addHandler(new SPIFFSEditor(SPIFFS, "kbox", "test"));
#endif
  webServer.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  webServer.begin();

  WebSocketSender *wsSender = new WebSocketSender();



  nmeaServer.start();

  // runBenchmark();
  // printPartitionInfo();

#ifdef ALDISONE
  DEBUG("expander input: %x", ioExpander.readRegister(0));
  DEBUG("expander output: %x", ioExpander.readRegister(1));
  DEBUG("expander inversion: %x", ioExpander.readRegister(2));
  DEBUG("expander config: %x", ioExpander.readRegister(3));

#if defined(ALDISONE_v10)
  // We only have one input: the sdcard detection on pin P3 (P0-P7)
  ioExpander.configurePins(0b00001000);
  // Turn on all LEDs, keep NMEA RX/TX disabled and NMEA2000 enabled
  ioExpander.setOutput(0b00000110);
#elif defined(ALDISONE_v11)
  // no inputs
  ioExpander.configurePins(0x00);
  // turn on all leds - enable n2k - disable nmea tx
  ioExpander.setOutput(0b00000010);
#endif


#ifdef USE_NMEA2000
  /*
   With CAN32 - which works:
    D main.cpp:675 N2k Registers before init - MOD=8e RMC=8e RXE=8e TXE=8e
    D main.cpp:683 N2k Registers after init - MOD=0 RMC=0 RXE=0 TXE=0

   With Aldis one - does not work:
    MOD=f1 RMC=f1 RXE=f1 TXE=f1
    MOD=0 RMC=0 RXE=81 TXE=0

   Also MOD shows at 1 when querying later ... interesting. that is the reset flag.
   and if I use the same pins as CAN32:
    MOD=f9 RMC=f9 RXE=f9 TXE=f9
    MOD=0 RMC=0 RXE=0 TXE=88

   With WRover-Kit - with ALDISONE config
    MOD=49 RMC=49 RXE=49 TXE=49
    MOD=0 RMC=0 RXE=81 TXE=0
   With WROVER Config => same.
    And if it's connected to traffic, MOD1, RXE:128 TXE:128 will appear soon.


*/

  DEBUG("N2k Registers before init - MOD=%x RMC=%x RXE=%x TXE=%x",
        MODULE_CAN->MOD.U, MODULE_CAN->RMC.U, MODULE_CAN->RXERR.U, MODULE_CAN->TXERR.U);

  // Enable n2k transceiver otherwise it will detect a fault and put itself offline
  ioExpander.setOutputPin(exp_n2k_en, false);

  nmea2000Task.setup(n2k_tx, n2k_rx);
  nmea2000Task.addWriter(&sdLoggingTask);
  nmea2000Task.addWriter(wsSender);
  nmea2000Task.addWriter(&nmeaServer);

  DEBUG("N2k Registers after init - MOD=%x RMC=%x SR=%x RXE=%x TXE=%x",
        MODULE_CAN->MOD.U, MODULE_CAN->RMC.U, MODULE_CAN->SR.U, MODULE_CAN->RXERR.U, MODULE_CAN->TXERR.U);
#endif


  // We have an external pullup - no internal pullup on this pin.
  pinMode(34 /* EXP_INT */, INPUT);
#ifdef ALDISONE_v10
  attachInterrupt(digitalPinToInterrupt(34), handleExpanderInterrupt, FALLING);
#elif defined(ALDISONE_v11)
  attachInterrupt(digitalPinToInterrupt(34), handleSdcardInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(0), handleUserButtonInterrupt, CHANGE);
#endif

  bmp280.begin(0x76);
#endif

  // Configure SVP and SVN as inputs.
// We use unsigned long here since 32 is a long
#define GPIO_BIT_MASK  ((1ULL<<GPIO_NUM_36) | (1ULL<<GPIO_NUM_39))

  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = GPIO_BIT_MASK;
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  gpio_config(&io_conf);
}

void loop() {
  static elapsedMillis lastLogMessage = 0;
  if (lastLogMessage > 1000) {
#ifdef USE_NMEA2000
    DEBUG("KBox Running - Free heap=%i (min %i - largest %i - total %i) - N2K MOD=%x RMC=%x SR=%x RXE=%x TXE=%x",
          heap_caps_get_free_size(MALLOC_CAP_INTERNAL), heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL),
          heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL), heap_caps_get_free_size(MALLOC_CAP_DEFAULT),
          MODULE_CAN->MOD.U, MODULE_CAN->RMC.U, MODULE_CAN->SR.U, MODULE_CAN->RXERR.U,
          MODULE_CAN->TXERR.U);
#else
    DEBUG("KBox Running - Free heap=%i", ESP.getFreeHeap());
#endif
    lastLogMessage = 0;
  }

  static elapsedMillis lastSDCardWrite = 0;
  if (lastSDCardWrite > 2000) {
    sdLoggingTask.loop();
    lastSDCardWrite = 0;
  }
#ifdef USE_NMEA2000
  nmea2000Task.loop();
  static elapsedMillis lastNMEA2000Write = 0;
  if (lastNMEA2000Write > 200) {
    tN2kMsg m;
    SetN2kWindSpeed(m, 0, 3.2, 1.2, N2kWind_True_North);
    if (!nmea2000Task.write(m)) {
      //ERROR("Unable to write sentence to nmea2000");
    }
    lastNMEA2000Write = 0;
  }
#endif

#ifdef USE_SERIAL
  const char *rx1 = serialTask1.readLine();
  if (rx1) {
    DEBUG("NMEA1: %s", rx1);
  }
  const char *rx2 = serialTask2.readLine();
  if (rx2) {
    DEBUG("NMEA2: %s", rx2);
  }
  static elapsedMillis lastNMEA1Write = 0;
  if (lastNMEA1Write > 200) {
    if (!serialTask1.write("SOME NMEA SENTENCE ON NMEAOUTPUT1")) {
      ERROR("Unable to write sentence to nmea1");
    }
    lastNMEA1Write = 0;
  }
  static elapsedMillis lastNMEA2Write = 0;
  if (lastNMEA2Write > 200) {
    if (!serialTask2.write("SOME NMEA SENTENCE ON NMEAOUTPUT2")) {
      ERROR("Unable to write sentence to nmea2");
    }
    lastNMEA2Write = 0;
  }
#endif

#ifdef ALDISONE_v10
  // Triggered by the interrupt handler
  if (needToReadExpander) {
    uint8_t inputs = ioExpander.readInput();
    DEBUG("IO Expander state changed - New value: %x - sdcard: %s", inputs, inputs & 8 ? "IN":"OUT");
    if (inputs & 8) {
      // Card need some time to get ready. Mounting immediately fails.
      delay(500);
      initializeCard();
    }
    else {
      SD_MMC.end();
    }
    needToReadExpander = false;
  }
#elif defined(ALDISONE_v11)
  // Triggered by the interrupt handler
  if (sdcardInterrupt) {
    int sdcardStatus = digitalRead(34);
    DEBUG("IO Expander state changed - New value: %x - sdcard: %s", sdcardStatus, sdcardStatus ? "IN":"OUT");
    if (sdcardStatus) {
      // Card need some time to get ready. Mounting immediately fails.
      delay(500);
      initializeCard();
    }
    else {
      SD_MMC.end();
    }
    sdcardInterrupt = false;
  }
#endif

  if (restartRequired) {
    DEBUG("Rebooting ESP in 1s");
    // gives time to http server to confirm we are done.
    delay(1000);
    ESP.restart();
  }
}
