#include "configuration.h"
//#include "KISS_TO_TNC2.h"
//#include "lora_utils.h"
//#include "display.h"
//#include "logger.h"

#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>
#include <NimBLEUUID.h>

#define DEVICE_NAME "LoRaTracker"

NimBLEServer *pServer;
NimBLECharacteristic *pCharacteristic;

// Define the BLE service and characteristic UUIDs
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer) {
        Serial.println("Client connected");
    }

    void onDisconnect(NimBLEServer* pServer) {
        Serial.println("Client disconnected");
    }
};

class MyCharacteristicCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        Serial.print("Received value: ");
        Serial.println(value.c_str());
    }
};





namespace BLE_Utils {
  
  void setup() {
    Serial.println("BLE Setup");
    Serial.print("Free heap: ");
  Serial.println(ESP.getFreeHeap());

    NimBLEDevice::init(DEVICE_NAME);
    //NimBLEDevice::setPower(ESP_PWR_LVL_P7); // Set transmit power level (optional)

    // Create the server
    pServer = NimBLEDevice::getServer();
    //pServer->setCallbacks(new MyServerCallbacks());
    Serial.println("BLE Setup 2");


    Serial.print("Free heap: ");
    Serial.println(ESP.getFreeHeap());
    // Create the service
    NimBLEService *pService = pServer->createService(SERVICE_UUID);
    Serial.println("BLE Setup 3");

    Serial.print("Free heap: ");
    Serial.println(ESP.getFreeHeap());


    /*
      // Create the characteristic
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::WRITE
    );
    pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

    // Start the service
    pService->start();

    // Start advertising
    pServer->startAdvertising();
    */


    /*pServer = NimBLEDevice::createServer();

    NimBLEService* pService = pServer->createService(NimBLEUUID("180D"));

    // Characteristic 1 - Battery Level
    NimBLECharacteristic* pCharacteristic1 = pService->createCharacteristic(NimBLEUUID("2A19"),NIMBLE_PROPERTY::READ);
    pCharacteristic1->setValue(50);

    // Characteristic 2 - Custom
    pCharacteristic2 = pService->createCharacteristic(NimBLEUUID("A35F"),NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
    pCharacteristic2->setValue("Hello, World!");

    pService->start();
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(pService->getUUID());
    pAdvertising->start();*/
  }

}