#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <syslog.h>

#include "utils.h"
#include "cJSON.h"
#include "tuya_cacert.h"
#include "tuya_error_code.h"
#include "system_interface.h"
#include "mqtt_client_interface.h"
#include "tuyalink_core.h"

static char doc[] = "Log daemon program. Connects to Tuya cloud service, sends and receives data.";

static char args_doc[] = "[PRODUCT_ID] [DEVICE_ID] [DEVICE_SECRET]";

struct arguments{
  char *args[3];         
};

/* Parse a single option. */
static error_t parse_opt (int key, char *arg, struct argp_state *state);

static struct argp argp = { 0, parse_opt, args_doc, doc };

char productId[30];
char deviceId[30];
char deviceSecret[30];

tuya_mqtt_context_t client_instance;

void on_connected(tuya_mqtt_context_t* context, void* user_data);

void on_disconnect(tuya_mqtt_context_t* context, void* user_data);

void on_messages(tuya_mqtt_context_t* context, void* user_data, const tuyalink_message_t* msg);

void sig_handler(int signum);

int run = 1;

int main(int argc, char *argv[])
{   
    /*get program arguments*/
    struct arguments arguments;
    argp_parse (&argp, argc, argv, 0, 0, &arguments);

    strncpy(productId, arguments.args[0], 30);
    strncpy(deviceId, arguments.args[1], 30);
    strncpy(deviceSecret, arguments.args[2], 30);

    /*register interrupt*/
    signal(SIGINT, sig_handler);
    
    /*start log*/
    openlog("log_daemon", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL0);
    setlogmask (LOG_UPTO (LOG_INFO));

    syslog(LOG_WARNING, "Log Daemon Start");
    /*make program a daemon*/
    int status = daemonize();
    if(status < 0)
    {
        syslog(LOG_ERR, "Cannot create daemon");
        return -1;
    }

    tuya_mqtt_context_t* client = &client_instance;
    
    /* initialize the client */
    int ret = tuya_mqtt_init(client, &(const tuya_mqtt_config_t) {
        .host = "m1.tuyacn.com",
        .port = 8883,
        .cacert = tuya_cacert_pem,
        .cacert_len = sizeof(tuya_cacert_pem),
        .device_id = deviceId,
        .device_secret = deviceSecret,
        .keepalive = 100,
        .timeout_ms = 5000,
        .on_connected = on_connected,
        .on_disconnect = on_disconnect,
        .on_messages = on_messages
    });

    if(ret != OPRT_OK)
    {
        syslog(LOG_ERR, "Failed to initialize");
        return ret;
    }
    
    /*try to connect*/
    if((ret = tuya_mqtt_connect(client)) != OPRT_OK)
    {
        syslog(LOG_ERR, "Failed to connect");
        return ret;
    }

    char command[] = "ps -eo ppid= | wc -l"; // used to get data for report
    char report[1024];
    
    while(run){

        if(create_report_string(report, command) > 0) // data sent to the cloud
            syslog(LOG_WARNING, "Cannot create report string");
        
        /*maintain connection*/
        if((ret = tuya_mqtt_loop(client)) != OPRT_OK)
        {
            syslog(LOG_ERR, "Cannot maintain connection");
            return ret;
        }

         /*send data to cloud*/
        if((ret = tuyalink_thing_property_report_with_ack(client, deviceId, report)) == OPRT_INVALID_PARM)
        {
            syslog(LOG_ERR, "Cannot send report");
            return ret;
        }
        syslog(LOG_INFO, "Report message sent: %s", report);
    }

    /*disconnect device*/
    tuya_mqtt_disconnect(client); 
    tuya_mqtt_deinit(client); //free memory
    syslog(LOG_WARNING, "Daemon terminated");

    return 0;
}

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;

    switch (key){
    case ARGP_KEY_ARG:
        if (state->arg_num >= 3)
        argp_usage (state);
        arguments->args[state->arg_num] = arg;
        break;

    case ARGP_KEY_END:
        if (state->arg_num < 3)
        /* Not enough arguments. */
        argp_usage (state);
        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

void on_connected(tuya_mqtt_context_t* context, void* user_data)
{
    syslog(LOG_INFO,"Client connected");
}

void on_disconnect(tuya_mqtt_context_t* context, void* user_data)
{
    syslog(LOG_INFO,"Client disconnected");
}
void on_messages(tuya_mqtt_context_t* context, void* user_data, const tuyalink_message_t* msg)
{
    switch (msg->type) {
        case THING_TYPE_PROPERTY_REPORT_RSP:
            syslog(LOG_INFO, "Cloud received and replied: id:%s, type:%d", msg->msgid, msg->type);
            break;

        case THING_TYPE_PROPERTY_SET:
            syslog(LOG_INFO, "Device received id:%s, type:%d, text:%s", msg->msgid, msg->type, msg->data_string);
            write_to_file("write", msg->data_string, "/var/tmp/from-cloud");
         
            break;

        default:
            break;
    }
}

void sig_handler(int signum)
{
    syslog(LOG_WARNING, "Received interrupt signal");
    run = 0;
}

