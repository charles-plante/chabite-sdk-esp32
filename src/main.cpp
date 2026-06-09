#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <esp_mac.h>
#include <rod_id.h>

#define SERVICE_UUID        "1fa3fdf2-0a5c-40eb-b520-6d31560637ab"
#define CHARACTERISTIC_UUID "44f675d0-c42f-4a7a-9e9d-6acb50c9d162"

BLEServer *pServer = nullptr;
BLECharacteristic *pCharacteristic = nullptr;
volatile bool sendStartupNotification = false;
volatile bool resumeAdvertising = false;

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) override {
    sendStartupNotification = true;
    // ESP32 stops advertising once a client connects; resume it so additional
    // phones can also pair to the same Chatbite simultaneously.
    resumeAdvertising = true;
  }

  void onDisconnect(BLEServer *pServer) override {
    resumeAdvertising = true;
  }
};

String getRodId() {
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_BT);
  char id[ROD_ID_LEN];
  formatRodId(mac, id, sizeof(id));
  return String(id);
}

void setup() {
  Serial.begin(115200);

  String rodId = getRodId();
  char nameBuf[ROD_DEVICE_NAME_LEN];
  buildDeviceName(rodId.c_str(), nameBuf, sizeof(nameBuf));
  String deviceName(nameBuf);
  Serial.println(deviceName + " starting...");

  BLEDevice::init(deviceName.c_str());

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );

  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->start();

  Serial.println("BLE advertising started, waiting for connection(s)...");
}

void loop() {
  if (resumeAdvertising) {
    resumeAdvertising = false;
    pServer->getAdvertising()->start();
  }

  if (sendStartupNotification && pServer->getConnectedCount() > 0) {
    sendStartupNotification = false;
    String msg = "Chabite-" + getRodId() + ": Device started";
    pCharacteristic->setValue((uint8_t *)msg.c_str(), msg.length());
    pCharacteristic->notify();
    Serial.print("Notification sent (");
    Serial.print(pServer->getConnectedCount());
    Serial.println(" client(s) connected)");
  }

  delay(100);
}
