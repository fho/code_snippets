#include <stdlib.h>
#include<stdio.h>
#include<string.h>

//Last modified: 12/12/10 17:10:52(CET) by Fabian Holler

int pidof(const char* process_name){
    char cmd[50] ="pidof ";

        strncat(cmd,process_name, sizeof(cmd) - strlen(cmd) -1);
    FILE* fpidof = popen(cmd, "r");
    char spid[10];
    if(fpidof == NULL){
        perror("popen error");
        return -1;
    }
    if(fscanf(fpidof, "%s", spid) == EOF)
        return -1;
    pclose(fpidof);

    return atoi(spid);
}
