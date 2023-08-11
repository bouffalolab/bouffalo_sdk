#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>

#define SECTION_TIME_MAX 16
struct registed_stat {
    char *name;
    uint64_t begin;
    uint64_t cost;
    uint64_t task_time;
    uint64_t background_time;
    uint64_t trap_counter;
    uint64_t msec;
};
static struct registed_stat section_time[SECTION_TIME_MAX];

int freertos_helper_section_time_register(char *name, int msec)
{
    int i;
    for(i = 0; i < SECTION_TIME_MAX; i++)
	if(section_time[i].name == NULL)
	    break;
    if(i == SECTION_TIME_MAX)
	return -1;
    section_time[i].name = name;
    section_time[i].begin = 0;
    section_time[i].cost = 0;
    section_time[i].task_time = 0;
    section_time[i].background_time = 0;
    section_time[i].msec = msec;
    return i;
}

inline static uint64_t msec2cycle(uint64_t msec)
{
    return msec * (configCPU_CORE_CLOCK_HZ / 1000);
}

void freertos_helper_section_time_begin(int index, float *result)
{
    uint64_t begin, background;

    if(index < 0)
	return;
    vGetTaskRunningTimeSnapshot(&begin, &background);
    if(section_time[index].begin != 0)
	section_time[index].task_time += begin - section_time[index].begin;
    section_time[index].begin = begin;

    if(section_time[index].background_time == 0) {
	section_time[index].background_time = background;
    } else if (background - section_time[index].background_time > msec2cycle(section_time[index].msec)) {
	/* msec arrived */
	*result = section_time[index].cost / (float) section_time[index].task_time * 100;
    }
}

void freertos_helper_section_time_end(int index)
{
    uint64_t end, background;

    if(index < 0)
	return;
    vGetTaskRunningTimeSnapshot(&end, &background);
    section_time[index].cost += end - section_time[index].begin;
}
