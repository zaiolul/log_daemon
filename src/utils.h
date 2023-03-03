/*run COMMAND and add the output to report string DEST*/
int create_report_string(char* dest, char* command);
/*make program a daemon*/
int daemonize();
/*gets value field from json MESSAGE and writes to FILENAME upon receiving PARAMETER set*/
int write_to_file(char* parameter, char* message, char* filename);