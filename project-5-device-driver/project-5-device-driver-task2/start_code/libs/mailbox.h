#ifndef INCLUDE_MAIL_BOX_
#define INCLUDE_MAIL_BOX_

#include "lock.h"
#include "queue.h"
#include "sem.h"

#define MAILBOX_MAX_CONTAIN 100
#define MAILBOX_MAX_NAME 30
typedef struct mailbox
{
    int contain;
    int use;
    char name[MAILBOX_MAX_NAME];
    char buffer[10][4];
    semaphore_t empty;
    semaphore_t mutex;
    semaphore_t full;
} mailbox_t;


void mbox_init();
mailbox_t *mbox_open(char *);
void mbox_close(mailbox_t *);
void mbox_send(mailbox_t *, void *, int);
void mbox_recv(mailbox_t *, void *, int);

#endif