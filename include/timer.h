/* Based on Jake Sandler implementation */

#ifndef TIMER_H
#define TIMER_H

void timer_init(void);
void sleep(uint32_t seconds);
void msleep(uint32_t mseconds);
void usleep(uint32_t useconds);
void timer_set(uint32_t usecs);

#endif