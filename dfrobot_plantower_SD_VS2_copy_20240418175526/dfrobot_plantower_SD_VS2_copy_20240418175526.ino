/*!
 * @file  SEN0233.ino
 * @brief Air Quality Monitor (HCHO, Temperature & Humidity)
 * @n Purchase the module here: https://www.dfrobot.com/product-1612.html
 * @n This example detects formaldehyde, temperature, and humidity in the environment.
 * @n Modified by Alejandro Rebolledo to enhance formaldehyde detection as per datasheet.
 * @n Modified to interface with a MySQL server; consider implementing key-based authentication or encryption for security enhancements.
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license  The MIT License (MIT)
 * @author  [lijun](ju.li@dfrobot.com)
 * @version  V2.0
 * @date  19/01/2024
 */

// Definitions for the GSM module and serial buffer size
#define TINY_GSM_MODEM_SIM7600
#define TINY_GSM_RX_BUFFER 1024  // Set RX buffer to 1Kb
#define DUMP_AT_COMMANDS
#define SerialAT Serial1

// Include necessary libraries for handling sensors, communication, and display
#include <Arduino.h>
#include <TinyGsmClient.h>
#include <SoftwareSerial.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_NeoPixel.h>
#include "SD.h"
#include "SPI.h"
#include "FS.h"

// Definitions for deep sleep and UART communication
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for microseconds to seconds */
#define TIME_TO_SLEEP 30           /* Time ESP32 will go to sleep (in seconds) */
#define UART_BAUD 115200
#define MODEM_TX 27
#define MODEM_RX 26

// APN data for GPRS connection
const char apn[] = "gigsky-02";  // Placeholder APN
const char gprsUser[] = "";      // GPRS username
const char gprsPass[] = "";      // GPRS password

// Server URL for data posting
String serverName = "https://example.com/update?sensorID=";

#ifdef DUMP_AT_COMMANDS  // Conditional compilation for debugging AT commands
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, Serial);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

// OLED display configuration
U8G2_SH1107_SEEED_128X128_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// RTC (Real Time Clock) setup
RTC_DS3231 rtc;

// Software serial ports for communication with external devices
SoftwareSerial externo(externo_RX, externo_TX);  // RX, TX

// NeoPixel LED configuration
#define PIN 12
#define NUMPIXELS 1
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Button configuration for user interaction
#define BUTTON_PIN 39

// SD card configuration
#define SD_CS_PIN 13
SPIClass spiSD(HSPI);

// Global variables for managing state and storing sensor data
bool SDok = true;
bool rtcOK = true;
String ID = "21";  // ID for this particular sensor setup

// Retry parameters for network connection
const int MAX_RETRIES = 3;
const unsigned long RETRY_DELAY_MS = 10000;  // Delay between retries

/*!
 * Attempt to connect to the mobile network and establish a GPRS connection.
 * Returns true if successful, false otherwise.
 */
bool connectToNetwork() {
    for (int attempt = 1; attempt <= MAX_RETRIES; ++attempt) {
        Serial.print("Attempting connection ");
        Serial.print(attempt);
        Serial.print(" of ");
        Serial.println(MAX_RETRIES);

        if (modem.waitForNetwork()) {
            Serial.println("Mobile network connected.");
            if (modem.gprsConnect(apn, gprsUser, gprsPass)) {
                Serial.println("GPRS connection successful.");
                return true;
            } else {
                Serial.println("GPRS connection failed.");
            }
        } else {
            Serial.println("Failed to connect to mobile network.");
        }

        Serial.print("Waiting ");
        Serial.print(RETRY_DELAY_MS);
        Serial.println(" ms before retrying...");
        delay(RETRY_DELAY_MS);
    }

    Serial.println("Could not connect after ");
    Serial.print(MAX_RETRIES);
    Serial.println(" attempts.");
    return false;
}

/*!
 * System setup function. Initializes all modules, sets up communication, and checks initial device states.
 */
void setup() {
    Serial.begin(115200);
    externo.begin(9600);
    u8g2.begin();
    pixels.begin();

    pinMode(BUTTON_PIN, INPUT);

    if (!rtc.begin()) {
        Serial.println("RTC not found");
        rtcOK = false;
    }

    spiSD.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS_PIN);

    if (!SD.begin(SD_CS_PIN, spiSD)) {
        Serial.println("Failed to initialize SD card");
        SDok = false;
    } else {
        Serial.println("SD card initialized successfully");
        if (!SD.exists("/data.csv")) {
            Serial.println("Base file does not exist...");
            delay(500);
            Serial.println("creating files");
            String tosave = "DATE,TIME,TEMPERATURE,HUMIDITY,FORMALDEHYDE,CONNECTION\n";
            writeFile(SD, "/data.csv", tosave.c_str());
        } else {
            Serial.println("Base data is: /data.csv");
        }
    }

    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
    Serial.println("\nStart");
    pinMode(MODEM_PWRKEY, OUTPUT);
    digitalWrite(MODEM_PWRKEY, HIGH);
    delay(300);
    digitalWrite(MODEM_PWRKEY, LOW);

    pinMode(MODEM_FLIGHT, OUTPUT);
    digitalWrite(MODEM_FLIGHT, HIGH);
    delay(500);

    modem.restart();

    if (!modem.init()) {
        Serial.println("Failed to initialize modem");
    }

    if (!connectToNetwork()) {
        Serial.println("Failed to connect to network and GPRS.");
    }
}

/*!
 * Main program loop. Handles sensor readings, network communication, and user interactions.
 */
void loop() {
    if (rtcOK) {
        DateTime now = rtc.now();
        String fecha = getFecha(now);
        String hora = getHora(now);
    }

    if (externo.available()) {
        String receivedString = externo.readStringUntil('\n');
        receivedString.trim();
        Serial.print("Received: ");
        Serial.println(receivedString);
    }
}

/*!
 * Utility function to format the current date from the RTC.
 */
String getFecha(DateTime now) {
    char fecha[11];
    sprintf(fecha, "%02d/%02d/%04d", now.day(), now.month(), now.year());
    return String(fecha);
}

/*!
 * Utility function to format the current time from the RTC.
 */
String getHora(DateTime now) {
    char hora[9];
    sprintf(hora, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
    return String(hora);
}

/*!
 * Function to write a message to a file on the SD card.
 */
void writeFile(fs::FS &fs, const char *path, const char *message) {
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    if (file.print(message)) {
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}
