import Chart from "chart.js/auto";
import 'chartjs-adapter-luxon';
import ChartStreaming from 'chartjs-plugin-streaming';
import { useRef, useEffect } from "react";

Chart.register(ChartStreaming);

export default function SensorGraph({clientChartSettings, serverChartSettings, isArrangementMode}) {
    const canvasRef = useRef(null);
    const chartRef = useRef(null);
    const chartRenderedRef = useRef(false);
    let currentIndex = 1;

    useEffect(() => {
        if (!chartRenderedRef.current) {
            generateInitialGraph();
        }
    }, []);

    return (
        <div className="sensor-graph-container" style={{position: 'relative', width: '100%', height: '100%', zIndex: 10}}>
            <canvas className="SensorGraph" ref={canvasRef} style={{width: '100%', height: '100%'}} maintainResize={false}></canvas>
        </div>
    );

    /*useEffect(() => 
        if (socket) {
            requestData(socket);
            socket.addEventListener("message", (msg) => {
                let res = JSON.parse(msg.data);
                if (res.msgType.toString() === "data") {
                    updateGraph(res.content);
                }
            });
        }
    }, [socket]); */

    async function generateInitialGraph() {
        const data = {
            datasets: serverChartSettings.datasets
        };

        chartRef.current = new Chart(canvasRef.current, {
            type: "line",
            data: data,
            options: {
                responsive: true,
                maintainAspectRatio: false,
                interaction:{
                    intersect: !isArrangementMode,
                    mode: isArrangementMode ? 'none' : 'index'
                },
                scales: {
                    x: {
                        type: "realtime",
                        realtime: {
                            duration: 20000,
                            refresh: 1000,
                            delay: 1000
                        },
                        title: {
                            display: true,
                            text: "Time"
                        }
                    },
                    y: {
                        title: {
                            display: true,
                            text: "Brake Pressure"
                        }
                    }
                },
                maintainAspectRatio: false,
                elements: {
                    point: {
                        radius: 0
                    }
                },
                plugins: {
                    streaming: {
                        duration: 5000
                    }
                }
            }
        });

        chartRenderedRef.current = true;
    }

    function updateGraph(sensorData) {
        if (!chartRef.current || !sensorData) return;

        const front = parseFloat(sensorData.brakePresssureFront);
        const rear = parseFloat(sensorData.brakePressureRear);
        const now = Date.now();

        if (!isNaN(front)) {
            chartRef.current.data.datasets[0].data.push({ x: now, y: front });
        }
        if (!isNaN(rear)) {
            chartRef.current.data.datasets[1].data.push({ x: now, y: rear });
        }

        chartRef.current.update('quiet');
    }
}

function requestData(socket) {
    if (socket.readyState === WebSocket.OPEN) {
        socket.send("data");
    } else {
        socket.addEventListener(
            "open",
            () => {
                alert("Requesting data!");
                socket.send("data");
            },
            { once: true }
        );
    }
}
