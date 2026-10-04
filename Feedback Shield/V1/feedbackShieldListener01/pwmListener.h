class PWMListener {
    int pin;
    float pinVal;
    unsigned long duration;
    int duty;
    float freq, period;


  public:
    PWMListener(int _pin) {
      pin = _pin;
    }


    void setupPWMPins() {
      pinMode(pin, INPUT);
    }


    //measure PWM pins
    int readPWMpins() {
      pinVal = analogRead(pin);
      duty = map(pinVal, 0, 1023, 0, 255);
      return duty;
    }



}; ////end of class/////////
