//React
import React from 'react';
import ReactDOM from 'react-dom/client';

//CSS
import './index.css';

//Website React Components
import App from './pages/App';
import Navbar from './components/Navbar'
import Test from "./pages/Test"

import reportWebVitals from './reportWebVitals';

//Routing
import { RouterProvider } from 'react-router-dom';
import router from './routes';

//Initialize WebSocket connection once and pass it down
const socket = new WebSocket("ws://localhost:9000");

socket.onopen = () => {
    console.log("WebSocket Connected");
};

socket.onclose = () => {
    console.log("WebSocket Disconnected");
};

const root = ReactDOM.createRoot(document.getElementById('root'));
root.render(
  <React.StrictMode>
      <Navbar socket={socket} />
    <RouterProvider router={router}/>
  </React.StrictMode>
);

// If you want to start measuring performance in your app, pass a function
// to log results (for example: reportWebVitals(console.log))
// or send to an analytics endpoint. Learn more: https://bit.ly/CRA-vitals
reportWebVitals();
