import {useEffect, useState} from "react"
import Navbar from "../components/Navbar";
import SensorGraph from "../components/SensorGraph";
import SensorChartManager from "../components/SensorChartManager";
import "../components/styles/SensorChartManager.css";

export default function Data({socket}) {
    const [isArrangementMode, setIsArrangementMode] = useState(false);

    function toggleArrangementMode() {
        setIsArrangementMode(!isArrangementMode);
    }

    function handleLayoutChange(layout) {
        // Optional: Handle layout changes at the parent level
        console.log('Layout changed:', layout);
    }

    const arrangeButtonText = isArrangementMode ? "Stop Arranging" : "Arrange Charts";

    return(
        <>
          <div className="chart-manager-toolbar">
            <button onClick={toggleArrangementMode}>{arrangeButtonText}</button>
            <button>Save Layout</button>
          </div>
          <SensorChartManager 
            socket={socket}
            isArrangementMode={isArrangementMode}
            onLayoutChange={handleLayoutChange}
          />
        </>
    )
}