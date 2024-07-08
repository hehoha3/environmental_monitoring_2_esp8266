#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include  <Adafruit_ST7735.h>
#include  <Adafruit_GFX.h>
#include  <SPI.h>
#define TFT_CS         D1      //---------------
#define TFT_RST        D2      //---------------                                      
#define TFT_DC         D3      //---------------
#define TFT_MOSI       D4      //---------------
#define TFT_SCLK       D5      //---------------
//----------------------------------------------
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

///
const char *WIFI_SSID = "B301-Sinhvien";
const char *WIFI_PASSWORD = "";

const char *MQTT_HOST = "10.23.10.22";
const int MQTT_PORT = 1883;
const char *MQTT_CLIENT_ID = "ESP8266 NodeMCU";
const char *MQTT_USER = "";
const char *MQTT_PASSWORD = "";
const char *TOPICTEMP = "dht/temp";
const char *TOPICHUM = "dht/hum";
////
WiFiClient client;
PubSubClient mqttClient(client);
///topic

void setup()
{
    Serial.begin(115200);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("Connected to Wi-Fi");

    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    mqttClient.setCallback(callback);

    while (!client.connected()) {
        if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD )) {
            Serial.println("Connected to MQTT broker");
        } else {
            delay(500);
            Serial.print(".");
        }
    }
    mqttClient.subscribe(TOPICTEMP);
    mqttClient.subscribe(TOPICHUM);
    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(ST7735_BLACK);
}

void callback(char* topic, byte* payload, unsigned int length)
{
    payload[length] = '\0';
    int value = String((char*) payload).toInt();

    Serial.println(topic);
    Serial.println(value);
    char buffer[10]; // Đảm bảo buffer có đủ kích thước để chứa chuỗi ký tự
    itoa(value, buffer, 10); // Chuyển đổi giá trị số thành chuỗi ký tự
    char textBuffer[15];
    printText("VKU", ST7735_YELLOW, 46, 10, 3);
    if (strcmp(topic, TOPICTEMP) == 0) {
        strcpy(textBuffer, buffer); // Sao chép chuỗi ký tự từ buffer vào textBuffer
        strcat(textBuffer, "C");
        tft.fillRect(50, 75, 200, 25, ST7735_BLACK); // Xóa nội dung cũ
        printText("Temperatur", ST7735_WHITE, 2, 50, 2);
        printText(textBuffer, ST7735_BLUE, 50, 75, 2);
    } else if (strcmp(topic, TOPICHUM) == 0) {
        strcpy(textBuffer, buffer); // Sao chép chuỗi ký tự từ buffer vào textBuffer
        strcat(textBuffer, "%");
        tft.fillRect(50, 125, 100, 30, ST7735_BLACK); // Xóa nội dung cũ
        printText("Humidity", ST7735_WHITE, 20, 100, 2);
        printText(textBuffer, ST7735_BLUE, 50, 125, 2);
    }
}

void printText(char*text, uint16_t color, int x, int y,int textSize)
{
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(textSize);
  tft.setTextWrap(true);
  tft.print(text);
}

void loop()
{
    mqttClient.loop();
}