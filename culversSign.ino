#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

const char* ssid = "SFSD-Public";
const char* password = "sfsdpublic";
const String size = "5";
const String zip = "53235";
const int DELAY = 2500;
LiquidCrystal_I2C lcd(0x27 , 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  lcd.print("Connecting...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  lcd.clear();
  lcd.print("Connected!");

  delay(2000); // Delay to display the connection message
  lcd.clear();
}

void loop() {
  Webfinder();
  delay(10000); // Scrape every 10 seconds
}

void Webfinder() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://files.codemke.org/flavorScraper.php?zipcode=" + zip + "&limit=" + size); // Replace with your target URL
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println(payload);
      payload.replace("<br>", ":");
      String payloadEdit = payload;
      Serial.println(payloadEdit);

      int maxSize = 10;
      String arr[maxSize];
      int size = splitStringToArray(payloadEdit, ':', arr, maxSize);

      // Scrolling logic
      for (int i = 0; i < size; i += 2) {
        scrollText(arr[i], arr[i+1]);  // Scroll each element
        delay(2500);      // Delay between elements (adjust as needed)
      }

    } else {
      Serial.printf("Error code: %d\n", httpResponseCode);
      lcd.clear();
      lcd.print("HTTP Error"); // Indicate error on LCD
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
    lcd.clear();
    lcd.print("WiFi Disconnected");
  }
}

void scrollText(String flav, String res) {
  int flavLen = flav.length();
  int resLen = res.length();

  for (int i = 0; i <= flavLen - 16; i++) { // Scroll up to the point where the full message is displayed
    if (resLen <= 16) {
    lcd.clear();
    lcd.setCursor(0, 0);
    Serial.println(flav);
    lcd.print(flav.substring(i, i + 16));  // Print 16 character chunk
    lcd.setCursor(0, 1);
    Serial.println(res);
    lcd.print(res);
    if (i==0) {
    delay(DELAY);
    }
    delay(500);
    } else {
      for (int j = 0; j <= resLen - 16; j++) {
        lcd.clear();
        lcd.setCursor(0, 0);
        Serial.println(flav);
        lcd.print(flav.substring(i, i + 16));  // Print 16 character chunk
        lcd.setCursor(0, 1);
        lcd.print(res.substring(j, j + 16));
        if (i==0) {
        delay(DELAY);
        }
        delay(500);
      }
    }
  }
    // if the text is shorter than 16 characters, display without scrolling
    if(flavLen <= 16){
          if (resLen <= 16) {
          lcd.clear();
         lcd.setCursor(0,0);
         lcd.print(flav);
         lcd.setCursor(0, 1);
         Serial.println(res);
         lcd.print(res);
          } else {
              for (int i = 0; i <= resLen - 16; i++) {
                  lcd.clear();
                  lcd.setCursor(0,0);
                  lcd.print(flav);
                  lcd.setCursor(0, 1);
                  lcd.print(res.substring(i, i + 16));
                  if (i==0) {
                  delay(DELAY);
                  }
                  delay(500);
          }
    }
    }
}


int splitStringToArray(const String &str, char delimiter, String arr[], int maxSize) {
  int size = 0;
  int start = 0;
  int end = str.indexOf(delimiter);

  while (end != -1 && size < maxSize) {
    arr[size++] = str.substring(start, end);
    start = end + 1;
    end = str.indexOf(delimiter, start);
  }

  if (start < str.length() && size < maxSize) {
    arr[size++] = str.substring(start);
  }

  return size;
}