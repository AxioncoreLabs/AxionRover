// PID差速 + 串口协议 + 通信超时停车
// 115200, 一行一条, Newline 结束。装车后改 WHEELBASE; 某侧反了改 SIGN_A/SIGN_B=-1
// ROS2 周期 cmd_vel 时把 CMD_TIMEOUT_MS 改为 300～500

const int PWMA = 25, AIN1 = 26, AIN2 = 27;
const int PWMB = 32, BIN1 = 14, BIN2 = 12;
const int STBY = 33;
const int ENC_A_A = 18, ENC_A_B = 19;
const int ENC_B_A = 21, ENC_B_B = 22;

const int COUNTS_PER_REV = 330;
const uint32_t DT_MS = 80;
const uint32_t CMD_TIMEOUT_MS = 2000;

const float WHEEL_R = 0.0325f;
float WHEELBASE = 0.159f;  // 外边距185mm(测量) − 轮宽26mm(官方)
int SIGN_A = 1;
int SIGN_B = -1;

float Kff = 1.15f, Kp = 1.0f, Ki = 0.8f, integMax = 180.0f;

volatile int32_t countA = 0, countB = 0;

float tgtA = 0, tgtB = 0;
float integA = 0, integB = 0;
float rpmA = 0, rpmB = 0;
int pwmA = 0, pwmB = 0;

uint32_t lastMotionCmdMs = 0;
bool motionActive = false;
bool distMode = false;
float distRemain = 0;
bool yawMode = false;
float yawRemain = 0;

void IRAM_ATTR onEncA() {
  if ((GPIO.in >> ENC_A_B) & 1) countA++;
  else countA--;
}

void IRAM_ATTR onEncB() {
  if ((GPIO.in >> ENC_B_B) & 1) countB++;
  else countB--;
}

void applyA(int pwm) {
  if (pwm > 255) pwm = 255;
  if (pwm < -255) pwm = -255;
  pwmA = pwm;
  if (pwm == 0) {
    analogWrite(PWMA, 0);
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    return;
  }
  if (pwm > 0) {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    analogWrite(PWMA, pwm);
  } else {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    analogWrite(PWMA, -pwm);
  }
}

void applyB(int pwm) {
  if (pwm > 255) pwm = 255;
  if (pwm < -255) pwm = -255;
  pwmB = pwm;
  if (pwm == 0) {
    analogWrite(PWMB, 0);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
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

float rpmFromV(float v_mps) {
  return v_mps / (2.0f * 3.1415926f * WHEEL_R) * 60.0f;
}

void clearNav() {
  distMode = false;
  distRemain = 0;
  yawMode = false;
  yawRemain = 0;
}

void setVW(float v, float w, bool fromMotionCmd) {
  float vL = v - w * (WHEELBASE * 0.5f);
  float vR = v + w * (WHEELBASE * 0.5f);
  float rL = rpmFromV(vL) * SIGN_A;
  float rR = rpmFromV(vR) * SIGN_B;
  if (fabsf(rL) < 8) rL = 0;
  if (fabsf(rR) < 8) rR = 0;
  if (rL > 200) rL = 200;
  if (rL < -200) rL = -200;
  if (rR > 200) rR = 200;
  if (rR < -200) rR = -200;
  tgtA = rL;
  tgtB = rR;
  integA = integB = 0;
  motionActive = (fabsf(tgtA) >= 8.0f) || (fabsf(tgtB) >= 8.0f);
  if (fromMotionCmd) {
    lastMotionCmdMs = millis();
  }
  Serial.print("OK v=");
  Serial.print(v, 3);
  Serial.print(" w=");
  Serial.print(w, 3);
  Serial.print(" rpmA=");
  Serial.print(tgtA, 1);
  Serial.print(" rpmB=");
  Serial.println(tgtB, 1);
}

int piStep(float tgt, float rpm, float &integ, float dt) {
  if (fabsf(tgt) < 8) {
    integ = 0;
    return 0;
  }
  float err = tgt - rpm;
  integ += err * dt;
  if (integ > integMax) integ = integMax;
  if (integ < -integMax) integ = -integMax;
  float u = Kff * tgt + Kp * err + Ki * integ;
  return (int)(u + (u >= 0 ? 0.5f : -0.5f));
}

void printStatus() {
  Serial.print("A ");
  Serial.print(rpmA, 1);
  Serial.print("/");
  Serial.print(tgtA, 1);
  Serial.print(" B ");
  Serial.print(rpmB, 1);
  Serial.print("/");
  Serial.print(tgtB, 1);
  Serial.print(" p ");
  Serial.print(pwmA);
  Serial.print("/");
  Serial.println(pwmB);
}

void handleLine(String line) {
  line.trim();
  if (line.length() == 0) return;

  if (line == "0") {
    clearNav();
    setVW(0, 0, true);
    Serial.println("OK stop");
    return;
  }
  if (line == "e" || line == "E") {
    printStatus();
    return;
  }
  if (line == "?") {
    Serial.println("cmds: 0 | v | w | c | d <m> [v] | r <deg> [w] | e | ?");
    Serial.println("v/w/c timeout 2s; d/r encoders, no timeout");
    Serial.println("this chassis SIGN_B=-1: r 90 right/CW  r -90 left/CCW");
    return;
  }
  if (line.startsWith("v ")) {
    clearNav();
    setVW(line.substring(2).toFloat(), 0, true);
    return;
  }
  if (line.startsWith("w ")) {
    clearNav();
    setVW(0, line.substring(2).toFloat(), true);
    return;
  }
  if (line.startsWith("d ")) {
    float meters = 0, speed = 0.12f;
    int sp = line.indexOf(' ', 2);
    if (sp > 0) {
      meters = line.substring(2, sp).toFloat();
      speed = line.substring(sp + 1).toFloat();
    } else {
      meters = line.substring(2).toFloat();
    }
    if (meters <= 0 || meters > 5 || speed < 0.05f || speed > 0.25f) {
      Serial.println("NAK d");
      return;
    }
    clearNav();
    distRemain = meters;
    distMode = true;
    setVW(speed, 0, true);
    Serial.print("OK d=");
    Serial.print(meters, 3);
    Serial.print(" v=");
    Serial.println(speed, 3);
    return;
  }
  if (line.startsWith("r ")) {
    float deg = 0, wz = 0.8f;
    int sp = line.indexOf(' ', 2);
    if (sp > 0) {
      deg = line.substring(2, sp).toFloat();
      wz = line.substring(sp + 1).toFloat();
    } else {
      deg = line.substring(2).toFloat();
    }
    if (fabsf(deg) < 5.0f || fabsf(deg) > 400.0f || wz < 0.3f || wz > 1.8f) {
      Serial.println("NAK r");
      return;
    }
    clearNav();
    yawRemain = deg * 3.1415926f / 180.0f;
    yawMode = true;
    float wcmd = (deg >= 0) ? wz : -wz;
    setVW(0, wcmd, true);
    Serial.print("OK r=");
    Serial.print(deg, 1);
    Serial.print(" w=");
    Serial.println(wcmd, 2);
    return;
  }
  if (line.startsWith("c ")) {
    int sp = line.indexOf(' ', 2);
    if (sp > 0) {
      clearNav();
      setVW(line.substring(2, sp).toFloat(), line.substring(sp + 1).toFloat(), true);
      return;
    }
  }
  Serial.println("NAK");
}

void setup() {
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, LOW);
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  applyA(0);
  applyB(0);

  Serial.begin(115200);
  delay(300);

  pinMode(ENC_A_A, INPUT);
  pinMode(ENC_A_B, INPUT);
  pinMode(ENC_B_A, INPUT);
  pinMode(ENC_B_B, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENC_A_A), onEncA, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_B_A), onEncB, RISING);
  digitalWrite(STBY, HIGH);

  Serial.println("DiffDrive proto+timeout. ? for help");
}

void loop() {
  static String line;
  while (Serial.available()) {
    char ch = Serial.read();
    if (ch == '\n' || ch == '\r') {
      handleLine(line);
      line = "";
    } else {
      line += ch;
      if (line.length() > 48) line = "";
    }
  }

  uint32_t now = millis();
  if (motionActive && !distMode && !yawMode && (now - lastMotionCmdMs >= CMD_TIMEOUT_MS)) {
    motionActive = false;
    tgtA = tgtB = 0;
    integA = integB = 0;
    Serial.println("OK timeout stop");
  }

  static uint32_t lastMs = 0;
  static int32_t lastA = 0, lastB = 0;
  if (now - lastMs < DT_MS) return;
  float dt = (now - lastMs) / 1000.0f;
  lastMs = now;

  noInterrupts();
  int32_t a = countA, b = countB;
  interrupts();
  int32_t dA = a - lastA, dB = b - lastB;
  lastA = a;
  lastB = b;

  float instA = (dA / dt) / COUNTS_PER_REV * 60.0f;
  float instB = (dB / dt) / COUNTS_PER_REV * 60.0f;
  rpmA = 0.45f * instA + 0.55f * rpmA;
  rpmB = 0.45f * instB + 0.55f * rpmB;

  if (distMode) {
    float ds = 0.5f * (dA * SIGN_A + dB * SIGN_B) / (float)COUNTS_PER_REV
               * (2.0f * 3.1415926f * WHEEL_R);
    distRemain -= ds;
    if (distRemain <= 0.02f) {
      clearNav();
      tgtA = tgtB = 0;
      integA = integB = 0;
      motionActive = false;
      applyA(0);
      applyB(0);
      Serial.println("OK dist done");
    }
  }

  if (yawMode) {
    const float twoPiR = 2.0f * 3.1415926f * WHEEL_R;
    float dsL = (dA * SIGN_A) / (float)COUNTS_PER_REV * twoPiR;
    float dsR = (dB * SIGN_B) / (float)COUNTS_PER_REV * twoPiR;
    float dYaw = (dsR - dsL) / WHEELBASE;
    yawRemain -= dYaw;
    bool done = (yawRemain > 0) ? (yawRemain <= 0.06f) : (yawRemain >= -0.06f);
    if (done) {
      clearNav();
      tgtA = tgtB = 0;
      integA = integB = 0;
      motionActive = false;
      applyA(0);
      applyB(0);
      Serial.println("OK yaw done");
    }
  }

  applyA(piStep(tgtA, rpmA, integA, dt));
  applyB(piStep(tgtB, rpmB, integB, dt));
}
