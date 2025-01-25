//Class is currently unused, use as reference
import logo from './logo.svg';
import './App.css';
import React, {Component} from "react";
import { useState } from "react"


export default function App () {
  let socket;
  let numberOfPings = 0;
  const [ping, setPing] = useState(0);
  createSocketConnection();
  function callAPI() {
    fetch("http://localhost:9000/testAPI")
      .then(res => res.text())
      .then(res => this.setState( {apiResponse: res}));
  }
  function componentDidMount() {
    callAPI();
  }
  function getCurrentDate() {
    socket.send(`Date please ${numberOfPings}`)
    numberOfPings += 1;
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
      const currPing = ((nowSec - time[0]) / 1000.0) + (nowMilliSec - time[1]);
      setPing(currPing);
    }
  }
  const sampleText = fetch("http://localhost:9000/testAPI")
  return (
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
  )
}
