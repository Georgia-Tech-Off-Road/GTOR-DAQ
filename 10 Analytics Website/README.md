# Documentation

### Concepts
Welcome to the GTOR-DAQ Data Analytics Website! First off the basic frameworks: React.js, Node.js, Express.js, and MongoDB. We currently have implemented a basic frontend backend setup with React, Node, and Express. Basic knowledge of HTML, CSS, and Javascript is assumed, as is basic web development concepts (RESTful APIs, HTTP request, frontend vs backend). You will see two sub folders here, client for the frontend, and backend for the backend server. During raceday, data will be received from the moving offroad vehicle by the backend, processed, and sent to connected clients via a POST request. It is important to note that the server makes a request to the clients and not the other way around. The client GUI consist of an inline list of graphs (which may be toggeled on or off via a control panel), which displays real time data from the vehicle sent from the backend. Of course, please read each file. 

##### Backend Organization
Entry point at ./backend/app.js, additional routes will be created in ./backend/routes directory.

##### Frontend Organization
Entry point at ./client/src/index.js. Inside root.render() defines structure of the overall website. 
<RouterProvider> tag defines what content to show depending on path URL. Routes are defined in ./client/src/routes.js.
All pages of the app are contained inside of ./client/src/pages. Components to create the pages (eg. a navigation bar) are contained inside of ./client/src/components. ./client/src/App.css is the only CSS file for the frontend client, DO NOT create more CSS files. This is not a massive 20+ page site, one CSS file is enough. 

### Quick Start
To run, first run "npm install" inside of the client and backend subdirectories. This will create a node_modules subdirectory containing the dependencies required. To run the backend or the client run "nodemon" in the respective folder. Whenever a change is written to ANY file in a subdirectory with nodemon running the code will be recompiled and run again.

##### Data Flow
Vehicle > POST to backend > POST to connected clients.