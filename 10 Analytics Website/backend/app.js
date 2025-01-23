const express = require("express");
const cors = require("cors")
const http = require("http")
const net = require("net")


const app = express();
const HOST = "127.0.0.1";
const PORT = 9000;

const testAPIRouter = require("./routes/testAPI.js");
const WebSocket = require("ws")
app.use(cors());

/* app.use('/things', things)
app.get("/:name/:id([0-9]{5})", function(req, res) {
    res.send(`The name you specified is ${req.params.name} and the id is ${req.params.id}`);
})

// MUST be at very end of file
app.get("*", function(req, res) {
    res.send("Sorry this is an invalid request!");
}) */

app.use("/testAPI", testAPIRouter);

app.listen(PORT, HOST, callback = function() {
    console.log(`Listening on ${HOST}:${PORT}`);
})

const wss = new WebSocket.Server({port: 8080});
wss.on("connection", (ws) => {
    console.log("Client connected");
    ws.on("message", (message) => {
        console.log(`Received message: ${message}`)
            if(ws.readyState == WebSocket.OPEN) {
                ws.send(`${new Date().getUTCSeconds().toString()},${new Date().getUTCMilliseconds().toString()}`)
            }
    })
    ws.on('close', () => {
        console.log('Client disconnected');
      });
    console.log('WebSocket server is running on ws://localhost:8080')
})