import Chart from "chart.js/auto";
import 'chartjs-adapter-luxon';
//Use Chart Streaming plugin: https://nagix.github.io/chartjs-plugin-streaming/master/guide/getting-started.html#integration
import ChartStreaming from 'chartjs-plugin-streaming';
import {useRef, useEffect} from "react";

Chart.register(ChartStreaming)

export default function SensorGraph() {

    //Reference to <canvas> in DOM
    const canvasRef = useRef(null);
    let chart = null;
    let chartRendered = false;
    //After DOM has been commited to, get graph data, declare chartRendered as a dependency
    //See: https://react.dev/learn/synchronizing-with-effects
    useEffect(() => {
        if(!chartRendered) {
            generateInitialGraph();
        }
    }, [chartRendered])

    //Generate graph using Chart.js
    async function generateInitialGraph() {
        //Test data
        const data = {
            labels: [0, 1, 2 ,3],
            datasets: [{
                axis: "y",
                label: "Sensor Data",
                data: [0, 1, 2, 3],
                fill: false,
                borderColor: "rgba(255, 0, 0, 1)",
                backgroundColor: "(0, 255, 0, 1)",
            }]
        }
        chart = new Chart(
            canvasRef.current,
              {
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
        
    }
    return(
        <div style={{width: "800px"}}>
            <canvas ref={canvasRef}></canvas>
            <button onClick={testDataPush}>Click Me</button>
        </div>
    )
}