
```js
var op = {
    "Serial_no":global.get("Serial_no") || 0,
    "timestamp": formatTimestamp(),
    "firmware": "focas",
    "ip": "192.168.11.11",
    "wifi": global.get("wifi_name")|| "na",
    "uptime": global.get("uptime") || 0,
    "rssi": "0",
    "A1":Number(data["machine_state_run"]), //machine status
    "A2":Number(data["job_counter_value"]), //production count
    "A3":Number(data["cycle_time"][0] * 60), //cycle time
    "A4":Number(data["nc_program"].program_number), // 
    "A5":Number(data["part_counter_value"]),
    "A6":Number(data["run_value"]),
    "A7":Number(data["power_on_time"][0] * 60), // machine on time
    "A8":Number(data["cutting_time"][0] * 60) , //
    "A9":Number(data["machine_state_auto"]), // 
    "A10":Number(data["machine_state_emergency"]), //Emergency status
    "A11":Number(data["machine_state_alarm"]), //Machine alarm
}

```

| Status |Sr No | Monitoring Parameter | Data Source |
|--|--|--|--|
|[x]|1   |   Machine Status	        |   Machine                             |
|[x]|2   |   Cycle Status	        |   Machine                             |
|[x]|3   |   Machine Ontime	        |   Machine                             |
|[x]|4   |   Productive Time	    |   Machine                             |
|[]|5   |   Idle Time	            |   Machine                             |
|[x]|6   |   Cycle Time	            |   Machine                             |
|[]|7   |   Part Change Time	    |   Machine                             |
|[x]|8   |   Machine Mode	        |   Machine                             |
|[]|9   |   Reset Count	            |   Machine                             |
|[]|10  |	Spindle Ontime	        |   Machine                             |
|[x]|11  |	Emergency Status	    |   Machine                             |
|[x]|12  |	Machine Fault	        |   Machine                             |
|[x]|13  |	Machine Alarms	        |   Machine                             |
|[x]|14  |	Production Count	    |   Machine                             |
|[]|15  |	Tool No	                |   Machine                             |
|[]|16  |	Spindlerate Overrite	|   Machine                             |
|[]|17  |	Feedrate Override	    |   Machine                             |
|[]|18  |	Machine Downtime reason	|   Data Entry/ Barcode                 |
|[]|19  |	Part Name	            |   Data Entry/ Barcode/ Machine SOP    |
|[]|20  |	Operator Name	        |   Data Entry/ Barcode/ Machine SOP    |

