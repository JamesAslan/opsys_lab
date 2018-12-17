#ifndef INCLUDE_MAIL_BOX_
#define INCLUDE_MAIL_BOX_

#include "lock.h"
#include "queue.h"
#include "cond.h"

#define MAILBOX_MAX_CONTAIN 100
#define MAILBOX_MAX_NAME 20
typedef struct mailbox
{
    int contain;
    int use;
    char name[MAILBOX_MAX_NAME];
    char buffer[MAILBOX_MAX_CONTAIN];
    mutex_lock_t mutex;
    condition_t condition_full;
    condition_t condition_empty;
} mailbox_t;


void mbox_init();
mailbox_t *mbox_open(char *);
void mbox_close(mailbox_t *);
void mbox_send(mailbox_t *, void *, int);
void mbox_recv(mailbox_t *, void *, int);

#endif