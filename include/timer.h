/* Based on Jake Sandler implementation */

#ifndef TIMER_H
#define TIMER_H

void timer_init(void);
void timer(uint32_t seconds);
void mtimer(uint32_t mseconds);
void utimer(uint32_t useconds);
void timer_set(uint32_t usecs);
void MsDelay (unsigned nMilliSeconds);
void usDelay (unsigned nMicroSeconds);

#endif