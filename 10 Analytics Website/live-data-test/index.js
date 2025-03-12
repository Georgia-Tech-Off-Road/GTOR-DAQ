const WebSocket = require("ws")
const nReadLines = require("n-readlines")

//------------------- IMPORTANT EDIT THIS ----------------------
const TEST_FILE_NAME = "Test2.csv"

const pparse = require("papaparse")
let currLine = ""
const numLinesToSkip = 5000
let startSec = 0
let startMicroSec = 0


const wss = new WebSocket.Server({port: 8080})
wss.on("connection", (wss) => {
    console.log("Client connected");

    //Response for any message
    wss.on("message", (message) => {
        console.log(`Received message: ${message}`)
            if(wss.readyState == WebSocket.OPEN) {
                if (message.toString() === "ping") {
                    console.log("Sending Ping!")
                    //Send current second and millisecond
                    msg = JSON.stringify(
                        {msgType: "ping",
                         content: `${new Date().getUTCSeconds().toString()},${new Date().getUTCMilliseconds().toString()}`
                    })
                    console.log(msg)
                    wss.send(msg)
                } else if (message.toString() === "data") {
                    testLiveData(wss);
                }
            }
    })
    //Close connection
    wss.on('close', () => {
        console.log('Client disconnected');
      });
    //Display on creation
    console.log('WebSocket server is running on ws://localhost:8080')
})
console.log("Hello World!")

function testLiveData(wss) {
    let testFileLines = new nReadLines(TEST_FILE_NAME)
    currLine = testFileLines.next()
    currLine = currLine.toString("ascii")
    let data = currLine.split(",")
    startSec = data[0]
    startMicroSec = data[1]
    console.log(`Start Sec: ${startSec}`)
    console.log(`Start Microsec: ${startMicroSec}`)
    while (currLine = testFileLines.next()) {
        for (let i = 0; i < numLinesToSkip; i++) {
            testFileLines.next()
        }
        currLine = currLine.toString("ascii")
        let data = currLine.split(",")
        console.log(data + "\n")
        dataJSON = {
            //Time since data collection start in millisec
            timeSinceStartMilli: ((data[0] - startSec) * (10 ** 3)) + ((data[1] - startMicroSec) * (10 ** -3)),
            clutchEngaged: null,
            pitRequest: null,
            speed: null,
            position: null,
            brakePresssureFront: data[5],
            brakePressureRear: data[6],
            RPMFrontLeft: null,
            RPMFrontRight: null,
            RPMRear: null,
            linearAcceleration: null,
            CVTTemp: null,
            transferCaseTemp: null,
            brakeTemp: null,
            steeringAngle: null
        }
        msg = JSON.stringify({
            msgType: "data",
            content: JSON.stringify(dataJSON)
        });
        console.log(msg)
        wss.send(msg)

    }
}