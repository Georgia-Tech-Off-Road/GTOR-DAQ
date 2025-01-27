const express = require("express");
const cors = require("cors")
const http = require("http")
const net = require("net")


const app = express();
const HOST = "127.0.0.1";
const PORT = 9000;

const testAPIRouter = require("./routes/testAPI.js");
const WebSocket = require("ws")
// Allows communication between server and client, SUPER IMPORTANT
app.use(cors());

// Example code for how values may be passed by route

/* app.use('/things', things)
app.get("/:name/:id([0-9]{5})", function(req, res) {
    res.send(`The name you specified is ${req.params.name} and the id is ${req.params.id}`);
})

// MUST be at very end of file
app.get("*", function(req, res) {
    res.send("Sorry this is an invalid request!");
}) */

//On the testAPI route, use the tesAPI router to determine res-res cycle.
app.use("/testAPI", testAPIRouter);

//Create a socket listenting on $HOST:$PORT
app.listen(PORT, HOST, callback = function() {
    console.log(`Listening on ${HOST}:${PORT}`);
})

/*Websocket server for communication of real-time data, MUCH faster than http requests
because a connection is only established one time. May need to be load-balanced in the future,
depending on expected volume of data transmitted and connected clients. 
Currently used to detect ping between client and backend.*/
const wss = new WebSocket.Server({port: 8080});
wss.on("connection", (ws) => {
    console.log("Client connected");

    //Response for any message
    ws.on("message", (message) => {
        console.log(`Received message: ${message}`)
            if(ws.readyState == WebSocket.OPEN) {
                //Send current second and millisecond
                ws.send(`${new Date().getUTCSeconds().toString()},${new Date().getUTCMilliseconds().toString()}`)
            }
    })
    //Close connection
    ws.on('close', () => {
        console.log('Client disconnected');
      });
    //Display on creation
    console.log('WebSocket server is running on ws://localhost:8080')
})