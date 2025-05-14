// basic code buat jam tangan yg bisa di set pake tombol, langsung modif sesuka kalian.
// banyak yang bisa di tambahin, kaya sleep mode, hitung mundur, stop watch, dan lain lain.
// kalo kalian pusing gimana cara codingnya dan pengen di bantu request aja IoT Custom Order di lynk.id/plexynex. nanti dapet code ino yang bisa kalian pelajarin sendiri lengkap sesuai requesan.

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Button pins
#define BTN_UP D5
#define BTN_DOWN D6
#define BTN_SELECT D4
#define BTN_BACK D3

int watchHours = 0;
int watchMinutes = 0;
int watchSeconds = 0;
unsigned long lastTimeUpdate = 0;

enum EditMode { NONE, EDIT_HOURS, EDIT_MINUTES, EDIT_SECONDS };
EditMode currentEditMode = NONE;
int menuItem = 0;
bool inMenu = false;

void setup() {
  Serial.begin(115200);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.display();
  
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_BACK, INPUT_PULLUP);
  
  // Set inisiasi waktu awal (optional) | boleh pake boleh ga,
  // watchHours = 12;
  // watchMinutes = 0;
  // watchSeconds = 0;
}

void loop() {
  if (millis() - lastTimeUpdate >= 1000 && currentEditMode == NONE) {
    lastTimeUpdate = millis();
    watchSeconds++;
    if (watchSeconds >= 60) {
      watchSeconds = 0;
      watchMinutes++;
      if (watchMinutes >= 60) {
        watchMinutes = 0;
        watchHours++;
        if (watchHours >= 24) {
          watchHours = 0;
        }
      }
    }
  }

  handleButtons();
  displayWatch();
  delay(50);
}

void displayWatch() {
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Digital Watch");
  display.drawLine(0, 10, 128, 10, WHITE);
  
  display.setTextSize(2);
  display.setCursor(14, 20);

  if (currentEditMode == EDIT_HOURS) {
    display.setTextColor(BLACK, WHITE);
    display.printf("%02d", watchHours);
    display.setTextColor(WHITE);
    display.printf(":%02d:%02d", watchMinutes, watchSeconds);
  } 
  else if (currentEditMode == EDIT_MINUTES) {
    display.printf("%02d:", watchHours);
    display.setTextColor(BLACK, WHITE);
    display.printf("%02d", watchMinutes);
    display.setTextColor(WHITE);
    display.printf(":%02d", watchSeconds);
  } 
  else if (currentEditMode == EDIT_SECONDS) {
    display.printf("%02d:%02d:", watchHours, watchMinutes);
    display.setTextColor(BLACK, WHITE);
    display.printf("%02d", watchSeconds);
    display.setTextColor(WHITE);
  } 
  else {
    display.printf("%02d:%02d:%02d", watchHours, watchMinutes, watchSeconds);
  }

  if (inMenu) {
    display.setTextSize(1);
    display.setCursor(0, 45);
    display.print(menuItem == 0 ? "> Adjust Time" : "  Adjust Time");

    display.setCursor(0, 55);
    display.print(menuItem == 1 ? "> Back to Watch" : "  Back to Watch");
  }
  
  display.display();
}

void handleButtons() {
  static unsigned long lastButtonPress = 0;
  
  if (millis() - lastButtonPress < 200) {
    return;
  }
  
  if (digitalRead(BTN_UP) == LOW) {
    lastButtonPress = millis();
    
    if (currentEditMode == EDIT_HOURS) {
      watchHours = (watchHours + 1) % 24;
    } 
    else if (currentEditMode == EDIT_MINUTES) {
      watchMinutes = (watchMinutes + 1) % 60;
    } 
    else if (currentEditMode == EDIT_SECONDS) {
      watchSeconds = (watchSeconds + 1) % 60;
    } 
    else if (inMenu) {
      menuItem = (menuItem - 1 + 2) % 2; 
    }
  }
  
  if (digitalRead(BTN_DOWN) == LOW) {
    lastButtonPress = millis();
    
    if (currentEditMode == EDIT_HOURS) {
      watchHours = (watchHours - 1 + 24) % 24;
    } 
    else if (currentEditMode == EDIT_MINUTES) {
      watchMinutes = (watchMinutes - 1 + 60) % 60;
    } 
    else if (currentEditMode == EDIT_SECONDS) {
      watchSeconds = (watchSeconds - 1 + 60) % 60;
    } 
    else if (inMenu) {
      menuItem = (menuItem + 1) % 2;
    }
  }
  
  if (digitalRead(BTN_SELECT) == LOW) {
    lastButtonPress = millis();
    
    if (inMenu) {
      if (menuItem == 0) {
        inMenu = false;
        currentEditMode = EDIT_HOURS;
      } 
      else if (menuItem == 1) {
        inMenu = false;
        currentEditMode = NONE;
      }
    } 
    else if (currentEditMode == NONE) {
      inMenu = true;
    } 
    else {
      if (currentEditMode == EDIT_HOURS) {
        currentEditMode = EDIT_MINUTES;
      } 
      else if (currentEditMode == EDIT_MINUTES) {
        currentEditMode = EDIT_SECONDS;
      } 
      else if (currentEditMode == EDIT_SECONDS) {
        currentEditMode = NONE;
        lastTimeUpdate = millis();
      }
    }
  }
  
  if (digitalRead(BTN_BACK) == LOW) {
    lastButtonPress = millis();
    
    if (currentEditMode != NONE) {
      currentEditMode = NONE;
      lastTimeUpdate = millis();
    } 
    else if (inMenu) {
      inMenu = false;
    } 
    else {
      inMenu = true;
    }
  }
}
