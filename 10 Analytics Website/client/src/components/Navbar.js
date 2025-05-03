import './styles/Navbar.css';
import { useState, useEffect } from "react";

export default function Navbar({socket}) {
    const [ping, setPing] = useState(null);
    const [connectionStatus, setConnectionStatus] = useState('Connecting...');

    useEffect(() => {
        if (!socket) return;

        const handleOpen = () => {
            console.log("WebSocket Connected");
            setConnectionStatus('Connected');
        };

        const handleClose = () => {
            console.log("WebSocket Disconnected");
            setConnectionStatus('Disconnected');
            clearInterval(interval);
        };

        const lonmessage = (event) => {
            try {
                let res = JSON.parse(event.data);
                if(res.msgType === "ping") {
                    const timestamps = res.content.split(",");
                    const serverTime = parseInt(timestamps[0]) * 1000 + parseInt(timestamps[1]);
                    const clientTime = new Date().getUTCSeconds() * 1000 + new Date().getUTCMilliseconds();
                    setPing(clientTime - serverTime); // Compute latency
                }
            } catch (error) {
                console.error("Error processing message:", error);
            }
        };

        socket.onopen = handleOpen;
        socket.onclose = handleClose;
        socket.onmessage = lonmessage;

        // Send ping every second
        const interval = setInterval(() => {
            if (socket.readyState === WebSocket.OPEN) {
                socket.send("ping");
            }
        }, 1000);

        return () => {
            clearInterval(interval); // Cleanup on unmount
            socket.removeEventListener('open', handleOpen);
            socket.removeEventListener('close', handleClose);
            socket.removeEventListener('message', lonmessage);
        };
    }, [socket]);

    return (
        <nav className="navbar">
            <div className="nav-container">
                <div className="logo-container">
                    <span className="eco-tech-logo">GTOR</span>
                    <span className="connection-status" data-status={connectionStatus.toLowerCase()}>
                        {connectionStatus}
                    </span>
                </div>
                <ul className="nav-list">
                    <li><a href="/">Home</a></li>
                    <li><a href="/test">Test</a></li>
                    <li><a href="/data">Race Data</a></li>
                </ul>
                <div className="system-metrics">
                    <div className="ping-display">
                        Ping: {ping !== null ? `${ping}ms` : "Calculating..."}
                    </div>
                </div>
            </div>
        </nav>
    );
}
