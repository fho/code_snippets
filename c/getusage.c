//Last modified: 17/10/11 18:52:02(CEST) by Fabian Holler
#include <stdlib.h> 
#include <sys/types.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

struct pstat{
    long unsigned int utime_ticks;
    long int cutime_ticks;
    long unsigned int stime_ticks;
    long int cstime_ticks;
    long unsigned int vsize; // virtual memory size in bytes
    long unsigned int rss; //Resident  Set  Size in bytes

    long unsigned int cpu_total_time;
};

/*
 * read /proc data into the passed struct pstat
 * returns 0 on success, -1 on error
*/
int get_usage(const pid_t pid, struct pstat* result){
    //convert  pid to string
    char pid_s[20];
    snprintf(pid_s, sizeof(pid_s), "%d", pid);
    char stat_filepath[30] = "/proc/";
    strncat(stat_filepath, pid_s, sizeof(stat_filepath) - strlen(stat_filepath) -1);
    strncat(stat_filepath, "/stat", sizeof(stat_filepath) - strlen(stat_filepath) -1);

    //open /proc/pid/stat
    FILE *fpstat = fopen(stat_filepath, "r");
    if(fpstat == NULL){
        printf("FOPEN ERROR pid stat %s:\n", stat_filepath);
        return -1;
    }

    //open /proc/stat
    FILE *fstat = fopen("/proc/stat", "r");
    if(fstat == NULL){
        printf("FOPEN ERROR");
        fclose(fstat);
        return -1;
    }
    bzero(result, sizeof(struct pstat));

    //read values from /proc/pid/stat
    long int rss;
    if(fscanf(fpstat, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %ld %ld %*d %*d %*d %*d %*u %lu %ld", &result->utime_ticks, &result->stime_ticks, &result->cutime_ticks, &result->cstime_ticks, &result->vsize, &rss) == EOF){
        fclose(fpstat);
        fclose(fstat);
        return -1;
    }
    fclose(fpstat);
    result->rss = rss * getpagesize();

    //read+calc cpu total time from /proc/stat, on linux 2.6.35-23 x86_64 the cpu row has 10values could differ on different architectures :/
    long unsigned int cpu_time[10];
    bzero(cpu_time, sizeof(cpu_time));
    if(fscanf(fstat, "%*s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu", &cpu_time[0], &cpu_time[1], &cpu_time[2], &cpu_time[3], &cpu_time[4], &cpu_time[5], &cpu_time[6], &cpu_time[7], &cpu_time[8], &cpu_time[9]) == EOF){
        fclose(fstat);
        return -1;
    }
    fclose(fstat);

    for(int i=0; i < 10;i++){
        result->cpu_total_time += cpu_time[i];
    }

    return 0;
}

/*
* calculates the actual CPU usage(cur_usage - last_usage) in percent
* cur_usage, last_usage: both last measured get_usage() results
* ucpu_usage, scpu_usage: result parameters: user and sys cpu usage in %
*/
void calc_cpu_usage(const struct pstat* cur_usage, const struct pstat* last_usage, float* ucpu_usage, float* scpu_usage){
    *ucpu_usage = 100 * ((((cur_usage->utime_ticks + cur_usage->cutime_ticks) - (last_usage->utime_ticks + last_usage->cutime_ticks))) / (float)((cur_usage->cpu_total_time - last_usage->cpu_total_time)));
    *scpu_usage = 100 * ((((cur_usage->stime_ticks + cur_usage->cstime_ticks) - (last_usage->stime_ticks + last_usage->cstime_ticks))) / (float)((cur_usage->cpu_total_time - last_usage->cpu_total_time)));
}

