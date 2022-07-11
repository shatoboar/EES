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
