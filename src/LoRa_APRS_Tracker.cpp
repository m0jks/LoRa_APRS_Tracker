/*__________________________________________________________________________________________________________________________________

██╗      ██████╗ ██████╗  █████╗      █████╗ ██████╗ ██████╗ ███████╗    ████████╗██████╗  █████╗  ██████╗██╗  ██╗███████╗██████╗ 
██║     ██╔═══██╗██╔══██╗██╔══██╗    ██╔══██╗██╔══██╗██╔══██╗██╔════╝    ╚══██╔══╝██╔══██╗██╔══██╗██╔════╝██║ ██╔╝██╔════╝██╔══██╗
██║     ██║   ██║██████╔╝███████║    ███████║██████╔╝██████╔╝███████╗       ██║   ██████╔╝███████║██║     █████╔╝ █████╗  ██████╔╝
██║     ██║   ██║██╔══██╗██╔══██║    ██╔══██║██╔═══╝ ██╔══██╗╚════██║       ██║   ██╔══██╗██╔══██║██║     ██╔═██╗ ██╔══╝  ██╔══██╗
███████╗╚██████╔╝██║  ██║██║  ██║    ██║  ██║██║     ██║  ██║███████║       ██║   ██║  ██║██║  ██║╚██████╗██║  ██╗███████╗██║  ██║
╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝    ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═╝╚══════╝       ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝

                                                    Ricardo Guzman - CA2RXU 
                                          https://github.com/richonguzman/LoRa_APRS_Tracker
                                            (donations : http://paypal.me/richonguzman)                                                                       
__________________________________________________________________________________________________________________________________*/

#include <BluetoothSerial.h>
#include <OneButton.h>
#include <TinyGPS++.h>
#include <Arduino.h>
#include <logger.h>
#include <WiFi.h>
#include <vector>
#include "APRSPacketLib.h"
#include "notification_utils.h"
#include "bluetooth_utils.h"
#include "keyboard_utils.h"
#include "configuration.h"
#include "station_utils.h"
#include "button_utils.h"
#include "pins_config.h"
#include "power_utils.h"
#include "menu_utils.h"
#include "lora_utils.h"
#include "msg_utils.h"
#include "gps_utils.h"
#include "bme_utils.h"
#include "ble_utils.h"
#include "display.h"
#include "SPIFFS.h"
#include "utils.h"

Configuration                       Config;
HardwareSerial                      neo6m_gps(1);
TinyGPSPlus                         gps;
#ifdef HAS_BT_CLASSIC
BluetoothSerial                     SerialBT;
#endif
#ifdef BUTTON_PIN
OneButton userButton                = OneButton(BUTTON_PIN, true, true);
#endif

String      versionDate             = "2024.05.07-F4HVV";

uint8_t     myBeaconsIndex          = 0;
int         myBeaconsSize           = Config.beacons.size();
Beacon      *currentBeacon          = &Config.beacons[myBeaconsIndex];
uint8_t     loraIndex               = 0;
int         loraIndexSize           = Config.loraTypes.size();
LoraType    *currentLoRaType        = &Config.loraTypes[loraIndex];

int         menuDisplay             = 100;

int         messagesIterator        = 0;
std::vector<String>                 loadedAPRSMessages;
std::vector<String>                 loadedWLNKMails;
std::vector<String>                 outputMessagesBuffer;
std::vector<String>                 outputAckRequestBuffer;

bool        displayEcoMode          = Config.display.ecoMode;
bool        displayState            = true;
uint32_t    displayTime             = millis();
uint32_t    refreshDisplayTime      = millis();

bool        sendUpdate              = true;
uint8_t     updateCounter           = Config.sendCommentAfterXBeacons;
bool	    sendStandingUpdate      = false;
bool        statusState             = true;
uint32_t    statusTime              = millis();
bool        bluetoothConnected      = false;
bool        bluetoothActive         = false;
bool        sendBleToLoRa           = false;
String      BLEToLoRaPacket         = "";
uint32_t    bluetoothDisableTimer   = millis();

bool        messageLed              = false;
uint32_t    messageLedTime          = millis();
uint8_t     lowBatteryPercent       = 21;

uint32_t    lastTelemetryTx         = millis();
uint32_t    telemetryTx             = millis();

uint32_t    lastTx                  = 0.0;
uint32_t    txInterval              = 60000L;
uint32_t    lastTxTime              = millis();
double      lastTxLat               = 0.0;
double      lastTxLng               = 0.0;
double      lastTxDistance          = 0.0;
double      currentHeading          = 0;
double      previousHeading         = 0;

uint32_t    menuTime                = millis();
bool        symbolAvailable         = true;

uint32_t    bmeLastReading          = -60000;

uint8_t     screenBrightness        = 1;
bool        keyboardConnected       = false;
bool        keyDetected             = false;
uint32_t    keyboardTime            = millis();
String      messageCallsign         = "";
String      messageText             = "";

bool        flashlight              = false;
bool        digirepeaterActive      = false;
bool        sosActive               = false;
bool        disableGPS;

bool        miceActive              = false;

bool        smartBeaconValue        = true;

int         ackRequestNumber;
bool        ackRequestState         = false;
String      ackCallsignRequest      = "";
String      ackNumberRequest        = "";
uint32_t    lastMsgRxTime           = millis();
uint32_t    lastRetryTime           = millis();

uint8_t     winlinkStatus           = 0;
String      winlinkMailNumber       = "_?";
String      winlinkAddressee        = "";
String      winlinkSubject          = "";
String      winlinkBody             = "";
String      winlinkAlias            = "";
String      winlinkAliasComplete    = "";
bool        winlinkCommentState     = false;

bool        wxRequestStatus         = false;
uint32_t    wxRequestTime           = 0;
uint32_t    batteryMeasurmentTime   = 0;

APRSPacket                          lastReceivedPacket;

logging::Logger                     logger;

void checkBluetoothState();
void checkDeepSleepNeeded();

void setup() {
    Serial.begin(115200);

    #ifndef DEBUG
    logger.setDebugLevel(logging::LoggerLevel::LOGGER_LEVEL_INFO);
    #endif

    POWER_Utils::setup();
    setup_display();
    POWER_Utils::externalPinSetup();

    STATION_Utils::loadIndex(0);
    STATION_Utils::loadIndex(1);
    startupScreen(loraIndex, versionDate);

    MSG_Utils::loadNumMessages();
    GPS_Utils::setup();
    currentLoRaType = &Config.loraTypes[loraIndex];
    LoRa_Utils::setup();
    BME_Utils::setup();
    
    ackRequestNumber = random(1,999);

    WiFi.mode(WIFI_OFF);
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "Main", "WiFi controller stopped");

    checkBluetoothState();

    if (!Config.simplifiedTrackerMode) {
        #ifdef BUTTON_PIN
        userButton.attachClick(BUTTON_Utils::singlePress);
        userButton.attachLongPressStart(BUTTON_Utils::longPress);
        userButton.attachDoubleClick(BUTTON_Utils::doublePress);
        userButton.attachMultiClick(BUTTON_Utils::multiPress);
        #endif
        KEYBOARD_Utils::setup();
    }

    POWER_Utils::lowerCpuFrequency();
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "Main", "Smart Beacon is: %s", Utils::getSmartBeaconState());
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "Main", "Setup Done!");
    menuDisplay = 0;
}

void loop() {
    currentBeacon = &Config.beacons[myBeaconsIndex];
    if (statusState) {
        if (Config.validateConfigFile(currentBeacon->callsign)) {
            KEYBOARD_Utils::rightArrow();
            currentBeacon = &Config.beacons[myBeaconsIndex];
        }
        miceActive = Config.validateMicE(currentBeacon->micE);
    }
    STATION_Utils::checkSmartBeaconValue();

    POWER_Utils::batteryManager();

    if (!Config.simplifiedTrackerMode) {
        #ifdef BUTTON_PIN
        userButton.tick();
        #endif
    }

    Utils::checkDisplayEcoMode();

    if (keyboardConnected) KEYBOARD_Utils::read();
    #ifdef TTGO_T_DECK_GPS
    KEYBOARD_Utils::mouseRead();
    #endif

    GPS_Utils::getData();
    bool gps_time_update = gps.time.isUpdated();
    bool gps_loc_update  = gps.location.isUpdated();
    GPS_Utils::setDateFromData();

    MSG_Utils::checkReceivedMessage(LoRa_Utils::receivePacket());
    MSG_Utils::processOutputBuffer();
    MSG_Utils::ledNotification();
    Utils::checkFlashlight();
    STATION_Utils::checkListenedTrackersByTimeAndDelete();

    checkBluetoothState();

    int currentSpeed = (int) gps.speed.kmph();

    if (gps_loc_update) {
        Utils::checkStatus();
        STATION_Utils::checkTelemetryTx();
    }
    lastTx = millis() - lastTxTime;
    if (!sendUpdate && gps_loc_update && smartBeaconValue) {
        GPS_Utils::calculateDistanceTraveled();
        if (!sendUpdate) {
            GPS_Utils::calculateHeadingDelta(currentSpeed);
        }
        STATION_Utils::checkStandingUpdateTime();
    }
    STATION_Utils::checkSmartBeaconState();
    checkDeepSleepNeeded();
    if (sendUpdate && gps_loc_update) STATION_Utils::sendBeacon("GPS");
    if (gps_time_update) STATION_Utils::checkSmartBeaconInterval(currentSpeed);
  
    if (millis() - refreshDisplayTime >= 1000 || gps_time_update) {
        GPS_Utils::checkStartUpFrames();
        MENU_Utils::showOnScreen();
        refreshDisplayTime = millis();
    }
}

void checkBluetoothState() {
  if (!Config.bluetoothActive) {
    return;
  }

  if (bluetoothActive) {
    if (bluetoothConnected) {
      bluetoothDisableTimer = millis();

      if (Config.bluetoothType == 0) {
        BLE_Utils::sendToLoRa();
      } else {
#ifdef HAS_BT_CLASSIC
        BLUETOOTH_Utils::sendToLoRa();
#endif
      }
    } else if (Config.disableBluetoothIfNotUsedOnBattery && !POWER_Utils::isUsbConnected()
               && millis() - bluetoothDisableTimer >= 30000) {
      logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "Bluetooth", "Disabled because we are on battery without any connection");
      if (Config.bluetoothType == 0) {
        BLE_Utils::end();
      } else {
#ifdef HAS_BT_CLASSIC
        BLUETOOTH_Utils::end();
#endif
      }
      bluetoothActive = false;
    }
  } else if (!Config.disableBluetoothIfNotUsedOnBattery || POWER_Utils::isUsbConnected()) { // No rule or (UsbOnly and no usb connected)
    if (Config.bluetoothType == 0) {
      BLE_Utils::setup();
    } else {
#ifdef HAS_BT_CLASSIC
      BLUETOOTH_Utils::setup();
#endif
    }
    bluetoothActive = true;
  }
}

void checkDeepSleepNeeded() {
  if (Config.secondsToSleepWhenNoMotion <= 0 // Sleep not activated
      || lastTx < Config.standingUpdateTime * 60 * 1000 // A trame has been sent so there is movement or Kiss
      || bluetoothConnected  // Device connected on Bluetooth so can't sleep
      || POWER_Utils::isUsbConnected()) { // USB connected so don't need to sleep
    return;
  }

  logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "Main", "Module goes to sleep");
  POWER_Utils::disableChgLed();
  POWER_Utils::deactivateMeasurement();
  POWER_Utils::deactivateLoRa();
  POWER_Utils::deactivateGPS();
  if (bluetoothActive) {
    btStop(); // We don't need to stop BLE or BT Classic because at wake up there is a reset
    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "Bluetooth", "Disabled");
  }
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF);
  esp_sleep_enable_ext1_wakeup(POWER_Utils::getIrqPinMask(), ESP_EXT1_WAKEUP_ALL_LOW); // Wake up when USB inserted
  esp_sleep_enable_timer_wakeup(Config.secondsToSleepWhenNoMotion * 1000 * 1000);
  esp_deep_sleep_start();
}