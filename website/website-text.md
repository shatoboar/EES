# Group name
**Group members**: Filip Rejmus, Sang Woo Bae, ...TODO

## Project overview

Over the course of this project we created a Assembly Line Robot that is able to sort stones according to their color and size while also detecting objects that aren't expected in the assembly line.

### Goals

Our goals for this project were:

- A reliable system detecting and sorting lego stones while sorting out unwanted objects
- A scalable and adaptable assembly line robot


### Requirements
There were multiple necessary requirements for this project.
Firstly, we were limited to using NXT controllers with very limited space and memory.

Our Assembly line had to

- Be able to sort objects according to their color and shape
- Sort accurately and efficiently
- Utilize the extremely limited NXT Brick as the robot controller.

### Approach

We assigned each component of our system architecture to two members of our group.
We defined the system specifications early and rigidly sticked to them throughout the project.
This allowed us to integrate our system continously and left space for the introduction of new hardware and software features.

## System architecture
Our project consisted of three main components that each respective team worked on: The Backend, the Robot Controller and the Bluetooth component. The Backend was programmed in C++ and is functioning as the eyes of our controller. It instructs our NXT Brick via Bluetooth to place objects on the conveyor and analyzes the image to command the controller to the right bucket. The NXT Brick takes the form of a state machine that gets triggered to take a specific state by the commands arriving over bluetooth. It is programmed in C and NXTOsek, a custom C based OS for the controller hardware. The communication interfaces were exposed in both environments as high level interfaces that represent the different stages of our sorting logic as presented in the chart.

![diagram](./images/softwarechart.png)

### Software design
**Short** overview of your software design. Maybe a diagram that illustrates the connections between the individual components. 

Please **do not** use the diagram from our project description.



### Robot design

Our robot consists of:
- main moveable robot with NXT brick
- tracks for the robot
- boxes to sort in 
- dispenser for the lego bricks at one side of the tracks

The main robot has an assembly line attached. This robot unit drives to the dispenser to get a new stone.
The dispenser consists of a tower filled with Lego stones, as well as a horizontal motor-controlled pusher.
We chose the combination of the pusher and the tower like that, so it is only possible to eject one lego stone at a time. A difficulty was to only drop one stone at a time in combination with different stones. The solve this problem, we narrowed the slot of the Lego tower, until it had the correct size.
Our main robot has two touch sensors, one attached to each side. These are for calibration purposes and safety features. If a touch sensor is hit while the robot is not expecting it, the robot will recalibrate.
On each box is a white Lego stone attached. We call them 'calibration stones'. They are recognized by the robot's light sensor. If the light sensor is over a calibration stone, the robot is in the correct position to drop a stone from the assembly line into the box.
Above the assembly line, the Raspberry PI camera is attached. This way, the assembly line can be analyzed and the correct element on the line recognized.


### Environment design
What does the environment look like in which your robot operates? 
The same: **Why** did you choose this environment?

![evironment](./images/environment.png)

### Algorithms
Write an introduction to the **most essential** algorithms or technologies in general that you have chosen for your project. 

Maybe with **short** code examples.

```python
def our_algorithm(x, y):
    # Do fancy stuff here
    return {'x': x, 'y': y}
```

## Summary
Overview of the achieved **and** not attained goals. Why were some goals not reached? Too difficult or wrong time management?

### Lessons learned
What did you learn from the project? What decisions would you have made differently from your current perspective?

### Future work
What problems would you tackle if you would continue to work on the project? Are there things you might actually take up and work on in the future? This part is **optional**.




