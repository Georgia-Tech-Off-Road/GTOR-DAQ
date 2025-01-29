import {createBrowserRouter} from "react-router-dom"

//Website React Components
import App from './pages/App';
import Navbar from './components/Navbar'
import Test from "./pages/Test"
import Data from "./pages/Data"

//Main router for web app
//Inspiration from: https://www.geeksforgeeks.org/create-a-basic-navbar-using-react-router-v6/?ref=next_article
const router = createBrowserRouter([
    {
        path: "/",
        element: <App />
        
    },
    {
         path: "/test",
        element: <Test />
    },
    {
        path: "/data",
        element: <Data />
    }
]);

export default router

