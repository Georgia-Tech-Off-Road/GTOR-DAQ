import Chart from "chart.js/auto";
import 'chartjs-adapter-luxon';
import ChartStreaming from 'chartjs-plugin-streaming';
import { useRef, useEffect } from "react";

Chart.register(ChartStreaming);

export default function SensorGraph({ socket }) {
    const canvasRef = useRef(null);
    const chartRef = useRef(null);
    const chartRenderedRef = useRef(false);
    let currentIndex = 1;

    useEffect(() => {
        if (!chartRenderedRef.current) {
            generateInitialGraph();
        }
    }, []);

    useEffect(() => {
        if (socket) {
            requestData(socket);
            socket.addEventListener("message", (msg) => {
                let res = JSON.parse(msg.data);
                if (res.msgType.toString() === "data") {
                    updateGraph(res.content);
                }
            });
        }
    }, [socket]);

    async function generateInitialGraph() {
        const data = {
            datasets: [
                {
                    label: "Brake Pressure Front",
                    data: [],
                    borderColor: "rgba(255, 99, 132, 1)", // Red
                    fill: false,
                    tension: 0.1
                },
                {
                    label: "Brake Pressure Rear",
                    data: [],
                    borderColor: "rgba(54, 162, 235, 1)", // Blue
                    fill: false,
                    tension: 0.1
                }
            ]
        };

        chartRef.current = new Chart(canvasRef.current, {
            type: "line",
            data: data,
            options: {
                responsive: true,
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

    function testDataPush() {
        let y1 = 10;
        let y2 = 8;

        const intervalID = setInterval(() => {
            y1 += (Math.random() - 0.5) * 0.5;
            y2 += (Math.random() - 0.5) * 0.5;

            const now = Date.now();

            chartRef.current.data.datasets[0].data.push({ x: now, y: y1 });
            chartRef.current.data.datasets[1].data.push({ x: now, y: y2 });

            chartRef.current.update('quiet');
        }, 100);

        setTimeout(() => {
            clearInterval(intervalID);
        }, 10000);
    }

    return (
        <canvas ref={canvasRef}></canvas>
    );
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
