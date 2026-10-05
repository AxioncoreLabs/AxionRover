// TB6612 motors A+B — open-loop
// A: 25/26/27  B: 32/14/12  STBY: 33

const int PWMA = 25, AIN1 = 26, AIN2 = 27;
const int PWMB = 32, BIN1 = 14, BIN2 = 12;
const int STBY = 33;

void applyA(int pwm) {
  if (pwm > 0) {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    analogWrite(PWMA, pwm);
  } else if (pwm < 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    analogWrite(PWMA, -pwm);
  } else {
    analogWrite(PWMA, 0);
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
  }
}

void applyB(int pwm) {
  if (pwm > 0) {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    analogWrite(PWMB, pwm);
  } else if (pwm < 0) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMB, -pwm);
  } else {
    analogWrite(PWMB, 0);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  applyA(0);
  applyB(0);
  digitalWrite(STBY, HIGH);
  Serial.println("Open-loop AB: FWD/STOP/REV both motors");
}

void loop() {
  Serial.println("FWD 2s");
  applyA(160);
  applyB(160);
  delay(2000);

  Serial.println("STOP 1s");
  applyA(0);
  applyB(0);
  delay(1000);

  Serial.println("REV 2s");
  applyA(-160);
  applyB(-160);
  delay(2000);

  applyA(0);
  applyB(0);
  delay(3000);
}
