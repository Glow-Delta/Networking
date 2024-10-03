const express = require('express');
const net = require('net');
const app = express();
const port = 3000;

app.use(express.json());
app.use(express.static('public'));  // Serve the static web files

// Route to send color change request to the C++ socket server
app.post('/color', (req, res) => {
    const clientId = req.body.clientId;
    const color = req.body.color;

    const client = new net.Socket();
    client.connect(8080, 'localhost', () => {
        const message = `client${clientId}/color=${color}`;
        client.write(message);
    });

    client.on('data', (data) => {
        console.log(`Received from server: ${data}`);
        res.send(`Color change sent to client ${clientId}`);
        client.destroy();  // Close connection after receiving response
    });

    client.on('error', (err) => {
        console.error('Connection error:', err);
        res.status(500).send('Error communicating with the C++ server');
    });
});

app.listen(port, () => {
    console.log(`Web interface running on http://localhost:${port}`);
});
