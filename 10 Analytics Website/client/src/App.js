import logo from './logo.svg';
import './App.css';
import React, {Component} from "react";


let socket;
let numberOfPings;
class App extends Component {
  constructor(props) {
    super(props);
    this.state = {ping: ""};
    this.createSocketConnection();
    numberOfPings = 0;
  }

  callAPI() {
    fetch("http://localhost:9000/testAPI")
      .then(res => res.text())
      .then(res => this.setState( {apiResponse: res}));
  }
  componentDidMount() {
    this.callAPI();
  }
  getCurrentDate() {
    socket.send(`Date please ${numberOfPings}`)
    numberOfPings += 1;
  }
  createSocketConnection() {
    console.log("Created Socket Connection")
    socket = new WebSocket("ws:/localhost:8080");
    socket.onopen = () => {
      console.log("Connected to server");
    }

    socket.onmessage = (event) => {
      const time = event.data.toString().split(",")
      const nowSec = new Date().getUTCSeconds()
      const nowMilliSec = new Date().getUTCMilliseconds();
      const ping = ((nowSec - time[0]) / 1000.0) + (nowMilliSec - time[1]);
      this.setState({ping: `${ping} ms`});
    }
  }
  render() {
    const sampleText = fetch("http://localhost:9000/testAPI")
    return (
      <div className="App">
        <header className="App-header">
          <img src={logo} className="App-logo" alt="logo" />
          <p>
            Ping: {this.state.ping}
          </p>
          <a
            className="App-link"
            href="https://reactjs.org"
            target="_blank"
            rel="noopener noreferrer"
          >
            Learn React
          </a>
          <button onClick={this.getCurrentDate}>
            Press Me!
          </button>
        </header>
      </div>
    );
  }
}


export default App;
