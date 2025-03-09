import Navbar from "../components/Navbar";
import SensorGraph from "../components/SensorGraph";
export default function Data({socket}) {
  socket.send("data");
  socket.onmessage = (msg) => {
    console.log(msg + "\n");
  }
    return(
        <>
          <SensorGraph></SensorGraph>
        </>
    )
}