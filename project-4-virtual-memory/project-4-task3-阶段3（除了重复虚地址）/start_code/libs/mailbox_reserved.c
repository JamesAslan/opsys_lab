#include "string.h"
#include "mailbox.h"

#define MAX_NUM_BOX 32

static mailbox_t mboxs[MAX_NUM_BOX];

void mbox_init()
{
    int i;
    for(i=0;i<=MAX_NUM_BOX;i++)
    {
        mboxs[i].use =0;
        mboxs[i].contain =0;
        mboxs[i].name[0]='\0';
        condition_init(&mboxs[i].condition_empty);
        condition_init(&mboxs[i].condition_full);
        mutex_lock_init(&mboxs[i].mutex);
    }
}

mailbox_t *mbox_open(char *name)
{
    int i=0;
    for(i=0;i<MAX_NUM_BOX;i++)
    {
        //printf("\n1");
        mutex_lock_acquire(&mboxs[i].mutex);
        if(mboxs[i].use==0)
        {
            mutex_lock_release(&mboxs[i].mutex);
            continue;
        }
        else
        {
            if(strcmp(mboxs[i].name,name)==0)
            {
                mutex_lock_release(&mboxs[i].mutex);
                return &mboxs[i];
            }
            else
                mutex_lock_release(&mboxs[i].mutex);
        }
    }
    for(i=0;i<MAX_NUM_BOX;i++)
    {
        //printf("\n2");
        mutex_lock_acquire(&mboxs[i].mutex);
        //printf("\n2.1");
        if(mboxs[i].use>0)
        {
            //printf("\n2.5");
            mutex_lock_release(&mboxs[i].mutex);
            continue;
        }
        else
        {
            //printf("\n3");
            mboxs[i].use ++;
            strcpy(mboxs[i].name,name);
            mutex_lock_release(&mboxs[i].mutex);
            //printf("\n4");
            return &mboxs[i];
        }
    }
    
}

void mbox_close(mailbox_t *mailbox)
{
    mutex_lock_acquire(&mailbox->mutex);
    mailbox->use --;
    if(mailbox->use <=0)
    {
        mailbox->contain =0;
        mailbox->name[0]='\0';
        condition_init(&mailbox->condition_empty);
        condition_init(&mailbox->condition_full);
    }
    mutex_lock_release(&mailbox->mutex);
}

void mbox_send(mailbox_t *mailbox, void *msg, int msg_length)
{
    mutex_lock_acquire(&mailbox->mutex);
    //printf("\n1");
    if(mailbox->contain+msg_length <= MAILBOX_MAX_CONTAIN)
    {
        //printf("\n2");
        mystrcpy(&(mailbox->buffer[mailbox->contain]),msg,msg_length);
        //printf("\n3");
        mailbox->contain += msg_length;
        condition_signal(&mailbox->condition_full);
        //printf("\n3");
    }
    else
    {
        //printf("\n3");
        while(mailbox->contain+msg_length > MAILBOX_MAX_CONTAIN)
        {
            condition_wait(&mailbox->mutex,&mailbox->condition_empty);
        } 
    }
    //printf("\n4");
    mutex_lock_release(&mailbox->mutex);
}

void mbox_recv(mailbox_t *mailbox, void *msg, int msg_length)
{
    mutex_lock_acquire(&mailbox->mutex);
    if(mailbox->contain >= msg_length)
    {
        mailbox->contain -= msg_length;
        mystrcpy(msg,&(mailbox->buffer[mailbox->contain]),msg_length);
        condition_signal(&mailbox->condition_empty);
    }
    else
    {
        while(mailbox->contain <msg_length)
        {
            condition_wait(&mailbox->mutex,&mailbox->condition_full);
        }
    }
    mutex_lock_release(&mailbox->mutex);
}