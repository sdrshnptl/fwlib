#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../../fwlib32.h"

#define DEFAULT_MACHINE_HOST "192.168.11.12"
#define MACHINE_PORT 8193

// Function to print time in hh:mm:ss:ms format
void print_time(long minutes, long milliseconds) {
    long total_seconds = (minutes * 60) + (milliseconds / 1000);
    long hours = total_seconds / 3600;
    long minutes_left = (total_seconds % 3600) / 60;
    long seconds = total_seconds % 60;
    long ms = milliseconds % 1000;

    printf("%02ld:%02ld:%02ld:%03ld\n", hours, minutes_left, seconds, ms);
}

int main(int argc, char *argv[]) {
    unsigned short libh;
    int ret;
    uint32_t cnc_ids[4];
    ODBPRO odbpro;
    ODBPSD job_count, total_parts, good_parts, operating_time, last_job_cycle_time;

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
    if ((ret = cnc_rdparam(libh, 6711, 0, 8, &job_count)) != EW_OK) {
        fprintf(stderr, "Failed to read job count! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }
    printf("Job count: %ld\n", job_count.u.ldata);

    // Read operating time
    if ((ret = cnc_rdparam(libh, 6750, 0, 8, &operating_time)) != EW_OK) { // Example parameter number for operating time
        fprintf(stderr, "Failed to read operating time! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }
    printf("Operating time: %ld minutes\n", operating_time.u.ldata);

    // Read total parts produced
    if ((ret = cnc_rdparam(libh, 6712, 0, 8, &total_parts)) != EW_OK) {
        fprintf(stderr, "Failed to read total parts produced! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }
    printf("Total parts produced: %ld\n", total_parts.u.ldata);

    // Read good parts produced
    if ((ret = cnc_rdparam(libh, 6713, 0, 8, &good_parts)) != EW_OK) {
        fprintf(stderr, "Failed to read good parts produced! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }
    printf("Good parts produced: %ld\n", good_parts.u.ldata);

    // Read last job cycle time
    if ((ret = cnc_rdparam(libh, 6715, 0, 8, &last_job_cycle_time)) != EW_OK) { // Correct parameter for last job cycle time
        fprintf(stderr, "Failed to read last job cycle time! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }
    printf("Last job cycle time: ");
    print_time(last_job_cycle_time.u.ldata / 60, last_job_cycle_time.u.ldata % 1000); // Change to appropriate format

    // Calculate and display OEE
    if (operating_time.u.ldata > 0 && total_parts.u.ldata > 0) {
        double availability = (double)operating_time.u.ldata / 480.0; // Assuming 480 minutes planned production time
        double performance = (1.0 * total_parts.u.ldata) / (double)operating_time.u.ldata; // Assuming ideal cycle time is 1 minute
        double quality = (double)good_parts.u.ldata / total_parts.u.ldata;
        double oee = availability * performance * quality * 100;

        printf("OEE: %.2f%%\n", oee);
    } else {
        printf("OEE calculation skipped due to insufficient data.\n");
    }

    // Free library handle
    if (cnc_freelibhndl(libh) != EW_OK) {
        fprintf(stderr, "Failed to free library handle!\n");
    }

    // Terminate FOCAS library
    cnc_exitprocess();

    return 0;
}

