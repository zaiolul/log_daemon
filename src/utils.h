#ifndef UTILS_H
#define UTILS_H

#include "tuyalink_core.h"
#include <argp.h>

/*program usage documentation*/
static char doc[] = "Log daemon program. Connects to Tuya cloud service, sends and receives data.";

/*argp options struct*/
static struct argp_option options[] = {
    {"product_id", 'p', "[PRODUCT ID]", 0, 0 },
    {"device_id", 'd', "[DEVICE ID]", 0, 0 },
    {"device_secret", 's', "[DEVICE SECRET]", 0},
    {"daemon", 'a', 0, 0},
    {0}
};
/*arguments struct*/
struct arguments{
    int daemon;
    char* secret;
    char* product_id;
    char* device_id;         
};
/*option parsing function*/
error_t parse_opt (int key, char *arg, struct argp_state *state);
/*run COMMAND and add the output to report string DEST*/
int create_report_string(char* dest, char* command);
/*make program a daemon*/
int daemonize();
/*gets value field from json MESSAGE and writes to FILENAME upon receiving PARAMETER set*/
int write_to_file(char* parameter, char* message, char* filename);

#endif