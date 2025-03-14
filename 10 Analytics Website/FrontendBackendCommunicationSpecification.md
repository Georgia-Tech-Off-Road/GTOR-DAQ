# Frontend to Backend

The frontend will usually make a request to the backend by with a simple statement socket.send("data") or socket.send("ping").

## Requests:

### ping
The frontend requests a packet to calculate ping from the backend.
Ex: socket.send("ping")

### data
The frontend signals to the backend that it is ready to receive graph data
Ex: socket.send("data")



# Backend to Frontend

Backend to frontend packets are a lot more complicated than vice-versa. These messages are composed as follows:

{\
    msgType: "msgType"\
    content: ""\
}\
msgType can be any string that describes the type of message that the backend is sending. For example a ping response packet will look as follows:
{\
    msgType: "ping"\
    content: "timeInSec,timeInMillisec"\
}\
It is important to note that content can be of ANY data type. All packets with the same msgType value should return the same data type of content.

## Requests

### ping
{\
    msgType: "ping"\
    content: "timeInSec,timeInMillisec"\
}\
Pretty self explanatory, not too complicated.

### data

{\
    msgType: "msgType"\
    content: "sensorData"\
}\
Here sensorData is stand in for whatever our current sensor data object is. This sensorData structure should contain fields for every possible sensor that we might include on the vehicle. Here is an example of what a Sensor Data object could look like:\
{\
    timeSinceStartMilli: 12345,\
    clutchEngaged: null,\
    pitRequest: null,\
    speed: null,\
    position: null,\
    brakePresssureFront: 100,\
    brakePressureRear: 200,\
    RPMFrontLeft: 1000,\
    RPMFrontRight: 1350,\
    RPMRear: 1250,\
    linearAcceleration: null,\
    CVTTemp: null,\
    transferCaseTemp: null,\
    brakeTemp: null,\
    steeringAngle: null\
}\