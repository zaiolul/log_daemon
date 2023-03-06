#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include "cJSON.h"
#include <argp.h>
#include "utils.h"

error_t parse_opt (int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;

    switch (key){
    case 'a':
        arguments->daemon = 1;

    case 'p':
        arguments->product_id = arg;
        break;

    case 'd':
        arguments->device_id = arg;
        break;

    case 's':
        arguments->secret = arg;
        break;
    case ARGP_KEY_END:
    {
        printf("%s %s %s\n", arguments->device_id, arguments->product_id, arguments->secret);
        if(arguments->device_id == NULL || arguments->product_id == NULL || arguments->secret == NULL){
            argp_failure(state, 1, 0, "Product id, device id and device secret are all required\nCheck with --usage option");
        }
        break;
    }
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}


int create_report_string(char *dest, char *command)
{
    char buf[256];
    FILE *fd;
    fd = popen(command, "r");
    if(fd == NULL){
        syslog(LOG_WARNING,"Failed to run command. Cannot create report string.\n");
        return 1;
    }
    fgets(buf, sizeof(buf),fd);
    printf("%s", buf);
    pclose(fd);
    sprintf(dest, "{\"upload\":{\"value\":\"%s\"}}", buf);
    return 0;
}

int daemonize()
{
    int pid;

    if((pid = fork()) > 0){
        exit(0);
    } else if(pid == -1){
        return -1;
    }
    
    if(setsid() == -1)               
        return -1;

    if((pid = fork()) > 0){
        exit(0);
    } else if(pid == -1){
        return -1;
    }

    umask(0);
    chdir("/");
    
    close(STDIN_FILENO);

    int fd = open("/dev/null", O_RDWR);
    if(fd != STDIN_FILENO)
      return -1;
    if(dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
      return -2;
    if(dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
      return -3;

    return 0;
}

int write_to_file(char* parameter, char* message, char* filename)
{
    cJSON *json = cJSON_Parse(message);
    char* value = json->child->valuestring;
    char* param = json->child->string;

    if(strcmp(param, parameter) == 0){
        FILE* fptr = fopen(filename, "a");
        if(fptr == NULL){
            syslog(LOG_ERR, "Cannot open file for writing");
            return -1;
        }
        else{
            time_t seconds;
            time(&seconds);

            fprintf(fptr, "Timestamp: %ld, value: %s\n",seconds, value);
            fclose(fptr);
        }
    }
    cJSON_Delete(json);
    return 0;
}

