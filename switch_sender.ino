#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "driver/rtc_io.h"

#define PIN_KEY_ONE   32
#define PIN_KEY_TWO   33
#define PIN_KEY_THREE 25
#define PIN_KEY_FOUR  26

// // bitmask for all four pins
// #define BUTTON_PIN_BITMASK ((1ULL<<PIN_KEY_ONE | 1ULL<<PIN_KEY_TWO | 1ULL<<PIN_KEY_THREE | 1ULL<<PIN_KEY_FOUR)) 

#define WAKE_PIN GPIO_NUM_32

#define ESPNOW_WIFI_CHANNEL 6

// receiver's MAC address
uint8_t receiverAddress[] = {0x58, 0x2A, 0xBD, 0x7C, 0x95, 0x34};

typedef struct struct_message {
  bool key_one;
  bool key_two;
  bool key_three;
  bool key_four;
} struct_message;

struct_message outgoing;
struct_message lastSent;

esp_now_peer_info_t peerInfo;

unsigned long lastActivity = 0;

void sendState() {
  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)&outgoing, sizeof(outgoing));
  // if (result != ESP_OK) {
  //   Serial.println("Send error");
  // }
  lastSent = outgoing;
}

void sendStateBurst() {
  // resend (240ms interval)
  for (int i = 0; i < 24; i++) {
    esp_now_send(receiverAddress, (uint8_t *)&outgoing, sizeof(outgoing));
    delay(10);
  }
  lastSent = outgoing;
}

void deepSleep() {
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
  esp_sleep_enable_ext0_wakeup(WAKE_PIN, 0); // wake when pin 32 goes low
  rtc_gpio_pullup_en(WAKE_PIN); // stay high when in sleep
  rtc_gpio_pulldown_dis(WAKE_PIN); // disable internal pull-down
  esp_deep_sleep_start(); 
}

void setup() { 
  setCpuFrequencyMhz(80);
  // Serial.begin(115200);

  // pressed = LOW
  pinMode(PIN_KEY_ONE, INPUT_PULLUP);
  pinMode(PIN_KEY_TWO, INPUT_PULLUP);
  pinMode(PIN_KEY_THREE, INPUT_PULLUP);
  pinMode(PIN_KEY_FOUR, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(ESPNOW_WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  if (esp_now_init() != ESP_OK) return;

  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = ESPNOW_WIFI_CHANNEL;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) return;

  // Send the initial state so the receiver starts in sync
  outgoing.key_one   = !digitalRead(PIN_KEY_ONE);
  outgoing.key_two   = !digitalRead(PIN_KEY_TWO);
  outgoing.key_three = !digitalRead(PIN_KEY_THREE);
  outgoing.key_four  = !digitalRead(PIN_KEY_FOUR);

  sendStateBurst();
  lastActivity = millis();
}

void loop() {
  // read keys
  outgoing.key_one   = !digitalRead(PIN_KEY_ONE);
  outgoing.key_two   = !digitalRead(PIN_KEY_TWO);
  outgoing.key_three = !digitalRead(PIN_KEY_THREE);
  outgoing.key_four  = !digitalRead(PIN_KEY_FOUR);

  // send if something changed
  if (memcmp(&outgoing, &lastSent, sizeof(outgoing)) != 0 )sendStateBurst();

  bool activity = outgoing.key_one || outgoing.key_two || outgoing.key_three || outgoing.key_four;
  if (activity) lastActivity = millis();

  if (!activity && millis() - lastActivity > 2000) { // if no activity and there's been no activity for > 2000ms
    sendStateBurst(); delay(50); // send current state and wait for ESPNOW to send
    sendStateBurst(); delay(50); // repeat in case of dropped packet 
    deepSleep(); // go back to deep sleep
  }

  delay(20);  // debounce
}
