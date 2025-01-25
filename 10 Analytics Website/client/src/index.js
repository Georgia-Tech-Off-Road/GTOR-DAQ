//React
import React from 'react';
import ReactDOM from 'react-dom/client';

//CSS
import './index.css';

//Website React Components
import App from './App';
import Navbar from './components/Navbar'
import Test from "./components/Test"

import reportWebVitals from './reportWebVitals';

//Routing
import { RouterProvider } from 'react-router-dom';
import router from './routes';

const root = ReactDOM.createRoot(document.getElementById('root'));
root.render(
  <React.StrictMode>
    <RouterProvider router={router}/>
  </React.StrictMode>
);

// If you want to start measuring performance in your app, pass a function
// to log results (for example: reportWebVitals(console.log))
// or send to an analytics endpoint. Learn more: https://bit.ly/CRA-vitals
reportWebVitals();
