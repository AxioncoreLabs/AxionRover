#define LED_BUILTIN 2

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  delay(1000);
  Serial.println();
  Serial.println("ESP32 Serial OK - 115200");
}

void loop() {
  static unsigned long last = 0;
  if (millis() - last >= 1000) {
    last = millis();
    Serial.print("uptime ms: ");
    Serial.println(millis());
  }
}
