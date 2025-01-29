import Chart from "chart.js/auto";
import {useRef, useEffect} from "react";
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
        const data = [
            { year: 2010, count: 10 },
            { year: 2011, count: 20 },
            { year: 2012, count: 15 },
            { year: 2013, count: 25 },
            { year: 2014, count: 22 },
            { year: 2015, count: 30 },
            { year: 2016, count: 28 },
          ];

         chart = new Chart(
            canvasRef.current,
              {
                 type: "bar",
                 data: {
                       labels: data.map(row => row.year),
                     datasets: [
                           {
                             label: "Aquisitions by year",
                               data: data.map(row => row.count)
                        }
                   ]
                }
           }
        )

        //Update chartRendered
        chartRendered = true
    }
    return(
        <div style={{width: "800px"}}>
            <canvas ref={canvasRef}></canvas>
        </div>
    )
}