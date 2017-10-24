const WebSocket = require('ws');

const ws = new WebSocket('ws://192.168.1.154/signalk/v1/stream');

ws.on('open', function open() {
  //console.log("connected!");
});

ws.on('message', function incoming(data) {
  console.log(data);
});
