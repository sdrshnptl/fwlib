#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../../fwlib32.h"

#define DEFAULT_MACHINE_HOST "192.168.11.12"
#define MACHINE_PORT 8193

void print_time(long total_minutes, long total_milliseconds) {
    long total_seconds = (total_minutes * 60) + (total_milliseconds / 1000);
    long hours = total_seconds / 3600;
    long minutes = (total_seconds % 3600) / 60;
    long seconds = total_seconds % 60;
    long milliseconds = total_milliseconds % 1000;

    printf("Formatted time: %02ld:%02ld:%02ld:%03ld\n", hours, minutes, seconds, milliseconds);
}

int main(int argc, char *argv[]) {
    unsigned short libh;
    int ret;
    uint32_t cnc_ids[4];
    ODBPRO odbpro;
    IODBPSD iodbpsd;
    ODBSPEED speed;
    IODBTIME timer; // Structure for cnc_rdtimer
    IODBTIME last_timer; // Structure for last job cycle time
    short spindle_number = 1;

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

    // Read current job cycle time
    if ((ret = cnc_rdtimer(libh, 3, &timer)) != EW_OK) { // 3 for cycle time
        fprintf(stderr, "Failed to read current job cycle time! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }
    printf("Current job cycle time: %ld minutes, %ld milliseconds\n", timer.minute, timer.msec);
    print_time(timer.minute, timer.msec); // Format and print the current cycle time

    // Read last job cycle time (assuming parameter 6715 holds last job cycle time)
    if ((ret = cnc_rdparam(libh, 6715, 0, 8, &last_timer)) != EW_OK) { // Example parameter number for last job cycle time
        fprintf(stderr, "Failed to read last job cycle time! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }
    printf("Last job cycle time: %ld minutes, %ld milliseconds\n", last_timer.minute, last_timer.msec);
    print_time(last_timer.minute, last_timer.msec); // Format and print the last cycle time

    // Free library handle
    if (cnc_freelibhndl(libh) != EW_OK) {
        fprintf(stderr, "Failed to free library handle!\n");
    }

    // Terminate FOCAS library
    cnc_exitprocess();

    return 0;
}

