// Function to format the timestamp as "yyyy:mm:dd hh:mm:ss"
function getFormattedTimestamp() {
    const now = new Date();
    const year = now.getFullYear();
    const month = String(now.getMonth() + 1).padStart(2, '0'); // Months are zero-indexed
    const day = String(now.getDate()).padStart(2, '0');
    const hours = String(now.getHours()).padStart(2, '0');
    const minutes = String(now.getMinutes()).padStart(2, '0');
    const seconds = String(now.getSeconds()).padStart(2, '0');
    return `${year}:${month}:${day} ${hours}:${minutes}:${seconds}`;
}

const data = {
    "Serial_no": "CNCIO_computer",
    "timestamp": getFormattedTimestamp(), // formatted timestamp
    "firmware": "0.0.1",
    "ip": "192.168.137.104",
    "wifi": "device",
    "uptime": 243,
    "rssi": -76,
    "A1": 14,
    "A2": 22
};

//const url = 'https://webhook.site/bab5a3e3-a572-43d5-a2d0-8047e0f55935';
const url = 'http://172.105.40.182:3980/api/rms/crm'
fetch(url, {
    method: 'POST',
    headers: {
        'Content-Type': 'application/json'
    },
    body: JSON.stringify(data)
})
.then(response => response.json())
.then(result => console.log('Success:', result))
.catch(error => console.error('Error:', error));

