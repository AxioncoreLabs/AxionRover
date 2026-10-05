// 电机 B 速度 PI — 与 SpeedPI_A 同参数, 只 init B 路
// PWMB=32 BIN1=14 BIN2=12 STBY=33; 编码器 21/22

const int PWMB = 32, BIN1 = 14, BIN2 = 12, STBY = 33;
const int ENC_A = 21, ENC_B = 22;

const int COUNTS_PER_REV = 330;
const uint32_t DT_MS = 80;

float Kff = 1.15f;
float Kp = 1.0f;
float Ki = 0.8f;
float integMax = 180.0f;

volatile int32_t count = 0;

float targetRpm = 0.0f;
float integ = 0.0f;
float rpmFilt = 0.0f;
int pwmCmd = 0;

void IRAM_ATTR onEncA() {
  if ((GPIO.in >> ENC_B) & 1) {
    count++;
  } else {
    count--;
  }
}

void stopB() {
  analogWrite(PWMB, 0);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  pwmCmd = 0;
}

void applyPwm(int pwm) {
  if (pwm > 255) pwm = 255;
  if (pwm < -255) pwm = -255;
  pwmCmd = pwm;
  if (pwm == 0) {
    stopB();
    return;
  }
  if (pwm > 0) {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    analogWrite(PWMB, pwm);
  } else {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMB, -pwm);
  }
}

void setup() {
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, LOW);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  stopB();

  Serial.begin(115200);
  delay(300);

  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENC_A), onEncA, RISING);
  digitalWrite(STBY, HIGH);

  Serial.println("SpeedPI_B same gains as A. type RPM, 0=stop");
}

void loop() {
  static String line;
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (line.length() > 0) {
        targetRpm = line.toFloat();
        if (targetRpm > 250) targetRpm = 250;
        if (targetRpm < -250) targetRpm = -250;
        integ = 0;
        Serial.print("targetRpm=");
        Serial.println(targetRpm, 1);
        line = "";
      }
    } else {
      line += c;
    }
  }

  static uint32_t lastMs = 0;
  static int32_t lastCount = 0;
  uint32_t now = millis();
  if (now - lastMs < DT_MS) {
    return;
  }
  float dt = (now - lastMs) / 1000.0f;
  lastMs = now;

  noInterrupts();
  int32_t c = count;
  interrupts();
  int32_t d = c - lastCount;
  lastCount = c;

  float rpm = (d / dt) / COUNTS_PER_REV * 60.0f;
  rpmFilt = 0.45f * rpm + 0.55f * rpmFilt;
  float err = targetRpm - rpmFilt;

  if (fabsf(targetRpm) < 8.0f) {
    integ = 0;
    rpmFilt = 0;
    applyPwm(0);
  } else {
    integ += err * dt;
    if (integ > integMax) integ = integMax;
    if (integ < -integMax) integ = -integMax;

    float u = Kff * targetRpm + Kp * err + Ki * integ;
    int pwm = (int)(u + (u >= 0 ? 0.5f : -0.5f));
    applyPwm(pwm);
  }

  Serial.print("rpm=");
  Serial.print(rpmFilt, 1);
  Serial.print(" tgt=");
  Serial.print(targetRpm, 1);
  Serial.print(" pwm=");
  Serial.print(pwmCmd);
  Serial.print(" e=");
  Serial.println(err, 1);
}
