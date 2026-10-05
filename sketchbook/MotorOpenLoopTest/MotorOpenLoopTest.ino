// TB6612 motor A — open-loop
// ESP32: PWMA=25, AIN1=26, AIN2=27, STBY=33

const int PWMA = 25;
const int AIN1 = 26;
const int AIN2 = 27;
const int STBY = 33;

void setup() {
  Serial.begin(115200);
  delay(300);

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH);

  Serial.println();
  Serial.println("Open-loop: stage A OK if you see FWD/STOP/REV (no VM = no spin)");
}

void loop() {
  Serial.println("FWD 2s");
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, 160);
  delay(2000);

  Serial.println("STOP 1s");
  analogWrite(PWMA, 0);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  delay(1000);

  Serial.println("REV 2s");
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 160);
  delay(2000);

  analogWrite(PWMA, 0);
  delay(3000);
}
