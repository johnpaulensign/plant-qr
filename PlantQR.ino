/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file i2c_mode.ino
 * @brief Unit QRCode I2C Mode Example
 * @version 0.2
 * @date 2024-06-19
 *
 *
 * @Hardwares: M5Core + Unit QRCode
 * @Platform Version: Arduino M5Stack Board Manager v2.1.0
 * @Dependent Library:
 * M5Unified: https://github.com/m5stack/M5Unified
 * M5GFX: https://github.com/m5stack/M5GFX
 * M5UnitQRCode: https://github.com/m5stack/M5Unit-QRCode
 */

#include <M5Unified.h>
#include <M5GFX.h>
#include "M5UnitQRCode.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "M5AtomS3.h"

// M5Canvas canvas(&M5.Display);

M5UnitQRCodeI2C qrcode;

// #define I2C_AUTO_SCAN_MODE


// Fill in the hotspot name and password you are connected to.
const char *ssid = "nvisia";
const char *password = "nvisia2020vision";
String serverName = "http://10.10.0.228:3000/plant/";

String currentPlant = "";

const uint8_t actionCount = 4;
uint8_t currentAction = 0;
const uint32_t actions[] = { WHITE, BLUE, PINK, ORANGE };
const String actionEndpoints[] = { "select", "water", "fertilize", "pest" };

void setup() {
  // M5.begin();

  AtomS3.begin();
  AtomS3.Display.setTextDatum(middle_center);
  AtomS3.Display.setTextSize(2);
  AtomS3.Display.fillScreen(BLACK);

  // WiFi
  AtomS3.Power.begin();  // Needed for wi-fi?
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }

  // canvas.setColorDepth(1);  // mono color
  // canvas.createSprite(M5.Display.width(), M5.Display.height());
  // canvas.setTextSize((float)canvas.width() / 80);
  // canvas.setTextScroll(true);

  while (!qrcode.begin(&Wire, UNIT_QRCODE_ADDR, 2, 1, 100000U)) {
    // canvas.println("Unit QRCode I2C Init Fail");
    Serial.println("Unit QRCode I2C Init Fail");
    // canvas.pushSprite(0, 0);
    delay(1000);
  }

  // canvas.println("Unit QRCode I2C Init Success");
  Serial.println("Unit QRCode I2C Init Success");
#ifdef I2C_AUTO_SCAN_MODE
  // canvas.println("Auto Scan Mode");
  // canvas.pushSprite(0, 0);
  qrcode.setTriggerMode(AUTO_SCAN_MODE);
#else
  // canvas.println("Manual Scan Mode");
  // canvas.pushSprite(0, 0);
  qrcode.setTriggerMode(MANUAL_SCAN_MODE);
#endif
}

void loop() {
  if (qrcode.getDecodeReadyStatus() == 1) {
    uint8_t buffer[512] = { 0 };
    uint16_t length = qrcode.getDecodeLength();
    Serial.printf("len:%d\r\n", length);
    qrcode.getDecodeData(buffer, length);
    Serial.printf("decode data: ");

    String plantID = "";

    for (int i = 0; i < length; i++) {
      Serial.printf("%c", buffer[i]);
      // canvas.printf("%c", buffer[i]);
      plantID = plantID + (char)buffer[i];
    }

    currentPlant = plantID;
    postPlantID();

    Serial.println();
    // canvas.println();
    // canvas.pushSprite(0, 0);
  }
#ifndef I2C_AUTO_SCAN_MODE
  AtomS3.update();
  if (M5.BtnA.wasPressed()) {
    // start scan
    // qrcode.setDecodeTrigger(1);
    cycleAction();
  }
  // if (M5.BtnB.wasPressed()) {
  //     // stop scan
  //     qrcode.setDecodeTrigger(0);
  // }
#endif
}

void cycleAction() {
  currentAction++;
  if (currentAction >= actionCount) {
    currentAction = 0;
    currentPlant = "";
  }

  Serial.printf("Cycling action to %d: %s\n", currentAction, actionEndpoints[currentAction]);
  redrawScreen();
}

void redrawScreen() {
  AtomS3.Display.fillScreen(BLACK);
  AtomS3.Display.drawString(actionEndpoints[currentAction], AtomS3.Display.width() / 2,
                            AtomS3.Display.height() / 3.5);
  // AtomS3.Display.drawString(currentPlant, AtomS3.Display.width() / 2,
  //                           AtomS3.Display.height() / 1.5);
}

void postPlantID() {
  redrawScreen();

  // HTTP
  HTTPClient http;

  String serverPath = serverName + currentPlant + "/" + actionEndpoints[currentAction];

  // Your Domain name with URL path or IP address with path
  http.begin(serverPath.c_str());

  // If you need Node-RED/server authentication, insert user and password below
  //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");

  // Send HTTP GET request
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
}