// countsPerRev 标定 — 12V OFF, USB 供电, 不驱动电机
// 标电机B(21/22): 改下面 CAL_A 为 false
// 操作: Serial 输入 z 回车清零 → 输出轴标记转 1 整圈 → 读 count; 或转 10 圈除以 10

#define CAL_A true

#if CAL_A
const int ENC_A = 18, ENC_B = 19;
const char *NAME = "Motor A (18/19)";
#else
const int ENC_A = 21, ENC_B = 22;
const char *NAME = "Motor B (21/22)";
#endif

volatile int32_t count = 0;

void IRAM_ATTR onEncA() {
  if ((GPIO.in >> ENC_B) & 1) {
    count++;
  } else {
    count--;
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);

  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENC_A), onEncA, RISING);

  Serial.println();
  Serial.println("EncoderCal — 12V OFF, hand rotate OUTPUT shaft only");
  Serial.println(NAME);
  Serial.println("z + Enter = zero count; then 1 rev (or 10 rev /10)");
  Serial.println("Use ONE direction per rev; take |delta|");
}

void zeroCount() {
  noInterrupts();
  count = 0;
  interrupts();
  Serial.println("count ZEROED");
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'z' || c == 'Z') {
      zeroCount();
    }
  }

  static uint32_t t = 0;
  if (millis() - t >= 300) {
    t = millis();
    noInterrupts();
    int32_t c = count;
    interrupts();
    Serial.print("count=");
    Serial.println(c);
  }
}
