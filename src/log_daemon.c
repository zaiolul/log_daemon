#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <syslog.h>

#include "utils.h"
#include "tuya-utils.h"
#include "tuya_error_code.h"
#include "tuyalink_core.h"

static struct argp argp = { options, parse_opt, 0, doc };

tuya_mqtt_context_t client_instance;

void sig_handler(int signum);

int run = 1;

int main(int argc, char *argv[])
{ 
    /*get program arguments*/
    struct arguments arguments;
    arguments.daemon = 0;
    arguments.device_id = NULL;
    arguments.product_id = NULL;
    arguments.secret = NULL;

    argp_parse (&argp, argc, argv, 0, 0, &arguments);

    /*register interrupt*/
    signal(SIGINT, sig_handler);
    
    /*start log*/
    openlog("log_daemon", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL0);
    setlogmask (LOG_UPTO (LOG_INFO));

    /*make program a daemon*/
    if(arguments.daemon){
        int status = daemonize();
        if(status < 0){
            syslog(LOG_ERR, "Cannot create daemon");
            return -1;
        }
    }
    
    tuya_mqtt_context_t* client = &client_instance;
    
    int ret = client_init(client, arguments.device_id, arguments.secret);
    if(ret != 0)
        return ret;

    char command[] = "ps -eo ppid= | wc -l"; // used to get data for report
    char report[1024];
    
    while(run){
        if(create_report_string(report, command) > 0) // data sent to the cloud
            syslog(LOG_WARNING, "Cannot create report string");
        
        /*maintain connection*/
        if((ret = tuya_mqtt_loop(client)) != OPRT_OK){
            syslog(LOG_ERR, "Cannot maintain connection");
            return ret;
        }
        
        send_report(client, arguments.device_id, report);
    }

    /*disconnect device*/
    client_deinit(client);

    return 0;
}

void sig_handler(int signum)
{
    syslog(LOG_WARNING, "Received interrupt signal");
    run = 0;
}

