#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID        "1fa3fdf2-0a5c-40eb-b520-6d31560637ab"
#define CHARACTERISTIC_UUID "44f675d0-c42f-4a7a-9e9d-6acb50c9d162"

BLEServer *pServer = nullptr;
BLECharacteristic *pCharacteristic = nullptr;
bool deviceConnected = false;
bool notificationSent = false;

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) override {
    deviceConnected = true;
    notificationSent = false;
  }

  void onDisconnect(BLEServer *pServer) override {
    deviceConnected = false;
    notificationSent = false;
    // Restart advertising so the app can reconnect
    pServer->getAdvertising()->start();
  }
};

String getRodId() {
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_BT);
  char id[7];
  snprintf(id, sizeof(id), "%02X%02X%02X", mac[3], mac[4], mac[5]);
  return String(id);
}

void setup() {
  Serial.begin(115200);

  String rodId = getRodId();
  String deviceName = "Chabite-" + rodId;
  Serial.println(deviceName + " starting...");

  BLEDevice::init(deviceName.c_str());

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );

  // Add the descriptor required for notifications
  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->start();

  Serial.println("BLE advertising started, waiting for connection...");
}

void loop() {
  if (deviceConnected && !notificationSent) {
    String msg = "Chabite-" + getRodId() + ": Device started";
    pCharacteristic->setValue((uint8_t *)msg.c_str(), msg.length());
    pCharacteristic->notify();
    notificationSent = true;
    Serial.println("Notification sent to app");
  }

  delay(100);
}
