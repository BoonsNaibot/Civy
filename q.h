#ifndef q_included
#define q_included

typedef struct _queueentry *QEntry;
typedef struct _queue *Q;


static Q Queue_new(void);
static void Queue_push(Q self, QEntry new_entry);
static void Queue_prepend(Q self, QEntry new_entry);
static QEntry Queue_pop(Q self);
#define Q_IS_EMPTY(q) (q->head == NULL)
#define Queue_dealloc(q) free(q)
#endif
