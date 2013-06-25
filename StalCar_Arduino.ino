#define LEFT_MOTER1 1
#define LEFT_MOTER2 2
#define RIGHT_MOTER1 3
#define RIGHT_MOTER2 4

void setup(){
  pinMode(LEFT_MOTER1,OUTPUT);
  pinMode(LEFT_MOTER2,OUTPUT);
  pinMode(RIGHT_MOTER1,OUTPUT);
  pinMode(RIGHT_MOTER2,OUTPUT);
  
  randomSeed(analogRead(0));
}

void loop(){
  int hoge = random(3);
  switch(hoge){
    case 0:
    digitalWrite(LEFT_MOTER1,HIGH);
    digitalWrite(LEFT_MOTER2,LOW);
    digitalWrite(RIGHT_MOTER1,HIGH);
    digitalWrite(RIGHT_MOTER2,LOW);
    break;
    case 1:
    digitalWrite(LEFT_MOTER1,HIGH);
    digitalWrite(LEFT_MOTER2,LOW);
    digitalWrite(RIGHT_MOTER1,LOW);
    digitalWrite(RIGHT_MOTER2,LOW);
    break;
    case 2:
    digitalWrite(LEFT_MOTER1,LOW);
    digitalWrite(LEFT_MOTER2,LOW);
    digitalWrite(RIGHT_MOTER1,HIGH);
    digitalWrite(RIGHT_MOTER2,LOW);
    break;
    /*
    case 3:
    digitalWrite(LEFT_MOTER1,LOW);
    digitalWrite(LEFT_MOTER2,LOW);
    digitalWrite(RIGHT_MOTER1,LOW);
    digitalWrite(RIGHT_MOTER2,LOW);
    break;
    */
  }
  delay(5000);
}

