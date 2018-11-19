#include <Arduino.h>
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

const unsigned int light = 3; //PWM Pin
int prev_bri = 100;
int prev_msg = 0;

void fade_in(int start, int stopv);
void fade_out(int start, int stopv);

void setup() {
  Serial.begin(9600);
  Serial.println("device started");

  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
  pinMode(light, OUTPUT);
  analogWrite(light, 0);

  Serial.println("device ready");
}

void loop() {
    int value = 0;
    //int pwm = 0;


    if (mySwitch.available()) {

    int msg = mySwitch.getReceivedValue();
    int code = msg - 23000;
    int rec_bri = 0;

    Serial.println("");
    Serial.print("message received: ");
    Serial.println(msg);
    Serial.print("extracted code: ");
    Serial.println(code);

  if (!(msg == prev_msg) && (msg > 23000) && !(code == 40)){  //40 indicates issues (dont know why), need to be filtered! transformation in transmitter!
    if ((code <= 100) && (code > 0)){
      rec_bri = prev_bri;
      prev_bri = code;
      value = code;
    }
    else if (code == 0){
      value = code;
      prev_bri = 100;
    }
    else if (code == 600){
      value = prev_bri;
      fade_in(0, value);
    }
    else if (code == 500){
      value = 0;
      fade_out(prev_bri, 0);
    }


    if((code >= 0) && (code <= 100)){
      //pwm = map(value, 0, 100, 0, 255);
      //analogWrite(light, pwm);


      if (value > rec_bri){
        fade_in(rec_bri, value);
      }
      else{
        fade_out(rec_bri, value);
      }



    }
      prev_msg = msg;
      delay(100);
   }
   else{
    Serial.println("received multiple times, wrong format or '40'");
   }

  }


    mySwitch.resetAvailable();

}


void fade_in(int start, int stopv){
  int start_pwm = map(start, 0, 100, 0, 255);
  int stop_pwm = map(stopv, 0, 100, 0, 255);

  for (int i = start_pwm; i <= stop_pwm; i++){
    analogWrite(light, i);
    delay(1);
  }
  Serial.print("brightness set to: ");
  Serial.println(stopv);
}

void fade_out(int start, int stopv){
  int  start_pwm = map(start, 0, 100, 0, 255);
  int  stop_pwm = map(stopv, 0, 100, 0, 255);

  for (int i = start_pwm; i >= stop_pwm; i--){
    analogWrite(light, i);
    delay(1);
  }

  Serial.print("brightness set to: ");
  Serial.println(stopv);
}
