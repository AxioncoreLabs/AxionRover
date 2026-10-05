// 电机 A — 有符号计数; FWD/REV 与 MotorOpenLoopTest 一致
// 若 fwd_delta≈0: 先查 12V/VM, 再单独试 count++(见下方注释)
// 若 FWD 时 count 减小: onEncA 里 ++ / -- 对调

const int PWMA = 25, AIN1 = 26, AIN2 = 27, STBY = 33;
const int ENC_A = 18, ENC_B = 19;

volatile int32_t count = 0;

// ISR 放到 IRAM, 取指令快、稳, 不丢脉冲
void IRAM_ATTR onEncA() {
  // ESP32 中断里用 GPIO 寄存器读 B, 判断该脚当前低或高
  if ((GPIO.in >> ENC_B) & 1) {
    count++;
  } else {
    count--;
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);

  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENC_A), onEncA, RISING);

  Serial.println("EncoderTest_A signed; need 12V ON + VM for fwd_delta>0");
}

void loop() {
  noInterrupts();
  int32_t c0 = count;
  interrupts();

  Serial.println("FWD 2s");
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, 160);
  delay(2000);

  noInterrupts();
  int32_t c = count;
  interrupts();
  Serial.print("  count=");
  Serial.print(c);
  Serial.print("  fwd_delta=");
  Serial.println(c - c0);

  Serial.println("STOP 1s");
  analogWrite(PWMA, 0);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  delay(1000);

  noInterrupts();
  c0 = count;
  interrupts();

  Serial.println("REV 2s");
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 160);
  delay(2000);

  noInterrupts();
  c = count;
  interrupts();
  Serial.print("  count=");
  Serial.print(c);
  Serial.print("  rev_delta=");
  Serial.println(c - c0);

  analogWrite(PWMA, 0);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  delay(1000);

  delay(500);
}
