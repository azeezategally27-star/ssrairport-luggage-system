const net = require('net');
const HOST = '127.0.0.1';
const PORT = 42042;
let client = null;

function connectBackend() {
  client = new net.Socket();
  client.connect(PORT, HOST, () => {
    document.getElementById('status').innerText = `Connected to ${HOST}:${PORT}`;
    // Request graph
    sendCommand('GRAPH');
  });

  client.on('data', (data) => {
    const text = data.toString();
    document.getElementById('raw').innerText = text;
    try {
      const obj = JSON.parse(text);
      if (obj.nodes) renderGraph(obj);
    } catch (e) {
      // not JSON or other response
      // show raw
    }
  });

  client.on('close', () => {
    document.getElementByText('status').innerText = 'Disconnected';
    client = null;
  });

  client.on('error', (err) => {
    document.getElementById('status').innerText = 'Error: ' + err.message;
    client = null;
  });
}

function sendCommand(cmd) {
  if (!client) {
    connectBackend();
    setTimeout(() => { if (client) client.write(cmd + '\n'); }, 300);
  } else {
    client.write(cmd + '\n');
  }
}

window.addEventListener('DOMContentLoaded', () => {
  document.getElementById('btnScanFace').addEventListener('click', () => sendCommand('SCAN_FACE'));
  document.getElementById('btnRegister').addEventListener('click', () => {
    const payload = JSON.stringify({ passenger_id: 'P123', type: 'face' });
    sendCommand('REGISTER ' + payload);
  });
  document.getElementById('btnVerify').addEventListener('click', () => sendCommand('SCAN_FACE'));
  document.getElementById('btnLogs').addEventListener('click', () => sendCommand('LOGS'));

  connectBackend();
});

function renderGraph(graphObj) {
  // pass to graph.js renderer if present
  if (window.renderGraph) window.renderGraph(graphObj);
}
