#include "civyprocess.h"
#define sentinel_doc "If you can read this, you're probably looking at the wrong object."


QueueEntry _CVProcess {
    PyObject *handler;
    PyGreenlet *loop;
    CVProcess parent;
    Q pipeline;
    };

typedef QueueEntry CVContext {
    PyGreenlet *cvthread;
    };


static PyObject* CVProcess_loop(PyObject *capsule)
{
    CVProcess self = (_CVProcess *)PyCapsule_GetPointer(capsule, NULL);
    Py_DECREF(capsule);
    PyObject *args = PyGreenlet_Switch( (PyGreenlet_GetCurrent())->parent, NULL, NULL );
    PyGreenlet *g;

    while (!Q_IS_EMPTY(self->pipeline)) {
        g = CVThreads_pop(self);

        if (g == NULL) {
            break;
        }
        args = PyGreenlet_Switch(g, args, NULL);
        Py_XDECREF(g);
    }
    return args;
}


CVProcess CVProcess_new(PyObject *event_handler)
/* To be called from `CVObject` */
{
    CVProcess context = (_CVProcess *)malloc(sizeof(_CVProcess));

    if (context == NULL) {
        return NULL;
    }
    context->loop = PyGreenlet_New(CVProcess_loop, NULL);
    PyObject *capsule = PyCapsule_New((void *)context, NULL, NULL);
    PyGreenlet_Switch(context->loop, capsule);
    context->handler = PyWeakref_NewRef(event_handler, NULL);
    context->parent = NULL;
    return context;
}


int CVProcess_dealloc(CVProcess self)
/* CVProcess method: Kill All */
{
    if (self == NULL) {
        return 0;
    }
    if (self->parent <> NULL) {
        if (Py_EnterRecursiveCall(" in CVProcess deallocation. Stack overvlow.") <> 0) || (CVProcess_dealloc(self->parent) == -1) {
            return -1;
        }
        Py_LeaveRecursiveCall();
    }
    while (!Q_IS_EMPTY(self->pipeline)) {
        Py_CLEAR(CVThreads_pop(self->pipeline));
    }
    Py_CLEAR(self->handler);
    Py_CLEAR(self->loop);
    free(self->pipeline);
    free(self);
    return 0;
}


int CVProcess_push_thread(CVProcess self, PyGreenlet *data)
/* CVProcess method: Push greenlets onto mini-queue */
{
    CVContext *new_entry = (CVContext *)malloc(sizeof(CVContext));
    
    if (new_entry == NULL) {
        return -1;
    }
    new_entry->cvthread = data;
    Q_prepend(self->pipeline, (QEntry *)new_entry);
    return 0;
}


PyGreenlet* CVProcess_pop_thread(CVProcess self)
/* CVProcess method: Pop greenlets from mini-queue */
{
    CVContext *entry = (CVContext *)Q_pop(self->pipeline);

    if (entry == NULL) {
        return NULL;
    }
    PyGreenlet *result = entry->cvthread;
    free(entry);
    return result;
}


struct cv_ForkSentinel {
    PyObject_HEAD
    };
static PyTypeObject cv_ForkSentinelType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "forksentinel",            /*tp_name*/
    sizeof(cv_ForkSentinel),   /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    0,                         /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    0,                         /*tp_flags*/
    sentinel_doc,              /* tp_doc */
    };


cv_ForkSentinel cv_WaitSentinel {
    PyObject *data;
    };
static PyTypeObject cv_WaitSentinelType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "waitsentinel",            /*tp_name*/
    sizeof(cv_WaitSentinel),   /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    0,                         /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    0,                         /*tp_flags*/
    sentinel_doc,              /* tp_doc */
    };
