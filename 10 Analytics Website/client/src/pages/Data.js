import {useEffect} from "react"
import Navbar from "../components/Navbar";
import SensorGraph from "../components/SensorGraph";
export default function Data({socket}) {
    return(
        <>
          <SensorGraph socket={socket}></SensorGraph>
        </>
    )
}