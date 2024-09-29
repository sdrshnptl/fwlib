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
int read_timer(unsigned short libh, short type, long *minute, long *msec, const char *timer_name) {
    IODBTIME timer;
    int ret = cnc_rdtimer(libh, type, &timer);
    if (ret != EW_OK) {
        fprintf(stderr, "Failed to read %s! (%d)\n", timer_name, ret);
        return ret;
    }
    *minute = timer.minute;
    *msec = timer.msec;
    printf("%s: %ld minutes, %ld milliseconds\n", timer_name, timer.minute, timer.msec);
    return EW_OK;
}

int main(int argc, char *argv[]) {
    unsigned short libh;
    int ret;
    uint32_t cnc_ids[4];
    ODBPRO odbpro;
    ODBSPEED speed;
    short spindle_number = 1;  // Try using 0 for the spindle number
    short data_type = 0;

    // Cycle time tracking
    int last_job_number = -1;
    long last_job_cycle_time_minutes = 0, last_job_cycle_time_msec = 0;
    long current_job_cycle_time_minutes = 0, current_job_cycle_time_msec = 0;

    // Define planned production time and ideal cycle time
    double planned_production_time = 480.0; // Example: 480 minutes (8 hours)
    double ideal_cycle_time = 5.0; // Example: 1 minute per part

    // Allow machine host and port to be set via command-line arguments
    const char *host = (argc > 1) ? argv[1] : DEFAULT_MACHINE_HOST;
    int port = (argc > 2) ? atoi(argv[2]) : MACHINE_PORT;

    // Initialize FOCAS library
    if (cnc_startupprocess(0, "focas.log") != EW_OK) {
        fprintf(stderr, "Failed to initialize FOCAS library!\n");
        return 1;
    }

    // Connect to CNC
    if ((ret = cnc_allclibhndl3(host, port, 10, &libh)) != EW_OK) {
        ODBERR err_no; // Fix: Use ODBERR structure for error details
        cnc_getdtailerr(libh, &err_no); // Pass ODBERR structure
        fprintf(stderr, "Failed to connect to CNC! (%d), Detail Error: %d\n", ret, err_no.err_no);
        goto cleanup;
    }

    // Read CNC ID
    if (cnc_rdcncid(libh, (unsigned long *)cnc_ids) != EW_OK) {
        fprintf(stderr, "Failed to read CNC ID!\n");
        goto cleanup;
    }

    // Read running job
    if ((ret = cnc_rdprgnum(libh, &odbpro)) != EW_OK) {
        fprintf(stderr, "Failed to read running job! (%d)\n", ret);
        goto cleanup;
    }
    printf("Running job: %d\n", odbpro.data);

    // Check if the job has changed
    if (last_job_number != odbpro.data) {
        // Save the cycle time for the last job
        if (last_job_number != -1) {
            last_job_cycle_time_minutes = current_job_cycle_time_minutes;
            last_job_cycle_time_msec = current_job_cycle_time_msec;
            printf("Cycle time for last job %d: %ld minutes, %ld milliseconds\n", 
                    last_job_number, last_job_cycle_time_minutes, last_job_cycle_time_msec);
        }

        // Update last job number and reset current job cycle time
        last_job_number = odbpro.data;
        current_job_cycle_time_minutes = 0;
        current_job_cycle_time_msec = 0;
    }

    // Read current job cycle time
    read_timer(libh, 3, &current_job_cycle_time_minutes, &current_job_cycle_time_msec, "Current job cycle time");

    // Read job count
    int job_count;
    if (read_cnc_param(libh, 6711, &job_count) != EW_OK) {
        fprintf(stderr, "Failed to read job count!\n");
        goto cleanup;
    }
    printf("Job count: %d\n", job_count);

    // Read spindle speed
    if ((ret = cnc_rdspeed(libh, spindle_number, &speed)) != EW_OK) {
        fprintf(stderr, "Failed to read spindle speed! (%d)\n", ret);
        goto cleanup;
    }
    printf("Spindle speed: %d\n", speed.actf);

    // Read operating time
    int operating_time;
    read_timer(libh, 1, &current_job_cycle_time_minutes, &current_job_cycle_time_msec, "Operating time");

    // Read total parts produced
    int total_parts_produced;
    if (read_cnc_param(libh, 6712, &total_parts_produced) != EW_OK) {
        fprintf(stderr, "Failed to read total parts produced!\n");
        goto cleanup;
    }
    printf("Total parts produced: %d\n", total_parts_produced);

    // Read good parts produced
    int good_parts_produced;
    if (read_cnc_param(libh, 6713, &good_parts_produced) != EW_OK) {
        fprintf(stderr, "Failed to read good parts produced!\n");
        goto cleanup;
    }
    printf("Good parts produced: %d\n", good_parts_produced);

    // Calculate OEE with safety checks
    if (total_parts_produced > 0 && operating_time > 0) {
        double availability = (double)operating_time / planned_production_time;
        double performance = (ideal_cycle_time * total_parts_produced) / (double)operating_time;
        double quality = (double)good_parts_produced / total_parts_produced;
        double oee = availability * performance * quality * 100;
        printf("OEE: %.2f%%\n", oee);
    } else {
        printf("OEE calculation skipped due to insufficient data.\n");
    }

cleanup:
    // Free library handle and terminate FOCAS library
    if (cnc_freelibhndl(libh) != EW_OK) {
        fprintf(stderr, "Failed to free library handle!\n");
    }
    cnc_exitprocess();
    return ret;
}

