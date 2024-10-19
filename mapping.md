```js
var op = {
    "Serial_no":global.get("Serial_no") || 0,
    "timestamp": formatTimestamp(),
    "firmware": "focas",
    "ip": "192.168.11.11",
    "wifi": global.get("wifi_name")|| "na",
    "uptime": global.get("uptime") || 0,
    "rssi": "0",
    "A1":Number(data["machine_state_run"]), //Machine status
    "A2":Number(data["job_counter_value"]), //
    "A3":Number(data["cycle_time"][0] * 60),
    "A4":Number(data["nc_program"].program_number),
    "A5":Number(data["part_counter_value"]),
    "A6":Number(data["run_value"]),
    "A7":Number(data["power_on_time"][0] * 60),
    "A8":Number(data["cutting_time"][0] * 60) ,
}
```

```json
{
    "Serial_no": "00000000d6c32fe6",
    "timestamp": "2024:10:19 09:03:51",
    "firmware": "focas",
    "ip": "192.168.11.11",
    "wifi": "Amrit\n",
    "uptime": 65400,
    "rssi": "0",
    "A1": 3,
    "A2": 18,
    "A3": 180,
    "A4": 4445,
    "A5": 407980,
    "A6": 1708663,
    "A7": 102519780,
    "A8": 28517280
}
```


