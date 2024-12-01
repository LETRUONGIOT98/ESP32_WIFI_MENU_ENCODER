#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // Địa chỉ I2C và kích thước màn hình LCD
const int encoderPinA = 18; // Chân A của bộ mã đọc góc xoay
const int encoderPinB = 19; // Chân B của bộ mã đọc góc xoay
const int buttonPin = 5; // Chân nút nhấn
volatile boolean aSetLast = false;
volatile boolean bSetLast = false;
bool tt = true;
int numberOfNetworks = 0;
int selectedNetwork = 0;
String wifiNames[30]; // Mảng lưu tên các mạng WiFi, giả sử tối đa có 30 mạng
bool isConnected = false; // Biến để xác định kết nối đến WiFi
int old = - 1;
void setup()
{
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP); // Cài đặt chân nút nhấn
  attachInterrupt(digitalPinToInterrupt(encoderPinA), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), updateEncoder, CHANGE);

  lcd.init();                      // Khởi tạo màn hình LCD
  lcd.backlight();                 // Bật đèn nền

  lcd.setCursor(0, 0);
  lcd.print("Scanning WiFi...");

  // Set WiFi to station mode and disconnect from an AP if it was previously connected.
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Setup done");
}

void loop()
{
  if (numberOfNetworks == 0) {
    // WiFi.scanNetworks will return the number of networks found.
    numberOfNetworks = WiFi.scanNetworks();
    Serial.println("Scan done");

    for (int i = 0; i < numberOfNetworks; ++i) {
      wifiNames[i] = WiFi.SSID(i);
    }
  }

  // Check if button is pressed
  if (digitalRead(buttonPin) == LOW) {
    tt = !tt;
    
    delay(300);
  }
  if(tt == false){
    displaySelectedWiFiInfo();
  }
  else{
   displayWiFiMenu();
  }
  delay(100); // Wait a bit before scanning again.
}

void displayWiFiMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Networks:");
  if (selectedNetwork <= 0) selectedNetwork = 0;
  if (selectedNetwork >= numberOfNetworks) selectedNetwork = numberOfNetworks - 1;
  int startIndex = selectedNetwork - 1;
  if (startIndex < 0) {
    startIndex = 0;
  }
  int endIndex = min(startIndex + 3, numberOfNetworks);

  for (int i = startIndex; i < endIndex; ++i) {
    lcd.setCursor(0, i - startIndex + 1);
    if (i == selectedNetwork) {
      lcd.print(">");
    } else {
      lcd.print(" ");
    }
    lcd.print(wifiNames[i]);
  }
}

void displaySelectedWiFiInfo() {
  if (selectedNetwork >= 0 && selectedNetwork < numberOfNetworks) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DA CHON: ");
    lcd.setCursor(0, 1);
    lcd.print(wifiNames[selectedNetwork]);
    lcd.setCursor(0, 2);
    lcd.print("RSSI: ");
    lcd.print(WiFi.RSSI(selectedNetwork));
  }
}

void updateEncoder() {
  // Đọc trạng thái mới của các chân encoder
  boolean aSetNew = digitalRead(encoderPinA);
  boolean bSetNew = digitalRead(encoderPinB);

  // Kiểm tra sự thay đổi trong trạng thái của encoder và cập nhật vị trí
  if (aSetLast == LOW && bSetLast == LOW) {
    if (aSetNew == HIGH && bSetNew == LOW) {
      selectedNetwork++;
    } else if (aSetNew == LOW && bSetNew == HIGH) {
      selectedNetwork--;
    }
  }

  // Lưu trạng thái mới
  aSetLast = aSetNew;
  bSetLast = bSetNew;
}
