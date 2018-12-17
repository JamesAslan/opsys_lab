#include "string.h"
#include "mailbox.h"

#define MAX_NUM_BOX 32

static mailbox_t mboxs[MAX_NUM_BOX];

void mbox_init()
{
    int i;
    for(i=0;i<MAX_NUM_BOX;i++)
    {
        mboxs[i].use =0;
        mboxs[i].contain =0;
        mboxs[i].name[0]='\0';
        semaphore_init(&mboxs[i].full,0);
        semaphore_init(&mboxs[i].empty,10);
        semaphore_init(&mboxs[i].mutex,1);
    }
}

mailbox_t *mbox_open(char *name)
{
    int i=0;
    for(i=0;i<MAX_NUM_BOX;i++)
    {
        //printf("\n1");
        semaphore_down(&mboxs[i].mutex);
        if(mboxs[i].use==0)
        {
            semaphore_up(&mboxs[i].mutex);
            continue;
        }
        else
        {
            if(strcmp(mboxs[i].name,name)==0)
            {
                semaphore_up(&mboxs[i].mutex);
                return &mboxs[i];
            }
            else
                semaphore_up(&mboxs[i].mutex);
        }
    }
    for(i=0;i<MAX_NUM_BOX;i++)
    {
        //printf("\n2");
        semaphore_down(&mboxs[i].mutex);
        //printf("\n2.1");
        if(mboxs[i].use>0)
        {
            semaphore_up(&mboxs[i].mutex);
            continue;
        }
        else
        {
            //printf("\n3");
            mboxs[i].use ++;
            strcpy(mboxs[i].name,name);
            semaphore_up(&mboxs[i].mutex);
            return &mboxs[i];
        }
    }
}

void mbox_close(mailbox_t *mailbox)
{
    semaphore_down(&mailbox->mutex);
    mailbox->use --;
    if(mailbox->use <=0)
    {
        mailbox->contain =0;
        mailbox->name[0]='\0';
        semaphore_init(&mailbox->full,0);
        semaphore_init(&mailbox->empty,25);
    }
    semaphore_up(&mailbox->mutex);
}

void mbox_send(mailbox_t *mailbox, void *msg, int msg_length)
{
    semaphore_down(&mailbox->empty);
    semaphore_down(&mailbox->mutex);
    memcpy(mailbox->buffer[mailbox->contain],msg,msg_length);
    mailbox->contain ++;
    semaphore_up(&mailbox->mutex);
    semaphore_up(&mailbox->full);
}

void mbox_recv(mailbox_t *mailbox, void *msg, int msg_length)
{
    semaphore_down(&mailbox->full);
    semaphore_down(&mailbox->mutex);
    mailbox->contain --;
    memcpy(msg,mailbox->buffer[mailbox->contain],msg_length);
    semaphore_up(&mailbox->mutex);
    semaphore_up(&mailbox->empty);
}