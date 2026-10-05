// 电机 B — 有符号计数; 与 EncoderTest_A 相同打印格式
// B路: 32/14/12/33; 编码器 A=21 B=22
// 若 |fwd_delta| 明显小于 A 且转速看起来一样: 对调绿/黄或 onEncA 里 ++/-- 对调

const int PWMB = 32, BIN1 = 14, BIN2 = 12, STBY = 33;
const int ENC_A = 21, ENC_B = 22;

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

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);

  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENC_A), onEncA, RISING);

  Serial.println("EncoderTest_B signed; compare |fwd_delta| with motor A");
}

void loop() {
  noInterrupts();
  int32_t c0 = count;
  interrupts();

  Serial.println("FWD 2s");
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, 160);
  delay(2000);

  noInterrupts();
  int32_t c = count;
  interrupts();
  Serial.print("  count=");
  Serial.print(c);
  Serial.print("  fwd_delta=");
  Serial.println(c - c0);

  Serial.println("STOP 1s");
  analogWrite(PWMB, 0);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  delay(1000);

  noInterrupts();
  c0 = count;
  interrupts();

  Serial.println("REV 2s");
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 160);
  delay(2000);

  noInterrupts();
  c = count;
  interrupts();
  Serial.print("  count=");
  Serial.print(c);
  Serial.print("  rev_delta=");
  Serial.println(c - c0);

  analogWrite(PWMB, 0);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  delay(1000);

  delay(500);
}
