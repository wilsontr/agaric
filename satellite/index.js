var SerialPort = require('serialport');
var express = require('express');
var bodyParser = require('body-parser');
var _ = require('lodash');
var port = new SerialPort('/dev/cu.usbmodem3417681', {
  baudRate: 57600
});

var app = express();
app.use(bodyParser.json());
app.listen(3001);
console.log("Server running at http://localhost:3001/");


app.post('/api/color', (req, res) => {
	console.log(req.body);
	if ( _.get(req.body, 'red') && _.get(req.body, 'green') && _.get(req.body, 'blue') ) {
		let command = sendColorCommand(req.body.red, req.body.green, req.body.blue);
		res.send(`Color command sent: ${command}`);
	} else {
		res.send('No command sent');
	}
});

function sendColorCommand(r, g, b) {
	var command = `color ${r}, ${g}, ${b}`;
	port.write(command);
	return command;
}

/*
setTimeout(() => {
	port.write("color 10, 20, 200");
}, 100);

setTimeout(() => {
	port.write("color 10, 200, 20");
}, 2000);

setTimeout(() => {
	port.write("color 200, 20, 20");
}, 4000);
*/