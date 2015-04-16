#include "mbed.h"
#include "Adafruit_PWMServoDriver.h"

Adafruit_PWMServoDriver pwm(p28, p27);

void setServoPulse(uint8_t n, float pulse) {
    float pulselength = 10000;   // 10,000 us per second
    pulse = 4094 * pulse / pulselength;
    pwm.setPWM(n, 0, pulse);
}

void initServoDriver() {
    pwm.begin();
    //pwm.setPWMFreq(100);  //This dosen't work well because of uncertain clock speed. Use setPrescale().
    pwm.setPrescale(128);    //This value is decided for 10ms interval.
    pwm.setI2Cfreq(400000); //400kHz
}

void set_servo(int n, int i) {
  if(i == 0) {
      setServoPulse(n, 150);
  } else if(i == 90) {
     setServoPulse(n, 500); 
  } else {
      setServoPulse(n, 1000);
  }
}

int main() {
    //pwm.i2c_probe();
    initServoDriver();
    /*
    int i = 0;
    while(1){
        set_servo(0, i);
        set_servo(1, 180-i);
        set_servo(14, i);
        set_servo(15, 180-i);
        
        wait_ms(750);
        i+=90;
        if (i > 180) i = 0;
    }
    */
    set_servo(0, 180);
    set_servo(1, 180);
    
    wait(1.0);
    set_servo(0, 90);
    wait(1.0);
    set_servo(0, 180);
    wait(1.0);
    set_servo(1, 90);
    wait(1.0);
    set_servo(1, 180);
    while(1){}
    
}
