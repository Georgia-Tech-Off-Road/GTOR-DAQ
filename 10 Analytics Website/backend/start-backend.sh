#!/bin/bash
docker create --mount type=bind,source=./app.js,target=/home/node/app/app.js,readonly \
--net custom_net --ip 10.5.0.5 \
 --mount type=bind,source=./routes,target=/home/node/app/routes,readonly \
  -p 9000:9000 --name TestName gtor-analytics-backend:test
echo "Created Docker container successfully!"