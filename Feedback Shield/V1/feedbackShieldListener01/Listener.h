class Listener {
    int pin;
    int pinState = 0;
    int lastPinState = 1;


  public:
    Listener(int _pin) {
      pin = _pin;
    }


    void setupPins() {
      pinMode(pin, INPUT);
    }

    int readPins() {

     pinState = digitalRead(pin);
     // Serial.println(pinState);
      return pinState;
    }

    boolean stateChange() {
      if (pinState != lastPinState) { //has it changed from last time? Avoid repeated pushes.
        //Serial.println(pinState);
        return true;
      }

      return false;//reset
    }

    boolean pinHigh() {

      if (stateChange() && pinState == 1) { //has it changed from last time is it LOW? / ON?
        lastPinState = pinState; //update lastPinState

        return true; //set pushed to be true.
      } else {

        return false; //otherwise  pushed is false.
      }
    }

    boolean pinLow() {
      if (stateChange() && pinState == 0) { //has it changed from last time is it HIGH? / OFF?
        lastPinState = pinState; //update lastButtonState

        return true; //set pushed to be true.
      } else {

        return false; //otherwise  pushed is false.
      }
    }







}; ////end of class/////////
