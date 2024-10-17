#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../../fwlib32.h"
#define DEFAULT_MACHINE_HOST "127.0.0.1"
#define MACHINE_PORT 8193

int main()
{
    int allocated = 0;
    int ret = 0;
    unsigned short libh;
    char cnc_id[40] = "";
    uint32_t cnc_ids[4];

    const char *MACHINE_HOST = getenv("MACHINE_HOST");
    if (MACHINE_HOST == NULL)
    {
        MACHINE_HOST = DEFAULT_MACHINE_HOST;
    }

#ifndef _WIN32
    if (cnc_startupprocess(0, "focas.log") != EW_OK)
    {
        fprintf(stderr, "Failed to create required log file!\n");
        return 1;
    }
#endif

    printf("connecting to machine at %s:%d...\n", MACHINE_HOST, MACHINE_PORT);
    if ((ret = cnc_allclibhndl3(MACHINE_HOST, MACHINE_PORT, 10, &libh)) != EW_OK)
    {
        fprintf(stderr, "Failed to connect to cnc! (%d)\n", ret);
        ret = 1;
        goto cleanup;
    }
    allocated = 1;

    if (cnc_rdcncid(libh, (unsigned long *)cnc_ids) != EW_OK)
    {
        fprintf(stderr, "Failed to read cnc id!\n");
        ret = 1;
        goto cleanup;
    }

    snprintf(cnc_id, 40, "%08x-%08x-%08x-%08x", cnc_ids[0], cnc_ids[1],
             cnc_ids[2], cnc_ids[3]);

    // Define variables to store the required values
    ODBST machine_status;
    IODBPSD machine_ontime, productive_time, idle_time, cycle_time, part_change_time, reset_count, spindle_ontime, production_count, tool_no, spindlerate_override, feedrate_override, cycle_status;
    ODBALM machine_alarms[10]; // Adjust the number of alarms as needed

    short num_alarms = 10; // Adjust the number of alarms as needed

    short num_alarms = 10; // Adjust the number of alarms as needed

    // Read the values using appropriate functions
    cnc_statinfo(libh, &machine_status);
    cnc_rdparam(libh, 6711, 0, 8, &machine_ontime);
    cnc_rdparam(libh, 6712, 0, 8, &productive_time);
    cnc_rdparam(libh, 6713, 0, 8, &idle_time);
    cnc_rdparam(libh, 6714, 0, 8, &cycle_time);
    cnc_rdparam(libh, 6715, 0, 8, &part_change_time);
    cnc_rdparam(libh, 6716, 0, 8, &cycle_status);
    cnc_rdparam(libh, 6717, 0, 8, &reset_count);
    cnc_rdparam(libh, 6718, 0, 8, &spindle_ontime);
    cnc_rdparam(libh, 6719, 0, 8, &machine_status.emergency);
    cnc_rdparam(libh, 6720, 0, 8, &machine_status.alarm);
    cnc_rdalmmsg(libh, -1, &num_alarms, machine_alarms);
    cnc_rdparam(libh, 6721, 0, 8, &production_count);
    cnc_rdparam(libh, 6722, 0, 8, &tool_no);
    cnc_rdparam(libh, 6723, 0, 8, &spindlerate_override);
    cnc_rdparam(libh, 6724, 0, 8, &feedrate_override);

    // Manually construct the JSON string
    char json_output[2048];
    snprintf(json_output, sizeof(json_output),
             "{"
             "\"Machine Status\": {"
             "\"hdck\": %d, "
             "\"tmmode\": %d, "
             "\"aut\": %d, "
             "\"run\": %d, "
             "\"motion\": %d, "
             "\"mstb\": %d, "
             "\"emergency\": %d, "
             "\"alarm\": %d, "
             "\"edit\": %d"
             "}, "
             "\"Cycle Status\": %d, "
             "\"Machine Ontime\": %ld, "
             "\"Productive Time\": %ld, "
             "\"Idle Time\": %ld, "
             "\"Cycle Time\": %ld, "
             "\"Part Change Time\": %ld, "
             "\"Machine Mode\": %d, "
             "\"Reset Count\": %ld, "
             "\"Spindle Ontime\": %ld, "
             "\"Emergency Status\": %d, "
             "\"Machine Fault\": %d, "
             "\"Machine Alarms\": [",
             machine_status.hdck, machine_status.tmmode, machine_status.aut, machine_status.run, machine_status.motion,
             machine_status.mstb, machine_status.emergency, machine_status.alarm, machine_status.edit,
             cycle_status.u.ldata, machine_ontime.u.ldata, productive_time.u.ldata, idle_time.u.ldata, cycle_time.u.ldata, part_change_time.u.ldata,
             reset_count.u.ldata, spindle_ontime.u.ldata, machine_status.emergency, machine_status.alarm);

    // Append machine alarms to JSON string
    for (int i = 0; i < num_alarms; i++)
    {
        char alarm_str[128];
        snprintf(alarm_str, sizeof(alarm_str), "{\"data\": %d}",
                 machine_alarms[i].data);
        strcat(json_output, alarm_str);
        if (i < num_alarms - 1)
        {
            strcat(json_output, ", ");
        }
    }

    strcat(json_output, "], ");
    snprintf(json_output + strlen(json_output), sizeof(json_output) - strlen(json_output),
             "\"Production Count\": %ld, "
             "\"Tool No\": %ld, "
             "\"Spindlerate Override\": %ld, "
             "\"Feedrate Override\": %ld"
             "}",
             production_count.u.ldata, tool_no.u.ldata, spindlerate_override.u.ldata, feedrate_override.u.ldata);

    // Print the JSON string
    printf("%s\n", json_output);

cleanup:
    if (allocated && cnc_freelibhndl(libh) != EW_OK)
        fprintf(stderr, "Failed to free library handle!\n");
#ifndef _WIN32
    cnc_exitprocess();
#endif

    printf("machine id: %s\n", cnc_id);
}
