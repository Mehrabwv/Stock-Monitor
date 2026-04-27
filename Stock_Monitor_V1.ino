#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define BUTTON_PIN 8

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// WiFi
char ssid[] = "powboi";
char pass[] = "p0wfu4lif3!";

// Stooq CSV API
char serverAddress[] = "stooq.com";
int port = 443;

WiFiSSLClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);

struct Asset {
  const char* name;
  const char* symbol;
};

Asset assets[] = {
  {"AAPL", "aapl.us"},
  {"GOOG", "goog.us"},
  {"VOO",  "voo.us"},
  {"ACWI", "acwi.us"},
  {"NVDA", "nvda.us"},
  {"INTC", "intc.us"},
  {"AMD",  "amd.us"},
  {"ORCL", "orcl.us"},
  {"GLD",  "gld.us"},
  {"XOM",  "xom.us"}
};

int totalAssets = sizeof(assets) / sizeof(assets[0]);
int currentAsset = 0;

float currentPrice = 0;
float dayHigh = 0;
float dayLow = 0;
float dayOpen = 0;
float dayChange = 0;

bool lastButtonState = HIGH;
unsigned long lastButtonTime = 0;
unsigned long lastFetchTime = 0;

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Wire.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);

  showMessage("Stock Monitor", "Connecting WiFi");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    showMessage("WiFi failed", "Retrying...");
    delay(3000);
  }

  showMessage("WiFi Connected", "Loading...");
  delay(800);

  fetchStockData();
}

void loop() {
  handleButton();

  // Refresh only every 5 minutes to avoid slow repeated requests
  if (millis() - lastFetchTime > 300000) {
    fetchStockData();
  }

  drawStockScreen();
  delay(100);
}

void handleButton() {
  bool buttonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH && buttonState == LOW) {
    if (millis() - lastButtonTime > 300) {
      lastButtonTime = millis();

      currentAsset++;
      if (currentAsset >= totalAssets) currentAsset = 0;

      fetchStockData();
    }
  }

  lastButtonState = buttonState;
}

void fetchStockData() {
  showMessage(assets[currentAsset].name, "Fetching...");

  String path = "/q/l/?s=";
  path += assets[currentAsset].symbol;
  path += "&f=sd2t2ohlcv&h&e=csv";

  Serial.print("GET ");
  Serial.println(path);

  client.get(path);

  int statusCode = client.responseStatusCode();

  Serial.print("Status: ");
  Serial.println(statusCode);

  if (statusCode != 200) {
    showMessage("HTTP Error", String(statusCode));
    delay(1200);
    return;
  }

  String response = client.responseBody();

  Serial.println(response);

  parseCSV(response);

  lastFetchTime = millis();
}

void parseCSV(String csv) {
  // Example:
  // Symbol,Date,Time,Open,High,Low,Close,Volume
  // AAPL.US,2026-04-27,18:32:45,212.40,213.92,210.50,213.01,123456

  int firstNewline = csv.indexOf('\n');
  if (firstNewline < 0) return;

  String row = csv.substring(firstNewline + 1);
  row.trim();

  if (row.length() < 5 || row.indexOf("N/D") >= 0) {
    currentPrice = 0;
    dayHigh = 0;
    dayLow = 0;
    dayChange = 0;
    return;
  }

  String fields[8];

  int start = 0;
  for (int i = 0; i < 8; i++) {
    int comma = row.indexOf(',', start);

    if (comma == -1) {
      fields[i] = row.substring(start);
      break;
    } else {
      fields[i] = row.substring(start, comma);
      start = comma + 1;
    }

    fields[i].trim();
  }

  dayOpen = fields[3].toFloat();
  dayHigh = fields[4].toFloat();
  dayLow = fields[5].toFloat();
  currentPrice = fields[6].toFloat();

  dayChange = currentPrice - dayOpen;
}

void drawStockScreen() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(assets[currentAsset].name);

  display.setCursor(92, 0);
  display.print("FAST");

  display.setTextSize(1);
  display.setCursor(0, 14);
  display.print("Price: ");
  display.setTextSize(2);
  display.print(currentPrice, 2);

  display.setTextSize(1);
  display.setCursor(0, 34);
  display.print("High: ");
  display.print(dayHigh, 2);

  display.setCursor(0, 44);
  display.print("Low : ");
  display.print(dayLow, 2);

  display.setCursor(0, 54);
  display.print("Chg : ");
  display.print(dayChange, 2);

  display.display();
}

void showMessage(String line1, String line2) {
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 18);
  display.println(line1);

  display.setCursor(0, 34);
  display.println(line2);

  display.display();
}