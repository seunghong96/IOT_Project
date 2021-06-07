#define D1 5
#define D2 4
#define D0 0

void setup() {
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D0, OUTPUT);
}

void loop() 
{
  analogWrite(D0,1000);  
  digitalWrite(D1,HIGH);
  digitalWrite(D2,LOW);
  delay(1000);
  digitalWrite(D1,LOW);
  digitalWrite(D2,HIGH);
  delay(1000);
}
