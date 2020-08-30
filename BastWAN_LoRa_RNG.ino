#include <SPI.h>
#include <LoRa.h>
#include "LoRandom.h"

void writeRegister(uint8_t reg, uint8_t value) {
  LoRa.writeRegister(reg, value);
}

uint8_t readRegister(uint8_t reg) {
  return LoRa.readRegister(reg);
}

void hexDump(unsigned char *buf, uint16_t len) {
  String s = "|", t = "| |";
  Serial.println(F("  |.0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .a .b .c .d .e .f |"));
  Serial.println(F("  +------------------------------------------------+ +----------------+"));
  for (uint16_t i = 0; i < len; i += 16) {
    for (uint8_t j = 0; j < 16; j++) {
      if (i + j >= len) {
        s = s + "   "; t = t + " ";
      } else {
        char c = buf[i + j];
        if (c < 16) s = s + "0";
        s = s + String(c, HEX) + " ";
        if (c < 32 || c > 127) t = t + ".";
        else t = t + (char)c;
      }
    }
    uint8_t index = i / 16;
    Serial.print(index, HEX); Serial.write('.');
    Serial.println(s + t + "|");
    s = "|"; t = "| |";
  }
  Serial.println(F("  +------------------------------------------------+ +----------------+"));
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.flush();
  Serial.print(F("\n\n\n[SX1276] Initializing ... "));
  delay(1000);
  LoRa.setPins(SS, RFM_RST, RFM_DIO0);
  Serial.println("SS: " + String(SS));
  Serial.println("RFM_RST: " + String(RFM_RST));
  Serial.println("RFM_DIO0: " + String(RFM_DIO0));
  Serial.println("RFM_SWITCH: " + String(RFM_SWITCH));
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.print("Setting up LoRa ");
  pinMode(RFM_SWITCH, OUTPUT);
  digitalWrite(RFM_SWITCH, 1);
  LoRa.setTxPower(20, PA_OUTPUT_PA_BOOST_PIN);
  LoRa.setPreambleLength(8);
  setupLoRandom();
  Serial.println("[o]");
  delay(1000);
  pinMode(PIN_PA28, OUTPUT);
  digitalWrite(PIN_PA28, HIGH);
  Serial.println("End of setup\n\n");
  uint8_t x = 0;
  x = LoRa.readRegister(0x01);
  Serial.print("RegOpMode: 0x");
  if (x < 16) Serial.write('0');
  Serial.println(x, HEX);
  x = LoRa.readRegister(0x1D);
  Serial.print("RegModemConfig1: 0x");
  if (x < 16) Serial.write('0');
  Serial.println(x, HEX);
  x = LoRa.readRegister(0x1E);
  Serial.print("RegModemConfig2: 0x");
  if (x < 16) Serial.write('0');
  Serial.println(x, HEX);
}
/*
  SX1276 Register (address)     Register bit field (bit #)      Values    Note
  RegOpMode (0x01)              LongRangeMode[7]                ‘1’       LoRa mode enable
                                Mode[2:0]                       ‘101’     Receive Continuous mode
  ------------------------------------------------------------------------------------------------------------------
  RegModemConfig1 (0x1D)        Bw[7:4]                         ‘0111’    ‘0111’ for 125kHz modulation Bandwidth
                                CodingRate[3:1]                 ‘001’     4/5 error coding rate
                                ImplicitHeaderModeOn[0]         ‘0’       Packets have up-front header
  ------------------------------------------------------------------------------------------------------------------
  RegModemConfig2 (0x1E)        SpreadingFactor[7:4]            ‘0111’    ‘0111’ (SF7) = 6kbit/s

  To generate an N bit random number, perform N read operation of the register RegRssiWideband (address 0x2c)
  and use the LSB of the fetched value. The value from RegRssiWideband is derived from a wideband (4MHz) signal strength
  at the receiver input and the LSB of this value constantly and randomly changes.
*/

void loop() {
  unsigned char randomStock[256];
  // We'll build a stock of random bytes for use in code
  uint8_t randomIndex = 0;
  uint16_t i;
  for (i = 0; i < 256; i++) {
    uint8_t x = getLoRandomByte();
    randomStock[i] = x;
  }
  randomIndex = 0;
  hexDump(randomStock, 256);
  delay(2000);
}
