#include <Arduino.h>
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

const unsigned int light = 3; //PWM Pin
unsigned rec_bri = 0;
unsigned prev_brightness = 0;
unsigned long timestamp = 0;

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
    if (mySwitch.available()) {

      unsigned long msg = mySwitch.getReceivedValue();
      unsigned long brightness = msg - 23000;

      Serial.println("");
      Serial.print("message received: ");
      Serial.println(msg);
      Serial.print("extracted brightness: ");
      Serial.println(brightness);

      if ((!(brightness == prev_brightness) || (millis() - timestamp > 1000)) && (brightness >= 0) && (brightness <= 100) && !(brightness == 40)){  //40 indicates issues (dont know why), need to be filtered! transformation in transmitter!
        timestamp = millis();
        if (brightness > rec_bri){
          fade_in(rec_bri, brightness);
        }
        else{
          fade_out(rec_bri, brightness);
        }
          prev_brightness = brightness;
          rec_bri = brightness;
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
