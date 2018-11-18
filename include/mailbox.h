#ifndef MAILBOX_H
#define MAILBOX_H

#include "stdint.h"
#include "gpio.h"

#define MAILBOX_READ (MAILBOX_BASE)
#define MAILBOX_STATUS (MAILBOX_BASE + 0x18)
#define MAILBOX_WRITE (MAILBOX_BASE + 0x20)

uint32_t mailbox_read(uint32_t channel);
void mailbox_send(uint32_t msg, uint32_t channel);

#endif