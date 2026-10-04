# Day 1: Physical Computing 101

Today we will explore the foundations of working with Arduino. We will get started looking at simple outputs like a light (LED) and finish with exploring how we control the brightness of the light.

> **Before you start:** we need to download the Arduino software to work with.
>
>
> [download Arduino software](https://www.arduino.cc/en/software/#ide)

## What you'll need

- An Arduino Leonardo or UNO and a USB cable
- One LED
- A potentiometer
- a mini breadboard
- A handful of jumper wires

## Task 1: Make it blink

Wire an LED into pin 13.

- LED stands for Light Emitting Diode. It emits light when current flows through it.
- The longer leg is **positive + and it** is attached to a resistor
- The other end of the resistor goes into pin 13.
- The resistor limits the current through the LED and prevents excess current that can burn out the LED
- The shorter leg goes into GND it is **negative -***(notice there is a GND pin next to pin 13)*

![Wiring the LED to pin 13](/worksheet/led.png)

> **Next:** we need to plug the board in, open the Arduino IDE, and select your
> board under **Tools → Board**.
> Then we need to select the port under **Tools → Port**
> Once we have the board connected we can verify and upload the code.

Wire the LED to pin 13 and GND, then upload this:

```arduino
int ledPin = 13;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  digitalWrite(ledPin, HIGH);
  digitalWrite(ledPin, LOW);
}
```
> **What's happening in the code?!**
>
> We are sending signals to the LED via the code.
>
> **digitalWrite()** means send a digital message **out** to the LED.
>
> **HIGH** means **on** and **LOW** means **off**

Why do you think we can't see the LED turn on and off?

Often when we work with physical outputs and computing we need to allow the physical component enough time to make the change we are requesting.

The LED is actually turning on and off but its happening so quickly our human eyes cannot see it!

Add these lines to the code and upload again to see it come to life.

```arduino
int ledPin = 13;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  digitalWrite(ledPin, HIGH);
  delay(1000); // wait time 1000 milliseconds = 1 second
  digitalWrite(ledPin, LOW);
  delay(1000); // wait time 1000 milliseconds = 1 second
}
```

Change the two `delay(500)` values and watch what happens. Can you make it
blink twice as fast?

> **Sometimes it can be hard** to troubleshoot why something isn't working.
>
> The problem might be in your code, or your circuit or the relationship between the two.
>
> We are going to work with what's called a **shield** to help s visualise this.
>
> A shield allows you to plug your components into it and allow the electricity to pass through to the shield.
>
> Shields are often built to make complex tasks and electronic circuitry easier.

## Feedback Shield

```shield
```

Put your shield on top of your Arduino and turn it on at the little switch.
Place your LED back into the shield at pins 13 and GND.
**Do you see your LED light up? What else do you see on the shield?**

> **Feedback:** [tell us how Task 1 went](/survey/1/task1) before moving on.

## Task 2: Make the LED fade
When we want to make something turn on we use ```HIGH``` and when we want something to tur off we se ```LOW```. These commands work with digital signals and are binary. There is no in-between state.

However, we can also send varied amounts of 'On' to a pin by sending a number value instead of ```HIGH``` or ```LOW```.

In order to do this we use the ```analogWrite()``` function instead of ```digitalWrite()```. When we use ```analogWrite()``` we are able to send a **pulse width modulation (PWM)** signal.

Try upload this code to your Arduino and observe what you see.

```arduino
void setup() {
  pinMode(9,OUTPUT);
}

void loop() {
  analogWrite(9, 255)
  delay(30);
  analogWrite(9, 155)
  delay(30);
  analogWrite(9, 55)
  delay(30);
  analogWrite(9, 0)
  delay(30);
}
```
> What is happening to your physical LED?
>
> What is happening on the shield?

Try moving your LED and re-uploading the code.

![Wiring the LED to pin 11 (a PWM pin)](/worksheet/pwm.png)

*intermediatte tasks*

We can also write our code in a more efficient way. Writing each command line by line is ok and it works but for long animated behaviour that changes over time its a lot of code!

You can use something called a ```for loop``` to iterate over a value and increment each time.

Try uploading this code and changing some of the values. How does the behaviour of the light change?

```arduino
int ledPin = 11;  // LED connected to digital pin 11

void setup() {
  // nothing happens in setup
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // fade in from min to max in increments of 5 points:
  for (int fadeValue = 0; fadeValue <= 255; fadeValue += 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(60);
  }

  // fade out from max to min in increments of 5 points:
  for (int fadeValue = 255; fadeValue >= 0; fadeValue -= 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(60);
  }
}
```
> **Feedback:** [tell us how Task 2 went](/survey/1/task2) before moving on.

## Task 3: Control the LED

So far we have controlled the LED by coding fixed values or values that change as the computer counts.

We can also control an output with a physical input like a sensor or a dial. We are going to start by looking at dials or potentiometers as they are known.

![Wire a potentiometer to A0](/worksheet/dial.png)

Connect the potentiometer's middle pin to `A0`. Upload the code and then open the serial monitor
(**Tools → Serial Monitor**) at 9600 baud.

```arduino
void setup() {
  Serial.begin(9600);
}

void loop() {
  int value = analogRead(A0);   // 0–1023
  Serial.println(value);
  delay(100);
}
```

Turn the knob slowly. The numbers should sweep from `0` to `1023`.

> Notice how we are now using ```analogRead()``` not ```write```.
>
> Think of the analogy -> Read to absorb and take **in** , Write to make a mark and put **out** in the world.

We can connect the dial to the LED to control it!
![Wire a potentiometer to an LED](/worksheet/dial-led.png)

Upload the code and observe what is happening.

```arduino
void setup() {
  Serial.begin(9600);
  pinMode(11, OUTPUT);
}

void loop() {
  int value = analogRead(A0);   // 0–1023
  analogWrite(11,value);
  Serial.println(value);
  delay(100);
}
```
> How is the physical LED behaving?
>
> What is happning on the shield?

If we rememeber when we sent values to the LED with ```analogWrite()``` the range was 0 - 255. However, when we used ```analogRead()``` and ```serialPrint()``` to view the values from the dial we saw 0 - 1023. These values don't align very well. If we turn the dial we quickly go past 255 (full brightness).

In order to make the behaviour of turning the dial align more closely with the brightness of an LED we need use something called ```map()```. This allows us to **map** one value range to another value range.

Try upload this code and observe the difference.

```arduino
void setup() {
  Serial.begin(9600);
  pinMode(11, OUTPUT);
}

void loop() {
  int value = analogRead(A0);   // 0–1023
  int brightness = map(value, 0,1023,0,255); //map one range to another range
  analogWrite(11,brightness);
  Serial.println(value);
  Serial.println(brightness);
  delay(100);
}
```
> **Feedback:** [tell us how Task 3 went](/survey/1/task3) before moving on.

## Day 1 complete!
We have finished for the day. Tomorrow we will look at how to connect our dial to creating graphics and making sounds!

Please fill in the last few form feedback questions for the day. Open the **[feedback for today](/survey/1/exit)** . You don't need to use technical langage, you can answer in your own words.

## Reference

- [Arduino `pinMode()` docs](https://docs.arduino.cc/language-reference/en/functions/digital-io/pinMode/)
- [Arduino `digitalWrite()` docs](https://docs.arduino.cc/language-reference/en/functions/digital-io/digitalwrite/)
- [Arduino `analogWrite()` docs](https://docs.arduino.cc/language-reference/en/functions/analog-io/analogWrite/)
- [Arduino PWM docs](https://docs.arduino.cc/learn/microcontrollers/analog-output/)
- [Arduino `analogRead()` docs](https://docs.arduino.cc/language-reference/en/functions/analog-io/analogRead/)
- [Arduino `map()` docs](https://docs.arduino.cc/language-reference/en/functions/math/map/)



