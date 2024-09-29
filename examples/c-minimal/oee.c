#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../../fwlib32.h"

#define DEFAULT_MACHINE_HOST "192.168.11.12"
#define MACHINE_PORT 8193
#define JSON_BUFFER_SIZE 2048

// Function to print time in hh:mm:ss:ms format
void print_time(long minutes, long milliseconds, char *time_buffer) {
    long total_seconds = (minutes * 60) + (milliseconds / 1000);
    long hours = total_seconds / 3600;
    long minutes_left = (total_seconds % 3600) / 60;
    long seconds = total_seconds % 60;
    long ms = milliseconds % 1000;

    snprintf(time_buffer, 50, "%02ld:%02ld:%02ld:%03ld", hours, minutes_left, seconds, ms);
}

int main(int argc, char *argv[]) {
    unsigned short libh;
    int ret;
    uint32_t cnc_ids[4];
    ODBST statinfo;        // Machine status
    ODBPRO odbpro;         // NC program info
    IODBPSD part_counter;  // Part counter
    IODBPSD job_counter;  // job counter
    IODBPSD feed_rate;     // Feed rate
    ODBSPEED spindle_speed; // Spindle speed
    ODBALM alarm_data;     // Alarm status
    ODBPOS position_data[3]; // Position data for X, Y, Z
    ODBSPLOAD spindle_load; // Spindle load meter
    ODBSVLOAD servo_load;   // Servo load meter
    IODBPSD power_data;     // Power data
    char json_output[JSON_BUFFER_SIZE];

    // Initialize FOCAS library
    if (cnc_startupprocess(0, "focas.log") != EW_OK) {
        fprintf(stderr, "Failed to initialize FOCAS library!\n");
        return 1;
    }

    // Connect to CNC
    if ((ret = cnc_allclibhndl3(DEFAULT_MACHINE_HOST, MACHINE_PORT, 10, &libh)) != EW_OK) {
        fprintf(stderr, "Failed to connect to CNC! (%d)\n", ret);
        return 1;
    }

    // Read CNC ID
    if (cnc_rdcncid(libh, (unsigned long *)cnc_ids) != EW_OK) {
        fprintf(stderr, "Failed to read CNC ID!\n");
        cnc_freelibhndl(libh);
        return 1;
    }



    // MACHINE STATE: Retrieve machine status info
    if ((ret = cnc_statinfo(libh, &statinfo)) != EW_OK) {
        fprintf(stderr, "Failed to read machine status! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }

    // PART COUNTER: Retrieve part counter value
    if ((ret = cnc_rdparam(libh, 6712, 0, 8, &part_counter)) != EW_OK) {
        fprintf(stderr, "Failed to read part counter! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }

    // NC PROGRAM INFO: Retrieve running NC program number
    if ((ret = cnc_rdprgnum(libh, &odbpro)) != EW_OK) {
        fprintf(stderr, "Failed to read NC program number! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }

    // Read job count
    if ((ret = cnc_rdparam(libh, 6711, 0, 8, &job_counter)) != EW_OK) {
        fprintf(stderr, "Failed to read job count! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }
    // int job_count = iodbpsd.u.ldata;
    // printf("Job count: %d\n", job_count);

    // Read operating time
    IODBPSD operating_time;
    if ((ret = cnc_rdparam(libh, 6750, 0, 8, &operating_time)) != EW_OK) { // Example parameter number for operating time
        fprintf(stderr, "Failed to read operating time! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }
    // printf("Operating time: %d minutes\n", operating_time.u.ldata); 

    


    // FEED RATE: Retrieve feed rate
    // if ((ret = cnc_rdparam(libh, 5001, 0, 8, &feed_rate)) != EW_OK) {
    //     fprintf(stderr, "Failed to read feed rate! (%d)\n", ret);
    //     cnc_freelibhndl(libh);
    //     return 1;
    // }

    // SPINDLE SPEED: Retrieve spindle speed
    // if ((ret = cnc_rdspeed(libh, 1, &spindle_speed)) != EW_OK) {
    //     fprintf(stderr, "Failed to read spindle speed! (%d)\n", ret);
    //     cnc_freelibhndl(libh);
    //     return 1;
    // }

    // ALARM STATUS: Retrieve alarm information
    if ((ret = cnc_alarm(libh, &alarm_data)) != EW_OK) {
        fprintf(stderr, "Failed to read alarm status! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }

    // POSITION DATA: Retrieve current position data for X, Y, Z axes
    // short axis = 0; // Specify the axis if needed
    // for (int i = 0; i < 3; i++) {
    //     if ((ret = cnc_rdposition(libh, 0, i, &position_data[i])) != EW_OK) {
    //         fprintf(stderr, "Failed to read position data for axis %d! (%d)\n", i, ret);
    //         cnc_freelibhndl(libh);
    //         return 1;
    //     }
    // }

    // SPINDLE LOAD METER: Retrieve spindle load
    // if ((ret = cnc_rdspmeter(libh, 1, &spindle_load)) != EW_OK) {
    //     fprintf(stderr, "Failed to read spindle load meter! (%d)\n", ret);
    //     cnc_freelibhndl(libh);
    //     return 1;
    // }

    // SERVO LOAD METER DATA: Retrieve servo load for X, Y, Z axes
    // if ((ret = cnc_rdsvmeter(libh, 1, &servo_load)) != EW_OK) {
    //     fprintf(stderr, "Failed to read servo load meter! (%d)\n", ret);
    //     cnc_freelibhndl(libh);
    //     return 1;
    // }

    // POWER DATA: Retrieve power information (use appropriate parameter)
    // if ((ret = cnc_rdparam(libh, 6713, 0, 8, &power_data)) != EW_OK) {
    //     fprintf(stderr, "Failed to read power data! (%d)\n", ret);
    //     cnc_freelibhndl(libh);
    //     return 1;
    // }

    // Prepare JSON Output
    snprintf(json_output, JSON_BUFFER_SIZE,
        "{"
        "\"machine_state_auto\": \"%d\", "
        "\"machine_state_run\": \"%d\", "
        "\"machine_state_emergency\": \"%d\", "
        "\"machine_state_alarm\": \"%d\", "
        "\"nc_program\": { \"program_number\": %d }, "
        "\"job_counter_value\": %ld, "
        "\"part_counter_value\": %ld, "
        "\"run_value\": %ld, "
        // "\"feed_rate\": %ld, "
        // "\"spindle_speed\": %ld, "
        // "\"alarm_status\": \"%s\", "
        // "\"position_data\": { \"X\": %.3f, \"Y\": %.3f, \"Z\": %.3f }, "
        // "\"tool_data\": { \"tool_offset\": %.2f }, "
        // "\"spindle_load_meter\": %d, "
        // "\"servo_load_meter\": { \"X\": %d, \"Y\": %d, \"Z\": %d }, "
        // "\"power\": { \"voltage\": %ld, \"current\": %ld }"
        "}",
        statinfo.aut,  // Machine state (use the actual enum value)
        statinfo.run,  // Machine state (use the actual enum value)
        statinfo.emergency,  // Machine state (use the actual enum value)
        statinfo.alarm,  // Machine state (use the actual enum value)
        odbpro.data,  // NC Program number
        job_counter.u.ldata, //job counter
        part_counter.u.ldata,  // Part counter
        operating_time.u.ldata //
        // feed_rate.u.ldata,  // Feed rate
        // spindle_speed.actf,  // Spindle speed
        // alarm_data.data[0].almmsg,  // Alarm status
        // position_data[0].pos, position_data[1].pos, position_data[2].pos,  // Position data for X, Y, Z
        // tool_offset.u.ldata / 1000.0,  // Tool offset (replace with actual parameter)
        // spindle_load.spload,  // Spindle load meter
        // servo_load.svload[0], servo_load.svload[1], servo_load.svload[2],  // Servo load meter for X, Y, Z
        // power_data.u.ldata / 100, power_data.u.ldata % 100  // Power data (voltage, current)
    );

    // Output JSON to console
    printf("%s\n", json_output);

    // Free library handle
    if (cnc_freelibhndl(libh) != EW_OK) {
        fprintf(stderr, "Failed to free library handle!\n");
    }

    // Terminate FOCAS library
    cnc_exitprocess();

    return 0;
}
