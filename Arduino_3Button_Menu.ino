#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8
//Mosi 11
//SCKL 13

#define BUTTON_UP 2
#define BUTTON_DOWN 3
#define BUTTON_SELECT 4

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

struct MenuItem {
  String name;
  void (*action)();
  MenuItem* subMenu;
  int subMenuSize;
};

void goBack();

MenuItem subMenu1[] = {
  { "Sub Option 1", []() {
     tft.fillScreen(ST77XX_BLACK);
     tft.setCursor(10, 100);
     tft.print("Sub 1 Selected!");
     delay(1000);
   },
    nullptr, 0 },
  { "Sub Option 2", []() {
     tft.fillScreen(ST77XX_BLACK);
     tft.setCursor(10, 100);
     tft.print("Sub 2 Selected!");
     delay(1000);
   },
    nullptr, 0 },
  { "Geri", goBack, nullptr, 0 }
};

MenuItem subMenu2[] = {
  { "Other Option", []() {
     tft.fillScreen(ST77XX_BLACK);
     tft.setCursor(10, 100);
     tft.print("Otherone Selected!");
     delay(1000);
   },
    nullptr, 0 },
  { "Geri", goBack, nullptr, 0 }
};

MenuItem mainMenu[] = {
  { "Option 1", nullptr, subMenu1, 3 },
  { "Option 2", nullptr, subMenu2, 2 },
  { "Option 3", []() {
     tft.fillScreen(ST77XX_BLACK);
     tft.setCursor(10, 100);
     tft.print("Option 3 Selected!");
     delay(1000);
   },
    nullptr, 0 },
};

MenuItem* currentMenu = mainMenu;
int currentMenuSize = 4;
int currentItem = 0;
int lastItem = -1;

MenuItem* menuStack[10];
int menuStackIndex = 0;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);
  tft.init(240, 280);
  tft.setRotation(2);
  tft.fillScreen(ST77XX_BLACK);
  drawFullMenu();
}

void loop() {
  if (digitalRead(BUTTON_UP) == LOW) {
    if (currentItem > 0) {
      lastItem = currentItem;
      currentItem--;
      updateMenu();
    }
    delay(200);
  }
  if (digitalRead(BUTTON_DOWN) == LOW) {
    if (currentItem < currentMenuSize - 1) {
      lastItem = currentItem;
      currentItem++;
      updateMenu();
    }
    delay(200);
  }
  if (digitalRead(BUTTON_SELECT) == LOW) {
    selectItem();
    delay(200);
  }
}

void drawFullMenu() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(2);
  for (int i = 0; i < currentMenuSize; i++) {
    tft.setCursor(10, 20 + i * 30);
    if (i == currentItem) {
      tft.setTextColor(ST77XX_YELLOW);
      tft.print("> ");
    } else {
      tft.setTextColor(ST77XX_WHITE);
      tft.print("  ");
    }
    tft.println(currentMenu[i].name);
  }
  lastItem = currentItem;
}

void updateMenu() {
  tft.setTextSize(2);
  if (lastItem != -1) {
    tft.fillRect(0, 20 + lastItem * 30, tft.width(), 20, ST77XX_BLACK);
    tft.setCursor(10, 20 + lastItem * 30);
    tft.setTextColor(ST77XX_WHITE);
    tft.print("  " + currentMenu[lastItem].name);
  }
  tft.fillRect(0, 20 + currentItem * 30, tft.width(), 20, ST77XX_BLACK);
  tft.setCursor(10, 20 + currentItem * 30);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("> " + currentMenu[currentItem].name);
  lastItem = currentItem;
}

void selectItem() {
  MenuItem selected = currentMenu[currentItem];
  if (selected.subMenu != nullptr) {
    menuStack[menuStackIndex++] = currentMenu;
    currentMenu = selected.subMenu;
    currentMenuSize = selected.subMenuSize;
    currentItem = 0;
    lastItem = -1;
    drawFullMenu();
  } else if (selected.action != nullptr) {
    selected.action();
    drawFullMenu();
  }
}

void goBack() {
  if (menuStackIndex > 0) {
    currentMenu = menuStack[--menuStackIndex];
    currentMenuSize = (currentMenu == mainMenu) ? 4 : (currentMenu == subMenu1 ? 3 : 2);
    currentItem = 0;
    lastItem = -1;
    drawFullMenu();
  }
}