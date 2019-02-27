/*
   DS3231 Pinout
   SCL - A5
   SDA - A4

   Lixie Pinout
   DIN - 7

   Buttons
   Hour - 8
   Minute - 9
   Color - 10

   Switches
   Daylight Savings - 11
   Time Set - 12
   24 Hour - 13
*/
#include <Adafruit_NeoPixel.h>
#include <DS3231.h>
#include <Wire.h>

const int Lixie = 7;

const int buttonH = 8;
const int buttonM = 9;
const int buttonColor = 10;
const int switchDST = 11;
const int switchSet = 12;
const int switch24HR = 13;

const int patternAmount = 11;

int Hour = 12;
int Minute = 00;

byte WheelR (byte WheelPos);
byte WheelG (byte WheelPos);
byte WheelB (byte WheelPos);
void rainbowCycle();
void displayTime (byte r, byte g, byte b);
void getDigits();
void FreakOut();

struct RGB {
  int r;
  int g;
  int b;
};

RGB White = {255, 255, 255};
RGB AbsoluteZero = {0, 72, 186};
RGB AlizarinCrimson = {227, 38, 54};
RGB Ao = {0, 128, 0};
RGB Aqua = {0, 255, 255};
RGB BananaYellow = {255, 225, 53};
RGB CadmiumGreen = {0, 107, 60};
RGB BarbiePink = {224, 33, 138};
RGB BlazeOrange = {255, 103, 0};
RGB BrightGreen = {102, 255, 0};

RGB Rainbow = {0, 0, 0};

boolean twelveHour;
boolean DSTON;
boolean DSTOFF;

boolean PM;
boolean h12;

int litLEDS[8];
int digits[4];

int numbers1[10] = {9, 4, 2, 5, 8, 0, 3, 1, 6, 7};
int numbers2[10] = {19, 14, 12, 15, 18, 10, 13, 11, 16, 17};

int colorPattern = 0;

unsigned long previousMillis = 0;
unsigned long previousMillisRainbow = 0;

long rainbowCyclesCount = 0;

uint16_t rainbowCycleCount;

DS3231 Clock;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(80, Lixie, NEO_GRB + NEO_KHZ800);

void setup() {
  Wire.begin();

  Serial.begin(9600);

  strip.begin();
  strip.show();

  pinMode(buttonH, INPUT);
  pinMode(buttonM, INPUT);
  pinMode(buttonColor, INPUT);
  pinMode(switchDST, INPUT);
  pinMode(switchSet, INPUT);
  pinMode(switch24HR, INPUT);

  Clock.setClockMode(false);

  if (digitalRead(switchDST) == LOW) {
    DSTOFF = false;
    DSTON = true;
  } else if (digitalRead(switchDST) == HIGH) {
    DSTON = false;
    DSTOFF = true;
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (digitalRead(switchSet) == LOW) {
    if (currentMillis - previousMillis >= 100) {
      previousMillis = currentMillis;
      if (digitalRead(buttonH) == HIGH) {
        Hour++;
        if (Hour > 24) {
          Hour = 0;
        }
        Clock.setHour(Hour);
      }
      if (digitalRead(buttonM) == HIGH) {
        Minute++;
        if (Minute > 60) {
          Minute = 0;
        }
        Clock.setHour(Minute);
      }
    }
  }

  if (digitalRead(switch24HR) == HIGH) {
    twelveHour = true;
  } else if (digitalRead(switch24HR) == LOW) {
    twelveHour = false;
  }

  if (digitalRead(switchDST) == LOW) {
    if (DSTON == true) {
      DSTON = false;
      DSTOFF = true;
      if (twelveHour == true) {
        Hour++;
        if (Hour > 12) {
          Hour = 1;
        }
      } else if (twelveHour == false) {
        Hour++;
        if (Hour > 24) {
          Hour = 0;
        }
      }
    }
  } else if (digitalRead(switchDST) == HIGH) {
    if (DSTOFF == true) {
      DSTOFF = false;
      DSTON = true;
      if (twelveHour == true) {
        Hour--;
        if (Hour < 1) {
          Hour = 12;
        }
      } else if (twelveHour == false) {
        Hour--;
        if (Hour < 0) {
          Hour = 24;
        }
      }
    }
  }
  if (currentMillis - previousMillis >= 100) {
    previousMillis = currentMillis;

    if (digitalRead(buttonColor) == HIGH) {
      colorPattern++;
      if (colorPattern > patternAmount) {
        colorPattern = 0;
      }
    }
  }

  if (colorPattern == 0) {
    displayTime(White.r, White.g, White.b);
  } else if (colorPattern == 1) {
    displayTime(AbsoluteZero.r, AbsoluteZero.g, AbsoluteZero.b);
  } else if (colorPattern == 2) {
    displayTime(AlizarinCrimson.r, AlizarinCrimson.g, AlizarinCrimson.b);
  } else if (colorPattern == 3) {
    displayTime(Ao.r, Ao.g, Ao.b);
  } else if (colorPattern == 4) {
    displayTime(Aqua.r, Aqua.g, Aqua.b);
  } else if (colorPattern == 5) {
    displayTime(BananaYellow.r, BananaYellow.g, BananaYellow.b);
  } else if (colorPattern == 6) {
    displayTime(CadmiumGreen.r, CadmiumGreen.g, CadmiumGreen.b);
  } else if (colorPattern == 7) {
    displayTime(BarbiePink.r, BarbiePink.g, BarbiePink.b);
  } else if (colorPattern == 8) {
    displayTime(BlazeOrange.r, BlazeOrange.g, BlazeOrange.b);
  } else if (colorPattern == 9) {
    displayTime(BrightGreen.r, BrightGreen.g, BrightGreen.b);
  } else if (colorPattern == 10) {
    if (currentMillis - previousMillisRainbow >= 20) {
      previousMillisRainbow = currentMillis;
      rainbowCycle();
    }
    displayTime(Rainbow.r, Rainbow.g, Rainbow.b);
  }
}

byte WheelR (byte WheelPos) {
  WheelPos = 255 - WheelPos;

  if (WheelPos < 85) {
    return (255 - WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return (0);
  }
  WheelPos -= 170;
  return (WheelPos * 3);
}

byte WheelG (byte WheelPos) {
  WheelPos = 255 - WheelPos;

  if (WheelPos < 85) {
    return (0);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return (WheelPos * 3);
  }
  WheelPos -= 170;
  return (255 - WheelPos * 3);
}

byte WheelB (byte WheelPos) {
  WheelPos = 255 - WheelPos;

  if (WheelPos < 85) {
    return (WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return (255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return (0);
}

void rainbowCycle() {
  uint16_t i, j;

  if (rainbowCycleCount < (256 * 5)) {
    byte tempR = WheelR((256 + rainbowCycleCount) & 255);
    byte tempG = WheelG((256 + rainbowCycleCount) & 255);
    byte tempB = WheelB((256 + rainbowCycleCount) & 255);
    Rainbow = {tempR, tempG, tempB};

    rainbowCycleCount++;
  } else {
    rainbowCycleCount = 0;
  }
}

void displayTime (int r, int g, int b) {
  Hour = Clock.getHour(h12, PM);
  Minute = Clock.getMinute();

  if (twelveHour == true) {
    if (Hour > 12) {
      Hour = Hour - 12;
    }
  }

  digits[0] = (Hour / 10);
  digits[1] = (Hour % 10);
  digits[2] = (Minute / 10);
  digits[3] = (Minute % 10);

  if (Hour == 0) {
    digits[0] = 0;
    digits[1] = 0;
  }

  if (Minute == 0) {
    digits[2] = 0;
    digits[3] = 0;
  }

  getDigits();

  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }

  for (int j = 0; j < 8; j++) {
    strip.setPixelColor(litLEDS[j], r, g, b);
  }

  strip.show();
  Serial.print("Hour: ");
  Serial.print(Hour);
  Serial.print("    Min: ");
  Serial.print(Minute);
  Serial.print("    ");
  Serial.print(digits[0]);
  Serial.print(digits[1]);
  Serial.print(" ");
  Serial.print(digits[2]);
  Serial.print(digits[3]);
  Serial.print("    ");
  Serial.print(digitalRead(switchDST));
  Serial.print(digitalRead(switch24HR));
  Serial.println(digitalRead(switchSet));
}

void getDigits() {
  for (int k = 0; k < 4; k++) {
    for (int l = 0; l < 10; l++) {
      if (digits[k] == l) {
        litLEDS[(k * 2)] = ((k * 20) + numbers1[l]);
        litLEDS[(k * 2) + 1] = ((k * 20) + numbers2[l]);
      }
    }
  }
}

void FreakOut() {

}
