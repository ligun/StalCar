#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MotorDriver.h>

#define LEFT_MOTOR1 2
#define LEFT_MOTOR2 3
#define LEFT_PWM 10
#define RIGHT_MOTOR1 4
#define RIGHT_MOTOR2 5
#define RIGHT_PWM 11

//MotorDriver *left,*right;
MotorDriver left(LEFT_MOTOR1,LEFT_MOTOR2,LEFT_PWM),right(RIGHT_MOTOR1,RIGHT_MOTOR2,RIGHT_PWM);

void setup(){
  pinMode(8,INPUT);
  
  Serial.begin(9600);
}

void loop(){
  if(digitalRead(8)){
    mode1();
    Serial.println("mode1");
  }
  //else mode2();
  else mode3();
}

void mode1(){
  float distance = measureDistance(14,15); 
  if(distance > 50){
    left.forward();
    right.forward();
  }else{
    left.forward();
    right.stop();
  }
  Serial.println(distance);
  delay(1);
}

void mode2(){
  float distance = measureDistance(14,15);
  if(distance > 30){
    left.forward();
    right.forward();
  }else if(distance >20){
    left.stop();
    right.stop();
  }else{
    left.brake();
    right.brake();
  }
  Serial.println(distance);
}

void mode3(){
  char buf[64];
  read(buf,64);
  Serial.println(buf);
  parse(buf);
}

int read(char *buf,int size){
  int length,i = 0;
  while(1){
    if(Serial.available()){
        buf[i] = Serial.read();
        if(buf[i] == '\n')break;
        ++i;
    }
  }
  buf[i] = '\0';
  return 0;
}

int parse(char *buf){
  char *motor,*muki,*speed_s;
  int speed;
  motor = strtok(buf,",");
  muki = strtok(NULL,",");
  speed_s = strtok(NULL,",");
  speed = atoi(speed_s);
  
  Serial.print("motor:");
  Serial.println(motor);
  Serial.print("muki:");
  Serial.println(muki);
  Serial.print("speed:");
  Serial.println(speed);
  
  if(strcmp(motor,"left") == 0){
    if(strcmp(muki,"forward") == 0)left.forward(speed);
    if(strcmp(muki,"reverse") == 0)left.reverse(speed);
    if(strcmp(muki,"stop") == 0)left.stop();
    if(strcmp(muki,"brake") == 0)left.brake();
  }
  else if(strcmp(motor,"right") == 0){
    if(strcmp(muki,"forward") == 0)right.forward(speed);
    if(strcmp(muki,"reverse") == 0)right.reverse(speed);
    if(strcmp(muki,"stop") == 0)right.stop();
    if(strcmp(muki,"brake") == 0)right.brake();
  }
  
  return 0;
}
