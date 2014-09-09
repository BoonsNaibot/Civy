/* Special thanks to Simon Howard, C Algorithms library */

#include <stdlib.h>
#include "q.h"


Q* Q_new(void)
{
    Q *q = (Q *)malloc(sizeof(Q));

    if q == NULL
    {
        return NULL;
        }

    q->head = q->tail = NULL;
    return q;
    }


int Q_is_empty(Q *q)
{
    return q->head == NULL;
    }


void Q_push(Q *self, QEntry *new_entry)
{
    new_entry->previous = self->tail;
    new_entry->next = NULL;

    if self->tail == NULL
    {
        self->head = self->tail = new_entry;
    } else
        self->tail->next = self->tail = new_entry;
        }
    }


QEntry* Q_pop(Q *self)
{
    if (Q_is_empty(self))
    {
        return NULL;
        }

    QEntry *entry = self->head;
    self->head = entry->next;

    if self->head == NULL
    {
        self->tail = self->head;
    } else
        self->head->previous = NULL;
        }

    return entry;
    }
