//Class is currently unused, use as reference
import logo from '../logo.svg';
import '../App.css';
import { useState } from "react"

import Navbar from "../components/Navbar"


export default function App () {
  let socket;
  let numberOfPings = 0;
  const [ping, setPing] = useState(null);
  createSocketConnection();
  function getCurrentDate() {
    if (socket.readyState == WebSocket.OPEN) {
      socket.send(`Date please ${numberOfPings}`)
      numberOfPings += 1;
    } else {
      setPing("unavailable")
    }
  }
  function createSocketConnection() {
    console.log("Created Socket Connection")
    socket = new WebSocket("ws:/localhost:8080");
    socket.onopen = () => {
      console.log("Connected to server");
    }

    socket.onmessage = (event) => {
      const time = event.data.toString().split(",")
      const nowSec = new Date().getUTCSeconds()
      const nowMilliSec = new Date().getUTCMilliseconds();
      const currPing = `${((nowSec - time[0]) / 1000.0) + (nowMilliSec - time[1])} ms`;
      setPing(currPing);
    }
  }
  return (
    <>
      <div className="App">
        <header className="App-header">
          <img src={logo} className="App-logo" alt="logo" />
          <p>
            Ping: {ping}
          </p>
          <a
            className="App-link"
            href="https://reactjs.org"
            target="_blank"
            rel="noopener noreferrer"
          >
            Learn React
          </a>
          <button onClick={getCurrentDate}>
            Press Me!
          </button>
        </header>
      </div>
    </>
  )
}
