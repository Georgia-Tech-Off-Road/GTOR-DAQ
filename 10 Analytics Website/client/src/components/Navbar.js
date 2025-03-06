import '../App.css';
import { useState, useEffect } from "react";

export default function Navbar() {
    const [ping, setPing] = useState(null);

    useEffect(() => {
        const ws = new WebSocket("ws://localhost:8080");

        ws.onopen = () => {
            console.log("WebSocket Connected");
        };

        ws.onmessage = (event) => {
            const timestamps = event.data.split(",");
            const serverTime = parseInt(timestamps[0]) * 1000 + parseInt(timestamps[1]);
            const clientTime = new Date().getUTCSeconds() * 1000 + new Date().getUTCMilliseconds();
            setPing(clientTime - serverTime); // Compute latency
        };

        // Send ping every second
        const interval = setInterval(() => {
            if (ws.readyState === WebSocket.OPEN) {
                ws.send("ping");
            }
        }, 1000);

        ws.onclose = () => {
            console.log("WebSocket Disconnected");
            clearInterval(interval);
        };

        return () => {
            ws.close();
            clearInterval(interval); // Cleanup on unmount
        };
    }, []);

    return (
        <nav className="navbar">
            <div className="nav-container">
                <ul className="nav-list">
                    <li><a href="/">Home</a></li>
                    <li><a href="/test">Test</a></li>
                    <li><a href="/data">Race Data</a></li>
                </ul>
                <div className="ping-display">Ping: {ping !== null ? `${ping}ms` : "Calculating..."}</div>
            </div>
        </nav>
    );
}
