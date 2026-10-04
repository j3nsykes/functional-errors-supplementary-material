# Day 2: Controlling graphics and sound

Building on yesterday: today we will take our dial and use it to control drawings and sounds in [Processing](https://processing.org/)

## Intro to Processing

In order to generate our audio visual elements we will use an open-source tool called **Processing**. Many of the instruments made in SoundPlay are created using Arduino and Processing together. By the end of the day we will aim to create something similar!

> Download Processing for your computer.
>
> [Download](https://processing.org/download)

Processing langauge is very similar to Arduino but has a focus on creating graphics rather than connecting physical components. It has a similar friendly library to get started with drawing to the canvas.

Some tips to begin...

> Open a new sketch
>
> Copy the basic start of the code below

```java
void setup() {
  size(400, 400);
}

void draw() {
  background(240);

}
```

You should see a grey square when you press the run/play icon. This is your canvas.

In order to add some shapes you can refer to the [reference page](https://processing.org/reference) for help and the tools I will give you.

Let's do some together to start...

```java
void setup() {
  size(400, 400);
}

void draw() {
  background(240); //a background colour in greyscale
  ellipse(200,200,50,50); //draws a circle at 200pixels across and 200 pixels down. Its 50 pixels in diameter.

}
```

```java
void setup() {
  size(400, 400);
}

void draw() {
  background(240); //a background colour in greyscale
  fill(255,0,0); // fills the circle in red
  ellipse(200,200,50,50); //draws a circle

}
```

## Task 1: Sculpt and draw some shapes

I have created a little tool called [Sculpting code](https://sculpting-code.vercel.app/) to help you get to know the basics of Processing code (and its sister P5JS).

You can use playDoh or paper and pens to create a drawing. Start simple with a simple shape then explore adding to your drawing or adding a second playdoh shape. Capture your image again and see how the code updates.

Read the highlighted functions that are added
![Sculpting code response](/worksheet/sculpting-code-reply.png)

> **Feedback:** [tell us how Task 1 went](/survey/2/task1) before moving on.

## Task 2: Interactivity

We can make our graphics interactive by using inputs already on our computers.

> Can you name two inputs already on your computer?

Create a new sketch and add this code to see your circle come to life.

```java
void setup() {
  createCanvas(400, 400);
}

void draw() {
  background(240);
  int movement = mouseX;
  ellipse(width / 2, height / 2, movement, movement);
}
```

How might we make the range of the circle size a little smaller? How can we take the range of the mouseX movement and make it control a smaller range of values?

```java
void setup() {
  createCanvas(400, 400);
}

void draw() {
  background(240);
  int movement = map(mouseX, 0, width, 10, 100);
  ellipse(width / 2, height / 2, movement, movement);
}
```

### Connect the Arduino

We can try and replace the mouse with the dial we connected yesterday.

In order to do this we need some extra code that connects Arduino and Processing together.

On your Arduino upload the ```Firmata``` example code
**File/Examples/Firmata**

In Processing create a new sketch and add this code...
```java
import processing.serial.*;   // Serial library
import cc.arduino.*;          // Arduino (Firmata) library

Arduino arduino;

void setup() {
  size(400, 400);
  printArray(Serial.list());                             // see your ports in the console
  arduino = new Arduino(this, Serial.list()[0], 57600);  // change the [0] to your port's number
}

void draw() {
  background(240);
  int value = arduino.analogRead(0);        // read the potentiometer on A0 (0–1023)
  float diameter = map(value, 0, 1023, 10, 300);  // turn it into a circle size
  fill(30, 120, 240);
  ellipse(width / 2, height / 2, diameter, diameter);
}

```

We need to install a library to make this work. Go to **sketch/import library/manage libraries** and select **Arduino**

In order to connect you Arduino you need to look for the list of deviced printed when you run the sketch and then edit the number in the square bracket.
```
[0] "/dev/cu.usbmodem1101"
[1] "/dev/cu.Bluetooth-Incoming-Port"
```

> **Feedback:** [tell us how Task 2 went](/survey/2/task2) before moving on.

## Task 3: Draw with the dial

Let's try make some more changes with the dial. Here are 4 sketches to try and run. Explore making some edits to them. If you see a number in brackets try and change it to see what happens.

> You can also go back to drawing your ideas and changes
>
> Ask sculpting-code to help
>
> Try to make simple changes, one at a time so you understand your code edits. Don't make large edits all at one.

### 01

```java
import processing.serial.*;   // Serial library
import cc.arduino.*;          // Arduino (Firmata) library

Arduino arduino;

void setup() {
  size(400, 400);
  printArray(Serial.list());                              // see your ports in the console
  arduino = new Arduino(this, Serial.list()[0], 57600);  // change [0] to your Arduino port index
}

void draw() {
  background(240);
  int value = arduino.analogRead(0);
  float redAmount = map(value, 0, 1023, 0, 255);  // dial → amount of red
  fill(redAmount, 50, 100);
  ellipse(width / 2, height / 2, 200, 200);
}
```


### 02

```java
import processing.serial.*;   // Serial library
import cc.arduino.*;          // Arduino (Firmata) library

Arduino arduino;

void setup() {
  size(400, 400);
  printArray(Serial.list());                              // see your ports in the console
  arduino = new Arduino(this, Serial.list()[0], 57600);  // change [0] to your Arduino port index
  rectMode(CENTER);
}

void draw() {
  background(240);
  int value = arduino.analogRead(0);
  float x = map(value, 0, 1023, 0, width);  // dial → x position
  fill(30, 120, 240);
  ellipse(x, height / 2, 60, 60);
}
```

### 03

```java
import processing.serial.*;   // Serial library
import cc.arduino.*;          // Arduino (Firmata) library

Arduino arduino;

void setup() {
  size(400, 400);
  printArray(Serial.list());
  arduino = new Arduino(this, Serial.list()[0], 57600);  // change [0] if needed
  rectMode(CENTER);
}

void draw() {
  background(240);
  int value = arduino.analogRead(0);
  float angle = map(value, 0, 1023, 0, TWO_PI);  // dial → rotation angle

  translate(width / 2, height / 2);  // move the origin to the centre
  rotate(angle);                     // rotate by the angle
  rectMode(CENTER);
  fill(30, 120, 240);
  rect(0, 0, 150, 60);
}
```

### 04

```java
import processing.serial.*;   // Serial library
import cc.arduino.*;          // Arduino (Firmata) library

Arduino arduino;

void setup() {
  size(400, 400);
  printArray(Serial.list());
  arduino = new Arduino(this, Serial.list()[0], 57600);  // change [0] if needed
  textAlign(CENTER, CENTER);
}

void draw() {
  background(240);

  int value = arduino.analogRead(0);  // 0-1023
  float letterSize = map(value, 0, 1023, 20, 260);

  fill(30, 120, 240);
  textSize(letterSize);
  text("A", width/2, height/2);
}
```

> **Feedback:** [tell us how Task 3 went](/survey/2/task3) before moving on.

## Task 4: Make sounds with the dial

We can also work with sound in Processing. It comes with a simple library already loaded. Some key functions that might be helpful for today are:

> `SoundFile song = new SoundFile(this, "song.wav");` for longer audio/music
>
> `AudioSample kick = new AudioSample(this, "kick.wav");` for short drum hits / SFX
>
Play / stop basics

> `song.play();` play once
>
> `song.loop();` repeat forever
>
> `song.stop();` stop playback
>
> `song.pause();` pause (if your version supports it)
>
> `song.isPlaying();` check if currently playing

Change loudness / stereo / speed-pitch
> `song.amp(0.5);` volume (usually 0.0 to 1.0)
>
> `song.rate(1.0);` playback speed
>
> 0.5 = slower + lower pitch
>
> 2.0 = faster + higher pitch
>
> -1.0 = reverse (in many cases)
>

> When working with soundfiles you must load them into the **data folder** first.
>
> The easiest way to do this is to drag and drop your file into the sketch.

### 01
Try control the playback speed

```java
import processing.serial.*;
import cc.arduino.*;
import processing.sound.*;

Arduino arduino;
SoundFile song;

void setup() {
  size(400, 400);
  printArray(Serial.list());
  arduino = new Arduino(this, Serial.list()[0], 57600); // change [0] if needed
  song = new SoundFile(this, "song.wav");
  song.loop();   // keep it playing so you hear the change live
}

void draw() {
  background(240);
  int value = arduino.analogRead(0);
  float speed = map(value, 0, 1023, 0.5, 2.0);  // dial → speed & pitch
  song.rate(speed);
}
```

### 02 Cycle through samples

```java
import processing.serial.*;
import cc.arduino.*;
import processing.sound.*;

Arduino arduino;
SoundFile kick, snare, hihat;
int lastZone = -1;   // which third the dial was in last frame

void setup() {
  size(400, 400);
  printArray(Serial.list());
  arduino = new Arduino(this, Serial.list()[0], 57600); // change [0] if needed
  kick  = new SoundFile(this, "kick.wav");
  snare = new SoundFile(this, "snare.wav");
  hihat = new SoundFile(this, "hihat.wav");
  textSize(40);
  textAlign(CENTER, CENTER);
}

void draw() {
  background(240);
  int value = arduino.analogRead(0);

  // which third of the dial? 0, 1 or 2
  int zone = 0;
  if (value > 683) zone = 2;
  else if (value > 341) zone = 1;

  // only play when we move into a NEW third
  if (zone != lastZone) {
    if (zone == 0) kick.play();
    if (zone == 1) snare.play();
    if (zone == 2) hihat.play();
    lastZone = zone;
  }

  fill(30);
  if (zone == 0) text("KICK", width/2, height/2);
  if (zone == 1) text("SNARE", width/2, height/2);
  if (zone == 2) text("HIHAT", width/2, height/2);
}
```

> **Feedback:** [tell us how Task 4 went](/survey/2/task4) before moving on.

## Task 5: Create some audio visuals!

Time to explore joining up drawing and sound. Create some audio visuals of your choice. I have added two examples that are easy to build from below or you can build froem a previous example or start from scratch. Whichever you prefer!

### 01 Background colour and volume change

```java
import processing.serial.*;
import cc.arduino.*;
import processing.sound.*;

Arduino arduino;
SoundFile pad;

void setup() {
  size(400, 400);
  printArray(Serial.list());
  arduino = new Arduino(this, Serial.list()[0], 57600); // change [0] if needed
  pad = new SoundFile(this, "pad.wav");
  pad.loop();
}

void draw() {
  int value = arduino.analogRead(0);

  float volume = map(value, 0, 1023, 0, 1);    // dial → volume
  pad.amp(volume);

  float blue = map(value, 0, 1023, 60, 255);   // dial → how blue the background is
  background(30, 60, blue);
}
```

### 02 Draw a shape and speed up/slow down a sample

```java
import processing.serial.*;
import cc.arduino.*;
import processing.sound.*;

Arduino arduino;
SoundFile beat;

void setup() {
  size(400, 400);
  printArray(Serial.list());
  arduino = new Arduino(this, Serial.list()[0], 57600); // change [0] if needed
  beat = new SoundFile(this, "beat.wav");
  beat.loop();
}

void draw() {
  background(240);
  int value = arduino.analogRead(0);

  float speed = map(value, 0, 1023, 0.5, 2.0);  // dial → pitch & speed
  beat.rate(speed);

  float size = map(value, 0, 1023, 20, 300);    // dial → circle size
  fill(30, 120, 240);
  ellipse(width/2, height/2, size, size);       // bigger + faster together
}
```

### 03 Different sounds and shapes

```java
import processing.serial.*;
import cc.arduino.*;
import processing.sound.*;

Arduino arduino;
SoundFile[] sounds = new SoundFile[3];   // a list holding 3 sounds
int lastActive = -1;                     // which one was active last frame

void setup() {
  size(600, 300);
  printArray(Serial.list());
  arduino = new Arduino(this, Serial.list()[0], 57600); // change [0] if needed

  // load the three sounds into the list
  sounds[0] = new SoundFile(this, "kick.wav");
  sounds[1] = new SoundFile(this, "snare.wav");
  sounds[2] = new SoundFile(this, "hihat.wav");

  rectMode(CENTER);
  noStroke();
}

void draw() {
  background(240);
  int value = arduino.analogRead(0);

  // the dial chooses which one (0, 1 or 2) is active
  int active = 0;
  if (value > 683) active = 2;
  else if (value > 341) active = 1;

  // play its sound only when it first becomes active
  if (active != lastActive) {
    sounds[active].play();
    lastActive = active;
  }

  // draw all three shapes: blue when active, grey when resting
  if (active == 0) fill(30, 120, 240); else fill(210);
  ellipse(150, 150, 120, 120);            // circle

  if (active == 1) fill(30, 120, 240); else fill(210);
  rect(300, 150, 120, 120);               // square

  if (active == 2) fill(30, 120, 240); else fill(210);
  triangle(450, 100, 400, 200, 500, 200); // triangle
}
```


### 04 Random circles and sound rate (intermediatte example)

```java
import processing.serial.*;
import cc.arduino.*;
import processing.sound.*;

Arduino arduino;
SoundFile beat;

void setup() {
  size(600, 400);
  printArray(Serial.list());
  arduino = new Arduino(this, Serial.list()[0], 57600); // change [0] if needed

  beat = new SoundFile(this, "beat.wav");
  beat.loop();
  beat.amp(0.8);

  noStroke();
  background(20);
  textSize(16);
}

void draw() {
  int value = arduino.analogRead(0); // 0-1023

  // Sound: playback rate (pitch + speed together)
  float rateValue = map(value, 0, 1023, 0.5, 2.0);
  beat.rate(rateValue);

  // Graphics: soft fade so old circles slowly disappear
  fill(20, 25);
  rect(0, 0, width, height);

  // More circles as dial increases
  int circlesThisFrame = int(map(value, 0, 1023, 1, 4));
  for (int i = 0; i < circlesThisFrame; i++) {
    float size = random(10, map(value, 0, 1023, 40, 140));
    fill(random(255), random(255), random(255), 180);
    ellipse(random(width), random(height), size, size);
  }

}
```
> **Feedback:** [tell us how Task 5 went](/survey/2/task5) before moving on.


We are finished for the day and workshop. Please complete the last set of feedback questions to help my research. **[feedback for today](/survey/2/exit)**.


## Reference
- [Processing reference](https://processing.org/reference)
- [Processing sound library](https://processing.org/reference/libraries/sound/index.html)

