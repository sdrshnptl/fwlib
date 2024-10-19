#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../../fwlib32.h"

#define DEFAULT_MACHINE_HOST "192.168.11.12"
#define MACHINE_PORT 8193
#define JSON_BUFFER_SIZE 16384

// Function to print time in hh:mm:ss:ms format
void print_time(long minutes, long milliseconds, char *time_buffer)
{
    long total_seconds = (minutes * 60) + (milliseconds / 1000);
    long hours = total_seconds / 3600;
    long minutes_left = (total_seconds % 3600) / 60;
    long seconds = total_seconds % 60;
    long ms = milliseconds % 1000;

    snprintf(time_buffer, 50, "%02ld:%02ld:%02ld:%03ld", hours, minutes_left, seconds, ms);
}

#define TOOL_LIFE_DATA_COUNT 10 // Adjust as needed

// void read_tool_life(unsigned short handle)
// {
//     ODBTLIFE2 tool_life_data[TOOL_LIFE_DATA_COUNT];
//     short data_count = TOOL_LIFE_DATA_COUNT; // Number of tool life data entries to read
//     short ret;

//     // Initialize the tool life data array
//     for (int i = 0; i < TOOL_LIFE_DATA_COUNT; ++i)
//     {
//         tool_life_data[i].dummy = 0;
//     }

//     // Read tool life data
//     ret = cnc_rdtoollife(handle, 1, 0, &data_count, tool_life_data);

//     if (ret == EW_OK)
//     {
//         // Print out the tool life data
//         for (int i = 0; i < data_count; ++i)
//         {
//             printf("Tool No: %d, Life: %d, Count: %d\n",
//                    tool_life_data[i].datano,
//                    tool_life_data[i].life,
//                    tool_life_data[i].count);
//         }
//     }
//     else
//     {
//         printf("Error reading tool life data: %d\n", ret);
//         cnc_freelibhndl(handle);
//     }
// }

int main(int argc, char *argv[])
{
    unsigned short libh;
    int ret;
    uint32_t cnc_ids[4];
    ODBST statinfo;          // Machine status
    ODBPRO odbpro;           // NC program info
    IODBPSD part_counter;    // Part counter
    IODBPSD job_counter;     // job counter
    IODBPSD feed_rate;       // Feed rate
    ODBSPEED spindle_speed;  // Spindle speed
    ODBALM alarm_data;       // Alarm status
    ODBPOS position_data[3]; // Position data for X, Y, Z
    ODBSPLOAD spindle_load;  // Spindle load meter
    ODBSVLOAD servo_load;    // Servo load meter
    IODBPSD power_data;      // Power data
    IODBPSD operating_time;  // Operating time

    IODBTIME power_on_time;  // 0
    IODBTIME operation_time; // 1
    IODBTIME cutting_time;   // 2
    IODBTIME cycle_time;     // 3
    IODBTIME free_time;      // 4

    char json_output[JSON_BUFFER_SIZE];

    // Initialize FOCAS library
    if (cnc_startupprocess(0, "focas.log") != EW_OK)
    {
        fprintf(stderr, "Failed to initialize FOCAS library!\n");
        return 1;
    }

    // Connect to CNC
    if ((ret = cnc_allclibhndl3(DEFAULT_MACHINE_HOST, MACHINE_PORT, 10, &libh)) != EW_OK)
    {
        fprintf(stderr, "Failed to connect to CNC! (%d)\n", ret);
        return 1;
    }

    // Read CNC ID
    if (cnc_rdcncid(libh, (unsigned long *)cnc_ids) != EW_OK)
    {
        fprintf(stderr, "Failed to read CNC ID!\n");
        cnc_freelibhndl(libh);
        return 1;
    }

    // MACHINE STATE: Retrieve machine status info
    if ((ret = cnc_statinfo(libh, &statinfo)) != EW_OK)
    {
        fprintf(stderr, "Failed to read machine status! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }

    // PART COUNTER: Retrieve part counter value
    if ((ret = cnc_rdparam(libh, 6712, 0, 8, &part_counter)) != EW_OK)
    {
        fprintf(stderr, "Failed to read part counter! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }

    // NC PROGRAM INFO: Retrieve running NC program number
    if ((ret = cnc_rdprgnum(libh, &odbpro)) != EW_OK)
    {
        fprintf(stderr, "Failed to read NC program number! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }

    // Read job count
    if ((ret = cnc_rdparam(libh, 6711, 0, 8, &job_counter)) != EW_OK)
    {
        fprintf(stderr, "Failed to read job count! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }
    // int job_count = iodbpsd.u.ldata;
    // printf("Job count: %d\n", job_count);

    // Read operating time
    
    if ((ret = cnc_rdparam(libh, 6750, 0, 8, &operating_time)) != EW_OK)
    { // Example parameter number for operating time
        fprintf(stderr, "Failed to read operating time! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }
    // printf("Operating time: %d minutes\n", operating_time.u.ldata);

    if ((ret = cnc_rdtimer(libh, 0, &power_on_time)) != EW_OK)
    {
        fprintf(stderr, "Failed to read power on time! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }

    if ((ret = cnc_rdtimer(libh, 1, &operation_time)) != EW_OK)
    {
        fprintf(stderr, "Failed to read operation time! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }

    if ((ret = cnc_rdtimer(libh, 2, &cutting_time)) != EW_OK)
    {
        fprintf(stderr, "Failed to read cutting time! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }

    if ((ret = cnc_rdtimer(libh, 3, &cycle_time)) != EW_OK)
    {
        fprintf(stderr, "Failed to read cycle time! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }

    if ((ret = cnc_rdtimer(libh, 4, &free_time)) != EW_OK)
    {
        fprintf(stderr, "Failed to read free time! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }

    // ALARM STATUS: Retrieve alarm information
    if ((ret = cnc_alarm(libh, &alarm_data)) != EW_OK)
    {
        fprintf(stderr, "Failed to read alarm status! (%d)\n", ret);
        cnc_freelibhndl(libh);
        return 1;
    }




    // Prepare JSON Output
    snprintf(json_output, JSON_BUFFER_SIZE,
             "{"
             "\"machine_id\": [%lu,%lu,%lu,%lu], "
             "\"alarm\": %d, "
             "\"machine_state_auto\": %d, "
             "\"machine_state_run\": %d, "
             "\"machine_state_emergency\": %d, "
             "\"machine_state_alarm\": %d, "
             "\"nc_program\": { \"program_number\": %d }, "
             "\"job_counter_value\": %ld, "
             "\"part_counter_value\": %ld, "
             "\"run_value\": %ld, "
             "\"power_on_time\":[%ld,%ld], "
             "\"operation_time\":[%ld,%ld], "
             "\"cutting_time\":[%ld,%ld], "
             "\"cycle_time\":[%ld,%ld], "
             "\"free_time\":[%ld,%ld] "
            //  "\"tool0\":[%ld,%ld,%ld], "
            //  "\"tool1\":[%ld,%ld,%ld], "
            //  "\"tool2\":[%ld,%ld,%ld], "
            //  "\"tool3\":[%ld,%ld,%ld], "
            //  "\"tool4\":[%ld,%ld,%ld], "
            //  "\"tool5\":[%ld,%ld,%ld], "
            //  "\"tool6\":[%ld,%ld,%ld], "
            //  "\"tool7\":[%ld,%ld,%ld], "
            //  "\"tool8\":[%ld,%ld,%ld], "
            //  "\"tool9\":[%ld,%ld,%ld], "
            //  "\"tool10\":[%ld,%ld,%ld], "

             "}",
             cnc_ids[0], cnc_ids[1], cnc_ids[2], cnc_ids[3],
             alarm_data.data,
             statinfo.aut,           // Machine state (use the actual enum value)
             statinfo.run,           // Machine state (use the actual enum value)
             statinfo.emergency,     // Machine state (use the actual enum value)
             statinfo.alarm,         // Machine state (use the actual enum value)
             odbpro.data,            // NC Program number
             job_counter.u.ldata,    // job counter
             part_counter.u.ldata,   // Part counter
             operating_time.u.ldata, // operating time
             power_on_time.minute, power_on_time.msec, // Power on time
             operation_time.minute, operation_time.msec, // Operation time
             cutting_time.minute, cutting_time.msec, // Cutting time
             cycle_time.minute, cycle_time.msec, // Cycle time
             free_time.minute, free_time.msec// Free time
            //  tool_life_data[0].data->ntool,tool_life_data[0].data->life,tool_life_data[0].data->count,
            //  tool_life_data[1].data->ntool,tool_life_data[1].data->life,tool_life_data[1].data->count,
            //  tool_life_data[2].data->ntool,tool_life_data[2].data->life,tool_life_data[2].data->count,
            //  tool_life_data[3].data->ntool,tool_life_data[3].data->life,tool_life_data[3].data->count,
            //  tool_life_data[4].data->ntool,tool_life_data[4].data->life,tool_life_data[4].data->count,
            //  tool_life_data[5].data->ntool,tool_life_data[5].data->life,tool_life_data[5].data->count,
            //  tool_life_data[6].data->ntool,tool_life_data[6].data->life,tool_life_data[6].data->count,
            //  tool_life_data[7].data->ntool,tool_life_data[7].data->life,tool_life_data[7].data->count,
            //  tool_life_data[8].data->ntool,tool_life_data[8].data->life,tool_life_data[8].data->count,
            //  tool_life_data[9].data->ntool,tool_life_data[9].data->life,tool_life_data[9].data->count,
            //  tool_life_data[10].data->ntool,tool_life_data[10].data->life,tool_life_data[10].data->count
    );

    // Output JSON to console
    printf("%s\n", json_output);

    // Free library handle
    if (cnc_freelibhndl(libh) != EW_OK)
    {
        fprintf(stderr, "Failed to free library handle!\n");
    }

    // Terminate FOCAS library
    cnc_exitprocess();

    return 0;
}
