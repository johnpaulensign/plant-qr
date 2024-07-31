const express = require('express');
const bodyParser = require('body-parser');

const app = express();

const http = require('http');
const server = http.createServer(app);

// Create a socket.io server to publish plant data
const { Server } = require("socket.io");

const io = new Server(server);

io.on('connection', (socket) => {
  console.log('a user connected');
});

// Middleware
app.use(bodyParser.json());

// GET endpoint for "/plant/{plant_id}/{plant_action}" that publishes the plant data to the socket.io server
app.get('/plant/:plant_id/:plant_action', (req, res) => {
  const plantId = req.params.plant_id;
  const plantAction = req.params.plant_action;

  // Process the plant data and perform necessary operations
  console.log(`Received plant data for plant ID ${plantId} with action ${plantAction}`);

  // Publish the plant data to the socket.io server
  io.emit('plantAction', {"plantId": plantId, "plantAction": plantAction});

  res.status(200).json({ message: 'Plant data received successfully' });
});

// Serve the static files from the Vue app in src folder
app.use(express.static(__dirname + '/src'));

// Handle all other routes by serving the Vue app
app.get('/', (req, res) => {
  res.sendFile(__dirname + '/src/index.html');
});


// Start the server
const port = 3000;
server.listen(port, () => {
  console.log(`Server is running on port ${port}`);
});