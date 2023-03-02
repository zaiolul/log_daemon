#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


int create_report_string(char dest[], char command[])
{
    char buf[256];
    FILE *fd;
    fd = popen(command, "r");
    if(fd == NULL){
        syslog(LOG_WARNING,"Failed to run command\n");
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
    
    //should be in child now;
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

