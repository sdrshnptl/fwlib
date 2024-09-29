#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../../fwlib32.h"

#define DEFAULT_MACHINE_HOST "192.168.11.12"
#define MACHINE_PORT 8193


// Helper function to read CNC parameters
int read_cnc_param(unsigned short libh, short param_no, int *result) {
    IODBPSD param_data;
    int ret = cnc_rdparam(libh, param_no, 0, 8, &param_data);
    if (ret == EW_OK) {
        *result = param_data.u.ldata;
    }
    return ret;
}

// Helper function to read a specific timer
intread_timer(unsigned short libh, short type, const char *timer_name) {
    IODBTIME timer;
    int ret = cnc_rdtimer(libh, type, &timer);
    if (ret != EW_OK) {
        fprintf(stderr, "Failed to read %s! (%d)\n", timer_name, ret);
        return ret;
    }
    printf("%s: %ld minutes, %ld milliseconds\n", timer_name, timer.minute, timer.msec);
    return EW_OK;
}




int main(int argc, char *argv[]) {
    unsigned short libh;
    int ret;
    uint32_t cnc_ids[4];
    ODBPRO odbpro;
    IODBPSD iodbpsd;
    ODBSPEED speed;
    short spindle_number = 1;
    short data_type = 0;

    // Define planned production time and ideal cycle time
    double planned_production_time = 480.0; // Example: 480 minutes (8 hours)
    double ideal_cycle_time = 1.0; // Example: 1 minute per part

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

    // Read running job
    if ((ret = cnc_rdprgnum(libh, &odbpro)) != EW_OK) {
        fprintf(stderr, "Failed to read running job! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }
    printf("Running job: %d\n", odbpro.data);

    // Read job count
    if ((ret = cnc_rdparam(libh, 6711, 0, 8, &iodbpsd)) != EW_OK) {
        fprintf(stderr, "Failed to read job count! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }
    int job_count = iodbpsd.u.ldata;
    printf("Job count: %d\n", job_count);

    // Read spindle speed
    if ((ret = cnc_rdspeed(libh, spindle_number, &speed)) != EW_OK) {
        fprintf(stderr, "Failed to read spindle speed! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }
    printf("Spindle speed: %d\n", speed.actf);

    // Read operating time
    IODBPSD operating_time;
    if ((ret = cnc_rdparam(libh, 6750, 0, 8, &operating_time)) != EW_OK) { // Example parameter number for operating time
        fprintf(stderr, "Failed to read operating time! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }
    printf("Operating time: %d minutes\n", operating_time.u.ldata);

        // Read various timers
    read_timer(libh, 0, "Power on time");
    read_timer(libh, 1, "Operating time");
    read_timer(libh, 2, "Cutting time");
    read_timer(libh, 3, "Cycle time");
    read_timer(libh, 4, "Free purpose time");
IODBTIME power_on_time; 
IODBTIME operation_time; 
IODBTIME cutting_time; 
IODBTIME cycle_time; 
IODBTIME free_time; 
    // Read total parts produced
    IODBPSD total_parts;
    if ((ret = cnc_rdparam(libh, 6712, 0, 8, &total_parts)) != EW_OK) {
        fprintf(stderr, "Failed to read total parts produced! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }
    int total_parts_produced = total_parts.u.ldata;
    printf("Total parts produced: %d\n", total_parts_produced);

    // Read good parts produced
    IODBPSD good_parts;
    if ((ret = cnc_rdparam(libh, 6713, 0, 8, &good_parts)) != EW_OK) {
        fprintf(stderr, "Failed to read good parts produced! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }
    int good_parts_produced = good_parts.u.ldata;
    printf("Good parts produced: %d\n", good_parts_produced);

    // Calculate OEE
    double availability = (double)operating_time.u.ldata / planned_production_time;
    double performance = (ideal_cycle_time * total_parts_produced) / (double)operating_time.u.ldata;
    double quality = (double)good_parts_produced / total_parts_produced;
    double oee = availability * performance * quality * 100;

    printf("OEE: %.2f%%\n", oee);

    // Free library handle
    if (cnc_freelibhndl(libh) != EW_OK) {
        fprintf(stderr, "Failed to free library handle!\n");
    }

    // Terminate FOCAS library
    cnc_exitprocess();

    return 0;
    snprintf(json_output, JSON_BUFFER_SIZE,
        "{"
        "\"machine_state\": \"%d\", "
        "\"nc_program\": { \"program_number\": %d }, "
        "\"job_counter_value\": %ld, "
        "\"part_counter_value\": %ld, "
        "\"run_value\": %ld, "
        "\"power_on_time\":[%ld,%ld], "
        "\"operation_time\":[%ld,%ld], "
        "\"cutting_time\":[%ld,%ld], "
        "\"cycle_time\":[%ld,%ld], "
        "\"free_time\":[%ld,%ld], "

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
        odbpro.data,  // NC Program number
        job_counter.u.ldata, //job counter
        part_counter.u.ldata,  // Part counter
        operating_time.u.ldata,
        power_on_time.minute,power_on_time.msec,
        operation_time.minute,operation_time.msec,
        cutting_time.minute,cutting_time.msec,
        cycle_time.minute,cycle_time.msec,
        free_time.minute,free_time.msec

        // feed_rate.u.ldata,  // Feed rate
        // spindle_speed.actf,  // Spindle speed
        // alarm_data.data[0].almmsg,  // Alarm status
        // position_data[0].pos, position_data[1].pos, position_data[2].pos,  // Position data for X, Y, Z
        // tool_offset.u.ldata / 1000.0,  // Tool offset (replace with actual parameter)
        // spindle_load.spload,  // Spindle load meter
        // servo_load.svload[0], servo_load.svload[1], servo_load.svload[2],  // Servo load meter for X, Y, Z
        // power_data.u.ldata / 100, power_data.u.ldata % 100  // Power data (voltage, current)
    );


}