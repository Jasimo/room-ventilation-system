
// TGS2600
#define TGS2600_DEFAULTPPM         10             //default ppm of CO2 for calibration
long    TGS2600_DEFAULTRO        = 45000;         //default Ro for TGS2600_DEFAULTPPM ppm of CO2
#define TGS2600_SCALINGFACTOR      0.3555567714   //CO2 gas value
#define TGS2600_EXPONENT           -3.337882361   //CO2 gas value
#define TGS2600_MAXRSRO            2.428          //for CO2
#define TGS2600_MINRSRO            0.358          //for CO2
float   TGS2600_RL               = 1000;



void loopDHTRead() {
  // Diese Routine liest den oder die DHT Sensoren aus
  currentMillis = millis();
  if (currentMillis - previousMillisDHTRead >= intervalDHTRead) {
    previousMillisDHTRead = currentMillis;
    sensors_event_t event;

    if (DHT1IsAvailable) {
      dht1.temperature().getEvent(&event);
      if (isnan(event.temperature)) {
        Serial.println(F("Failed reading temperature from DHT1"));
      }
      else if (event.temperature != DHT1Temp) {
        DHT1Temp = event.temperature;
        Serial.print(F("DHT1 T: "));
        Serial.println(event.temperature);
      }

      dht1.humidity().getEvent(&event);
      if (isnan(event.relative_humidity)) {
        Serial.println(F("Failed reading humidity from DHT1"));
      }
      else if (event.relative_humidity != DHT1Hum) {
        DHT1Hum = event.relative_humidity;
        Serial.print(F("DHT1 H: "));
        Serial.println(event.relative_humidity);
      }
    }
    if (DHT2IsAvailable) {
      dht2.temperature().getEvent(&event);
      if (isnan(event.temperature)) {
        Serial.println(F("Failed reading temperature from DHT2"));
      }
      else if (event.temperature != DHT2Temp) {
        DHT2Temp = event.temperature;
        Serial.print(F("DHT2 T: "));
        Serial.println(event.temperature);
      }

      dht2.humidity().getEvent(&event);
      if (isnan(event.relative_humidity)) {
        Serial.println(F("Failed reading humidity from DHT2"));
      }
      else if (event.relative_humidity != DHT2Hum) {
        DHT2Hum = event.relative_humidity;
        Serial.print(F("DHT2 H: "));
        Serial.println(event.relative_humidity);
      }
    }
  }
}

// **************************** CO2 Sensor MH-Z14 ******************************************
const uint8_t cmdReadGasPpm[9]   = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
const uint8_t cmdCalZeroPoint[9] = {0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};
#define Co2Min 402

boolean SetupMHZ14() {
  Serial2.begin(9600);
  MHZ14IsAvailable = true;
  return true;
}

void loopMHZ14Read() {
  // Auslesen des CO2 Sensors
  if (MHZ14IsAvailable) {
    currentMillis = millis();
    if (currentMillis - previousMillisMHZ14Read >= intervalMHZ14Read) {
      previousMillisMHZ14Read = currentMillis;

      uint8_t response[9];
      Serial2.write(cmdReadGasPpm, 9);
      if (Serial2.readBytes(response, 9) == 9) {
        int responseHigh = (int) response[2];
        int responseLow = (int) response[3];
        int ppm = (256 * responseHigh) + responseLow;

        // Automatische Kalibrieren des Nullpunktes auf den kleinstmöglichen Wert
        if (ppm < Co2Min) MHZ14CalibrateZeroPoint;

        MHZ14_CO2_ppm = ppm;
      }
    }
  }
}

void MHZ14CalibrateZeroPoint() {
  if (MHZ14IsAvailable) {
    Serial2.write(cmdCalZeroPoint, 9);
  }
}

char getChecksum(char *packet) {
  char i, checksum;
  checksum = 0;
  for (i = 1; i < 8; i++) {
    checksum += packet[i];
  }
  checksum = 0xff - checksum;
  checksum += 1;
  return checksum;
}

// ----------------------------- TGS2600 ------------------------------------

float calcSensor_VOC(int valr)
{
  float val;
  float TGS2600_ro;
  float valAIQ;

  //Serial.println(valr);
  if (valr > 0) {
    val =  ((float)TGS2600_RL * (1024 - valr) / valr);
    TGS2600_ro = TGS2600_getro(val, TGS2600_DEFAULTPPM);
    //convert to ppm (using default ro)
    valAIQ = TGS2600_getppm(val, TGS2600_DEFAULTRO);
  }
  Serial.print ( F("Vrl / Rs / ratio:"));
  Serial.print ( val);
  Serial.print ( F(" / "));
  Serial.print ( TGS2600_ro);
  Serial.print ( F(" / "));
  Serial.println ( valAIQ);
}


/*
   get the calibrated ro based upon read resistance, and a know ppm
*/
long TGS2600_getro(float resvalue, float ppm) {
  return (long)(resvalue * exp( log(TGS2600_SCALINGFACTOR / ppm) / TGS2600_EXPONENT ));
}

/*
   get the ppm concentration
*/
float TGS2600_getppm(long resvalue, long ro) {
  float ret = 0;
  ret = (float)TGS2600_SCALINGFACTOR * pow(((float)resvalue / ro), TGS2600_EXPONENT);
  return ret;
}

void loopVocRead() {
  if (TGS2600IsAvailable) {
    currentMillis = millis();
    if (currentMillis - previousMillisTGS2600Read >= intervalTGS2600Read) {
      previousMillisTGS2600Read = currentMillis;
      int analogVal = analogRead(sensPinVoc);
      TGS2600_VOC = calcSensor_VOC(analogVal);
    }
  }
}

boolean SetupTGS2600(){
  int analogVal = analogRead(sensPinVoc);
  if (analogVal > 100) return true; else return false;
}

