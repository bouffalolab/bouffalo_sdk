#ifndef __SECTION_TIME_STAT_H__
#define __SECTION_TIME_STAT_H__

/* register a instance that give a result in msec,
 * result is section_time / task_time
 */
int freertos_helper_section_time_register(char *name, int msec);

/* need ensure call begin and end by turns */
void freertos_helper_section_time_begin(int index, float *result);
void freertos_helper_section_time_end(int index);
int freertos_helper_section_time_stat_test_init(void);

#endif
