# Processing Community Day: Planet Friendly Processing

In this workshop we're going to explore creating different interactive sketches using [Processing](https://processing.org/)

After every sketch, we will explore how much energy they take to run, ending by looking at some alternate options to display our work.

## Intro to Processing

In order to generate our interactive code we will use an open-source tool called **Processing**. You may have come across this application already or used something similar like p5.js or Arduino. These tools have a lot in common with each other, providing libraries on top of programming languages, to make coding a little more accessible to creatives.

Before we start we need to make sure we have Processing installed.
> Download Processing for your computer.
>
> [Download](https://processing.org/download)

The Processing language has a friendly library to get started with drawing shapes to the canvas. It uses words that might be familiar from other production tools like InDesign, Illustrator, Photoshop etc

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

## Task 1: Drawing

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
> Let's monitor how much **energy** the sketch uses. Plug your laptop into the monitor plugs whilst running your sketch. What do you see?

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
> Let's monitor how much **energy** the sketch uses. Plug your laptop into the monitor plugs whilst running your sketch. Does this one require more energy?


## Task 3: Movement

Now we have our mouse input working, we can start to add more complex inputs. Let's explore how to work with our webcams to do similar interactions.

> We will make simple changes to a pre-built example
>
> Download the example here
>
> You need to install the Open CV library
>
> Go to sketch/import libraries/manage libraries and search for Open CV.
>
> Select install

In the example there is extra code to detect your face via the webcam. However, we are going to focus on one line and how we can adapt that to control other things.

The example code below will not run on its own. It is selected lines you can copy and paste into the example you downloaded to help you get started.

Let's try move a shape with our face!

### 01

```java

void draw() {
  scale(scl);
  opencv.loadImage(video); //use the openCV library to start get webcam image

  image(video, 0, 0 ); //draw the webcam image to the canvas

  detectFaces(); //detect your face

  // Draw all the faces
  for (int i = 0; i < faces.length; i++) { //loop through all detected faces
    noFill();
    strokeWeight(5);
    stroke(255,0,0); //make the outline of the rectangle red

    //draw a rectangle around the face detected.
    rect(faces[i].x, faces[i].y, faces[i].width, faces[i].height);
  }

  //display the blue shade rectangle over the face with an outline of 2 thickness.
  for (Face f : faceList) {
    strokeWeight(2);
    f.display();
  }
}
```


### 02

```java
    //draw a rectangle around the face detected.

    //we can use this line to control other things!
    rect(faces[i].x, faces[i].y, faces[i].width, faces[i].height);

    //use your face position to move another shape

    //make it green
    fill(0,255,0);
    ellipse(faces[i].x, faces[i].y,60,60);

```

### 03

```java
    //draw a rectangle around the face detected.

    //we can use this line to control other things!
    rect(faces[i].x, faces[i].y, faces[i].width, faces[i].height);

    //use your how close your face is to resize a shape

    //make it green
    fill(0,255,0);
    ellipse(width/2, height/2,faces[i].width, faces[i].height);

    //do you need to put faces[i].width into a new range so the shape isnt too small or big? How might you do that?
```

### 04

```java
//can you control lots of graphics with your face?

//insert example


```

> Let's monitor how much **energy** the sketch uses. Plug your laptop into the monitor plugs whilst running your sketch. Does this one require more energy?

## Task 4: Export your sketch as an application

When we are using Processing in exihibitions, or sharing our work with others so they can run it on their machine, there is a helpful export function that means our sketch is built as an application. This means we don't need the other person to have Processing downloaded or all the libraries installed. It also makes scheduling and triggering the launch of the work much easier.

> Go to File/Export Application
>
> You will see a grey window appear where we set the export settings.
>
> For our next step we need the applications export for Linux.
>
> Make sure to select Linux 64 bit (see image)
>

//insert screenshot



### Media Timers

Often when we are running interactive work in exhibitions we end up using our personal laptops or large computers available for AV solutions. However, these are often far more powerful and power hungry than what we need.

You can easily run Processing applications from simpler single board computers. We've brought along some [Adaptables Media Timers]() to show you how this can be done.

As part of the Media Timer device, they come with a custom operating systems and software to help you easily schedule your work. You can set on/off scheduling to launch your app at multiple different times a day, different days of the week.


## Reference
- [Processing reference](https://processing.org/reference)
- [Processing sound library](https://processing.org/reference/libraries/sound/index.html)

