import {useEffect} from "react"
import Navbar from "../components/Navbar";
import SensorGraph from "../components/SensorGraph";
import SensorChartManager from "../components/SensorChartManager";
export default function Data({socket}) {
    return(
        <>
          <SensorChartManager socket={socket}></SensorChartManager>
        </>
    )
}