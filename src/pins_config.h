#ifndef PINS_CONFIG_H_
#define PINS_CONFIG_H_

#undef OLED_SDA
#undef OLED_SCL
#undef OLED_RST


#if defined(TTGO_T_Beam_V0_7) || defined(TTGO_T_Beam_V1_0) || defined(TTGO_T_Beam_V1_2) || defined(TTGO_T_Beam_V1_0_SX1268) || defined(ESP32_DIY_1W_LoRa_GPS) || defined(TTGO_T_Beam_V1_2_SX1262) || defined(TTGO_T_Beam_S3_SUPREME_V3) || defined(OE5HWN_MeshCom) || defined(HELTEC_V3_GPS)
#define HAS_BUTTON
#endif

#if defined(TTGO_T_Beam_V1_0_SX1268) || defined(ESP32_DIY_1W_LoRa_GPS) || defined(TTGO_T_Beam_V1_2_SX1262) || defined(TTGO_T_Beam_S3_SUPREME_V3) || defined(HELTEC_V3_GPS) || defined(OE5HWN_MeshCom)
#define HAS_SX126X
#endif

#if defined(TTGO_T_Beam_V0_7) || defined(TTGO_T_Beam_V1_0) || defined(TTGO_T_LORA32_V2_1_TNC) || defined(TTGO_T_Beam_V1_2) || defined(ESP32_DIY_LoRa_GPS) || defined(TTGO_T_LORA32_V2_1_GPS) || defined(ESP32_C3_DIY_LoRa_GPS)
#define HAS_SX127X
#endif

#if defined(TTGO_T_Beam_V1_0) || defined(TTGO_T_Beam_V1_0_SX1268)
#define HAS_AXP192
#endif

#if defined(TTGO_T_Beam_V1_2) || defined(TTGO_T_Beam_V1_2_SX1262) || defined(TTGO_T_Beam_S3_SUPREME_V3)
#define HAS_AXP2101
#endif

#if !defined(TTGO_T_Beam_S3_SUPREME_V3) && !defined(HELTEC_V3_GPS) && !defined(ESP32_C3_DIY_LoRa_GPS)
#define OLED_SDA            21
#define OLED_SCL            22
#define OLED_RST            16
#define HAS_BT_CLASSIC
#endif


// ********** GPS and RADIO pins **********
#if defined(TTGO_T_Beam_V1_0) || defined(TTGO_T_Beam_V1_2) || defined(TTGO_T_Beam_V1_0_SX1268) || defined(TTGO_T_Beam_V1_2_SX1262) || defined(TTGO_T_LORA32_V2_1_GPS) || defined(ESP32_DIY_LoRa_GPS)
#define GPS_RX              12
#define GPS_TX              34
#endif

#if defined(TTGO_T_Beam_V1_0) || defined(TTGO_T_Beam_V1_2) || defined(TTGO_T_Beam_V0_7) || defined(TTGO_T_LORA32_V2_1_GPS) || defined(ESP32_DIY_LoRa_GPS) || defined(TTGO_T_LORA32_V2_1_TNC) || defined(TTGO_T_Beam_V1_0_SX1268) || defined(TTGO_T_Beam_V1_2_SX1262)
#define RADIO_SCLK          5
#define RADIO_MISO          19
#define RADIO_MOSI          27
#define RADIO_CS            18  // CS  --> NSS
#define RADIO_DIO0          26  // IRQ --> DIO0
#define RADIO_RST           23
#define RADIO_DIO1          33
#define RADIO_BUSY          32
#endif

#if defined(TTGO_T_Beam_V1_0) || defined(TTGO_T_Beam_V1_2) || defined(TTGO_T_Beam_V1_0_SX1268) || defined(TTGO_T_Beam_V1_2_SX1262)
#define BUTTON_PIN          38 // The middle button GPIO on the T-Beam
#endif


#if defined(TTGO_T_LORA32_V2_1_GPS) || defined(ESP32_DIY_LoRa_GPS)
#define BUTTON_PIN          -1
#define BATTERY_PIN         35  //LoRa32 Battery PIN 100k/100k
#endif

#ifdef TTGO_T_LORA32_V2_1_TNC
#define GPS_RX              -1
#define GPS_TX              -1
#define BUTTON_PIN          -1
#define BATTERY_PIN         35  //LoRa32 Battery PIN 100k/100k
#endif


#ifdef ESP32_DIY_1W_LoRa_GPS
#define GPS_RX              17
#define GPS_TX              16
#define BUTTON_PIN          15
#define RADIO_SCLK          18
#define RADIO_MISO          19
#define RADIO_MOSI          23
#define RADIO_CS            5
#define RADIO_RST           27
#define RADIO_DIO1          12
#define RADIO_BUSY          14
#define RADIO_RXEN          32
#define RADIO_TXEN          25
#endif

#ifdef TTGO_T_Beam_V0_7
#define GPS_RX              15
#define GPS_TX              12
#define BUTTON_PIN          39
#endif

#if defined(TTGO_T_Beam_S3_SUPREME_V3)
#define OLED_SDA            17
#define OLED_SCL            18
#define OLED_RST            16
#define GPS_RX              8
#define GPS_TX              9
#define BUTTON_PIN          0
#define RADIO_SCLK          12
#define RADIO_MISO          13
#define RADIO_MOSI          11
#define RADIO_CS            10
#define RADIO_DIO0          -1
#define RADIO_RST           5
#define RADIO_DIO1          1
#define RADIO_BUSY          4
#endif


#if defined(HELTEC_V3_GPS)
#define OLED_SDA            17
#define OLED_SCL            18
#define OLED_RST            21
#define GPS_RX              47
#define GPS_TX              48
#define BUTTON_PIN          0
#define RADIO_SCLK          9
#define RADIO_MISO          11
#define RADIO_MOSI          10
#define RADIO_CS            8
#define RADIO_RST           12
#define RADIO_DIO1          14
#define RADIO_BUSY          13
#define BATTERY_PIN         1
#endif


#if defined(OE5HWN_MeshCom)
#define GPS_RX              17
#define GPS_TX              16
#define BUTTON_PIN          12
#define RADIO_SCLK          18
#define RADIO_MISO          19
#define RADIO_MOSI          23
#define RADIO_CS            5
#define RADIO_RST           27
#define RADIO_DIO1          33
#define RADIO_BUSY          26
#define RADIO_RXEN          14
#define RADIO_TXEN          13
#endif


#if defined(ESP32_C3_DIY_LoRa_GPS)
#define OLED_SDA            8
#define OLED_SCL            9
#define OLED_RST            10
#define GPS_RX              20
#define GPS_TX              21
#define BUTTON_PIN          -1
#define RADIO_SCLK          4
#define RADIO_MISO          5
#define RADIO_MOSI          6
#define RADIO_CS            7  // CS  --> NSS
#define RADIO_RST           3
#define RADIO_IRQ           2  // IRQ --> DIO0
#define BATTERY_PIN         1
#endif

#endif