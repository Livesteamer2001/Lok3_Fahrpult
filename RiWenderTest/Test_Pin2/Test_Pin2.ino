#define SLEEP 2


void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  digitalWrite(SLEEP, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
   delay(5000);
   digitalWrite(SLEEP, HIGH);
}
