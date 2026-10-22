#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#define PIN_RED_1   32
#define PIN_GREEN_1 33
#define PIN_BLUE_1  25

#define PIN_RED_2   14
#define PIN_GREEN_2 27
#define PIN_BLUE_2  26

#define ESPNOW_WIFI_CHANNEL 6

String success;

typedef struct struct_message {
  bool key_one;
  bool key_two;
  bool key_three;
  bool key_four;
} struct_message;

struct_message incomingReadings;

volatile bool key_one = false;
volatile bool key_two = false;
volatile bool key_three = false;
volatile bool key_four = false;

// only need this on the reciever
// Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Successful" : "Delivery Failed");
// if (status == 0) {
//   success = ":)";
// } else {
//   success = ":(";
// }

void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  if (len != sizeof(incomingReadings)) return;
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));

  key_one   = incomingReadings.key_one;
  key_two   = incomingReadings.key_two;
  key_three = incomingReadings.key_three;
  key_four = incomingReadings.key_four;
}

void setColor(int R, int G, int B) {
  analogWrite(PIN_RED_1,   R);
  analogWrite(PIN_GREEN_1, G);
  analogWrite(PIN_BLUE_1,  B);
  analogWrite(PIN_RED_2,   R);
  analogWrite(PIN_GREEN_2, G);
  analogWrite(PIN_BLUE_2,  B);
}

void setup() {
  setCpuFrequencyMhz(80);
  // Serial.begin(115200);

  pinMode(PIN_RED_1, OUTPUT);
  pinMode(PIN_GREEN_1, OUTPUT);
  pinMode(PIN_BLUE_1, OUTPUT);
  pinMode(PIN_RED_2, OUTPUT);
  pinMode(PIN_GREEN_2, OUTPUT);
  pinMode(PIN_BLUE_2, OUTPUT);

  WiFi.mode(WIFI_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(ESPNOW_WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  if (esp_now_init() != ESP_OK) return;
  esp_wifi_set_ps(WIFI_PS_MIN_MODEM); // enable power save
  esp_now_set_wake_window(20); // listen for 20 ms
  esp_wifi_connectionless_module_set_wake_interval(200); // wake every 100ms

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  if (key_one)        setColor(255, 0, 0);
  else if (key_two)   setColor(255, 70, 0);
  else if (key_three) setColor(0, 255, 0);
  else if (key_four)  setColor(0, 20, 255);
  else                setColor(0, 0, 0);
  delay(10);
}
