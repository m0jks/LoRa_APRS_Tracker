#include <RadioLib.h>
#include <logger.h>
#include <SPI.h>
#include "notification_utils.h"
#include "configuration.h"
#include "pins_config.h"
#include "msg_utils.h"
#include "display.h"

extern logging::Logger  logger;
extern Configuration    Config;
extern LoraType         *currentLoRaType;
extern int              loraIndex;
extern int              loraIndexSize;

int                     state = RADIOLIB_ERR_NONE;


#if defined(TTGO_T_Beam_V1_0_SX1268) || defined(ESP32_DIY_1W_LoRa_GPS) || defined(OE5HWN_MeshCom)
SX1268 radio = new Module(RADIO_CS, RADIO_DIO1, RADIO_RST, RADIO_BUSY);
bool transmissionFlag = true;
bool operationDone = true;
#endif
#if defined(TTGO_T_Beam_V1_2_SX1262) || defined(TTGO_T_Beam_S3_SUPREME_V3) || defined(HELTEC_V3_GPS)
SX1262 radio = new Module(RADIO_CS, RADIO_DIO1, RADIO_RST, RADIO_BUSY);
bool transmissionFlag = true;
bool operationDone = true;
#endif
#if defined(HAS_SX127X)
SX1278 radio = new Module(RADIO_CS, RADIO_DIO0, RADIO_RST, RADIO_DIO1);
bool transmissionFlag = true;
bool operationDone = true;
#endif


namespace LoRa_Utils {

    void setFlag() {
        transmissionFlag = true;   
    }

    void changeFreq() {
        if(loraIndex >= (loraIndexSize - 1)) {
            loraIndex = 0;
        } else {
            loraIndex++;
        }
        currentLoRaType = &Config.loraTypes[loraIndex];
        float freq = (float)currentLoRaType->frequency/1000000;   //////////////////////////////////////////////////////////////////// modificar!!!!
        radio.setFrequency(freq);
        radio.setSpreadingFactor(currentLoRaType->spreadingFactor);
        float signalBandwidth = currentLoRaType->signalBandwidth/1000;
        radio.setBandwidth(signalBandwidth);
        radio.setCodingRate(currentLoRaType->codingRate4);

        #if defined(TTGO_T_Beam_V1_0_SX1268) || defined(TTGO_T_Beam_V1_2_SX1262) || defined(TTGO_T_Beam_S3_SUPREME_V3) || defined(HELTEC_V3_GPS)
        state = radio.setOutputPower(currentLoRaType->power + 2); // values available: 10, 17, 22 --> if 20 in tracker_conf.json it will be updated to 22.
        #endif
        #if defined(ESP32_DIY_1W_LoRa_GPS) || defined(OE5HWN_MeshCom)
        state = radio.setOutputPower(currentLoRaType->power); 
        #endif
        #ifdef HAS_SX127X
        state = radio.setOutputPower(currentLoRaType->power - 3); 
        #endif
        if (state == RADIOLIB_ERR_NONE) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "LoRa", "Initializing Power: %d", state);
        } else {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "LoRa", "Power set failed! Code:%d", state);
        }

        String loraCountryFreq;
        switch (loraIndex) {
            case 0:
                loraCountryFreq = "Eu/World";
                break;
            case 1:
                loraCountryFreq = "Poland";
                break;
            case 2:
                loraCountryFreq = "UK";
                break;
        }
        String currentLoRainfo = "LoRa " + loraCountryFreq + " / Freq: " + String(currentLoRaType->frequency)  + " / SF:" + String(currentLoRaType->spreadingFactor) + " / CR: " + String(currentLoRaType->codingRate4);
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "LoRa", currentLoRainfo.c_str());
        show_display("LORA FREQ>", "", "CHANGED TO: " + loraCountryFreq, "", "", "", 2000);
    }

    void setup() {
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "LoRa", "Set SPI pins!");
        #ifdef HAS_SX126X
        SPI.begin(RADIO_SCLK, RADIO_MISO, RADIO_MOSI);
        #endif
        #ifdef HAS_SX127X
        SPI.begin(RADIO_SCLK, RADIO_MISO, RADIO_MOSI, RADIO_CS);
        #endif
        float freq = (float)currentLoRaType->frequency/1000000;   //////////////////////////////////////////////////////////////////// modificar!!!!
        state = radio.begin(freq);
        if (state == RADIOLIB_ERR_NONE) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "LoRa", "Initializing LoRa Radio");
        } else {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "LoRa", "Starting LoRa failed!");
            while (true);
        }
        #ifdef HAS_SX126X
        radio.setDio1Action(setFlag);
        #endif
        #ifdef HAS_SX127X
        radio.setDio0Action(setFlag, RISING);
        //radio.setPreambleLength(8);
        #endif

        radio.setSpreadingFactor(currentLoRaType->spreadingFactor);
        radio.setBandwidth(currentLoRaType->signalBandwidth/1000);   //////////////////////////////////////////////////////////////////// modificar!!!!
        radio.setCodingRate(currentLoRaType->codingRate4);
        radio.setCRC(true);
        #if defined(ESP32_DIY_1W_LoRa_GPS) || defined(OE5HWN_MeshCom)
        radio.setRfSwitchPins(RADIO_RXEN, RADIO_TXEN);
        #endif

        /*#ifdef DIO3_TCXO_REF
        radio.setTCXO(DIO3_TCXO_REF, 5000);
        #endif*/

        #if defined(TTGO_T_Beam_V1_0_SX1268) || defined(TTGO_T_Beam_V1_2_SX1262) || defined(TTGO_T_Beam_S3_SUPREME_V3) || defined(HELTEC_V3_GPS)
        state = radio.setOutputPower(currentLoRaType->power + 2);   // values available: 10, 17, 22 --> if 20 in tracker_conf.json it will be updated to 22.
        #endif
        #if defined(ESP32_DIY_1W_LoRa_GPS) || defined(OE5HWN_MeshCom)
        state = radio.setOutputPower(currentLoRaType->power);       // max value 20 (when 20dB in setup 30dB in output as 400M30S has Low Noise Amp) 
        #endif
        #ifdef HAS_SX127X
        state = radio.setOutputPower(currentLoRaType->power - 3);   //SX1278 max value 17dB NOTE: 20 dBm value allows high power operation, but transmission duty cycle MUST NOT exceed 1%
        #endif
        
        #if defined(HAS_SX127X)
        //radio.setGain(currentLoRaType->lnaGain);
        radio.setGain(0);
        #endif

        if (state == RADIOLIB_ERR_NONE) {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "LoRa", "LoRa init done!");
        } else {
            logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "LoRa", "Starting LoRa failed!");
            while (true);
        }
    }

    void sendNewPacket(const String &newPacket) {
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "LoRa Tx","---> %s", newPacket.c_str());
        /*logger.log(logging::LoggerLevel::LOGGER_LEVEL_WARN, "LoRa","Send data: %s", newPacket.c_str());
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_ERROR, "LoRa","Send data: %s", newPacket.c_str());
        logger.log(logging::LoggerLevel::LOGGER_LEVEL_DEBUG, "LoRa","Send data: %s", newPacket.c_str());*/

        if (Config.ptt.active) {
            digitalWrite(Config.ptt.io_pin, Config.ptt.reverse ? LOW : HIGH);
            delay(Config.ptt.preDelay);
        }
        if (Config.notification.ledTx) digitalWrite(Config.notification.ledTxPin, HIGH);
        if (Config.notification.buzzerActive && Config.notification.txBeep) NOTIFICATION_Utils::beaconTxBeep();
        state = radio.transmit("\x3c\xff\x01" + newPacket);
        transmissionFlag = true;
        if (state == RADIOLIB_ERR_NONE) {
            //Serial.println(F("success!"));
        } else if (state == RADIOLIB_ERR_PACKET_TOO_LONG) {
            Serial.println(F("too long!"));
        } else if (state == RADIOLIB_ERR_TX_TIMEOUT) {
            Serial.println(F("timeout!"));
        } else {
            Serial.print(F("failed, code "));
            Serial.println(state);
        }
        if (Config.notification.ledTx) digitalWrite(Config.notification.ledTxPin, LOW);
        if (Config.ptt.active) {
            delay(Config.ptt.postDelay);
            digitalWrite(Config.ptt.io_pin, Config.ptt.reverse ? HIGH : LOW);
        }
    }

    ReceivedLoRaPacket receivePacket() {
        ReceivedLoRaPacket receivedLoraPacket;
        String packet = "";
        if(operationDone) {
            operationDone = false;
            if (transmissionFlag) {
                radio.startReceive();
                transmissionFlag = false;
            } else {
                state = radio.readData(packet);
                if (state == RADIOLIB_ERR_NONE) {
                    if(!packet.isEmpty()) {
                        if(!packet.isEmpty()) {
                            Serial.println("LoRa Rx ---> " + packet);
                        }
                        receivedLoraPacket.text       = packet;
                        receivedLoraPacket.rssi       = radio.getRSSI();
                        receivedLoraPacket.snr        = radio.getSNR();
                        receivedLoraPacket.freqError  = radio.getFrequencyError();
                        return receivedLoraPacket;
                    }
                } else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
                    // timeout occurred while waiting for a packet
                } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
                    Serial.println(F("CRC error!"));
                } else {
                    Serial.print(F("failed, code "));
                    Serial.println(state);
                }
            }
        }
        return receivedLoraPacket;





        /*ReceivedLoRaPacket receivedLoraPacket;
        String packet = "";
        if (transmissionFlag) {
            transmissionFlag = false;
            radio.startReceive();
            state = radio.readData(packet);
            if (state == RADIOLIB_ERR_NONE) {
                if(!packet.isEmpty()) {
                    logger.log(logging::LoggerLevel::LOGGER_LEVEL_INFO, "LoRa Rx","---> %s", packet.c_str());
                }
                receivedLoraPacket.text       = packet;
                receivedLoraPacket.rssi       = radio.getRSSI();
                receivedLoraPacket.snr        = radio.getSNR();
                receivedLoraPacket.freqError  = radio.getFrequencyError();
            } else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
                // timeout occurred while waiting for a packet
            } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
                Serial.println(F("CRC error!"));
            } else {
                Serial.print(F("failed, code "));
                Serial.println(state);
            }
        }
        return receivedLoraPacket;*/
    }

}