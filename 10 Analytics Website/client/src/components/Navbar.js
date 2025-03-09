import '../App.css';
import { useState, useEffect } from "react";

export default function Navbar({socket}) {
    const [ping, setPing] = useState(null);

    useEffect(() => {


        if (!socket) return;
        socket.onopen = () => {
            console.log("WebSocket Connected");
        };

        const lonmessage = (event) => {
            const timestamps = event.data.split(",");
            const serverTime = parseInt(timestamps[0]) * 1000 + parseInt(timestamps[1]);
            const clientTime = new Date().getUTCSeconds() * 1000 + new Date().getUTCMilliseconds();
            setPing(clientTime - serverTime); // Compute latency
        };
        socket.onmessage = lonmessage;

        // Send ping every second
        const interval = setInterval(() => {
            if (socket.readyState === WebSocket.OPEN) {
                socket.send("ping");
            }
        }, 1000);



        socket.onclose = () => {
            console.log("WebSocket Disconnected");
            clearInterval(interval);
        };

        return () => {
            clearInterval(interval); // Cleanup on unmount
        };
    }, [socket]);

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
