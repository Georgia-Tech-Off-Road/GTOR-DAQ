import Chart from "chart.js/auto";
import 'chartjs-adapter-luxon';
//Use Chart Streaming plugin: https://nagix.github.io/chartjs-plugin-streaming/master/guide/getting-started.html#integration
import ChartStreaming from 'chartjs-plugin-streaming';
import {useRef, useEffect} from "react";

Chart.register(ChartStreaming)

export default function SensorGraph({socket}) {

    //Reference to <canvas> in DOM
    const canvasRef = useRef(null);
    let chart = null;
    let chartRendered = false;
    let currentIndex = 1;
    //After DOM has been commited to, get graph data, declare chartRendered as a dependency
    //See: https://react.dev/learn/synchronizing-with-effects
    useEffect(() => {
        if(!chartRendered) {
            generateInitialGraph();
        }
    }, [chartRendered])

    //Setup the socket connection
    useEffect(() => {
        if (socket) {
            //Request data as part of setup
            requestData(socket)
            socket.addEventListener("message", (msg) => {
            let res = JSON.parse(msg.data)
            if (res.msgType.toString() === "data") {
              //JSON.parse convert a string to a json object, like in index.js of live-data-test!
              //
              let content = JSON.parse(res.content)
              updateGraph(content)
            }
          })
      }}, [socket]);

    //Generate graph using Chart.js
    async function generateInitialGraph() {
        //Test data
        let data = {
            datasets: [{
                axis: "y",
                label: "Sensor Data",
                data: [],
                fill: false,
                borderColor: "rgba(255, 0, 0, 1)",
                backgroundColor: "(0, 255, 0, 1)",
            }]
        }
        chart = new Chart(
            canvasRef.current,
              {
                options: {
                    plugins: {
                        streaming: {
                            duration: 5000
                        }
                    }
                },
                 type: "line",
                 data: data,
                 scales: {
                    x: {
                        type: "realtime"
                    }
                 }
           }
        )


        //Update chartRendered
        chartRendered = true
    }

    function testDataPush() {
        let intervalID = setInterval(() => {
            chart.data.datasets[0].data.push({x: Date.now(), y: currentIndex});
            currentIndex = currentIndex + 1;
            chart.update('quiet');
        }, 10)
        setTimeout(() => {
            clearInterval(intervalID)
        }, 10 * 1000)
    }
    return(
        <div style={{width: "800px"}}>
            <canvas ref={canvasRef}></canvas>
            <button onClick={testDataPush}>Click Me</button>
        </div>
    )
}

function requestData(socket) {
    //If socket is open, request data immediately
    if (socket.readyState === WebSocket.OPEN) {
      socket.send("data");
    } else { //Otherwise wait
      socket.addEventListener(
        "open",
        () => {
          alert("Requesting data!")
          socket.send("data")
        },
        { once: true}
      );
   }
  }
  
  //For y'all to implement!!!
  function updateGraph(sensorData) {
    console.log(JSON.stringify(sensorData))
    return;
  }