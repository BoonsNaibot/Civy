static Uint32 cv_periodic_function(Uint32 interval, void *param)
{
    PyObject *v;
    CVCoroutine *coro;
    PyObject *this = (PyObject *)param;
    
    CV_BEGIN_DENY_THREADS
    
    /* Pass the interval's value to the coroutine */
    v = Py_BuildValue("(OI)", this->weak_actor, interval);

    if (v == NULL) {
        /* Schedule fail */
        return 0;
    }
    else if ((cv_spawn(this->coro, this->func, v, NULL) < 0) || (cv_push_event(this->weak_actor, CV_DISPATCHED_EVENT, 0) < 0) || (cv_object_queue_push(this->q, this->coro) < 0)) {
        Py_DECREF(v);
        /* Schedule Fail */
        return 0;
    }
    Py_INCREF(this->func);

    /* Create a new coroutine for the next period */
    coro = cv_create_coroutine(this->actor);

    if (coro == NULL) {
        /* Schedule Fail */
        return 0;
    }
    else if (cv_schedule_period(this->actor, this->ids, this->key) < 0) {
        /* Schedule Fail */
        return 0;
    }
    this->coro = coro;

    CV_END_DENY_THREADS

    return interval;
}


static int cv_schedule_period(CVCoroutine *coro, PyObject *ids, PyObject *key)
{
    _cvcontinuation period = {{0, NULL}, cv_periodic_exec, NULL, {NULL, NULL, NULL}};
    PyObject *arguments[3] = {ids, key, NULL};

    period.coargs = arguments;

    if (cv_costack_push(coro, &period) < 0) {
        return -1;
    }
    Py_INCREF(ids);
    Py_INCREF(key);
    Py_INCREF(func)
    return 0;
}


static void cv_schedule_once(PyObject **self, PyObject **callback, Uint32 *delay, PyObject *ids)
{
    PyObject *key;
    long int lint;
    
    lint = PyObject_Hash(*callback);
    
    if (lint < 0) {
        return -1;
    }
    key = PyLong_FromLong(lint);
    
    if (key == NULL) {
        return -1;
    }
    else {
        PyObject *whatever = PyDict_GetItem(ids, key);

        if (whatever != NULL) {
            if (whatever->type == INTERVAL) {
                PyErr_Format(PyExc_RuntimeError, "%s is already a scheduled periodic function.", PYOBJECT_NAME(*callback));
                Py_DECREF(key);
                return -1;
            }
            my_timer_id = SDL_AddTimer(*delay, schedule_once, my_callback_param);

            if (!my_timer_id) {
                PyErr_SetString(PyExc_RuntimeError, SDL_GetError());
                Py_DECREF(key);
                return -1;
            }
            new_id = Py_BuildValue("i", my_timer_id);

            if ((new_id == NULL) || (PyList_Append(whatever->sdl_ids, new_id) < 0)) {
                Py_DECREF(key);
                return -1;
            }
            Py_INCREF(whatever);
            Py_DECREF(new_id);
        }
    }
}


static PyObject* EventDispatcher_schedule(CVEventDispatcher self, PyObject *args, PyObject *kwargs)
{
    int repeat=0;
    Uint32 delay;
    PyObject *callback;

    {
        static char *kwds[] = {"callback", "delay", "repeat", NULL};

        if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OI|i", kwds, &callable, &delay, &repeat)) {
            return NULL;
        }
        else if (!PyCallable_Check(callable)) {
            PyErr_Format(PyExc_TypeError, "%s is not callable.", PYOBJECT_NAME(callback));
            return NULL;
        }
        else if (delay <= 0) {
            PyErr_SetString(PyExc_ValueError, "'delay' must be greater than 0 miliseconds.");
            return NULL;
        }
    }

    if (!repeat && (cv_schedule_once(&self, &callback, &delay, self->timer_ids) < 0)) {
        return NULL;
    }
    else if (cv_schedule_interval(&self, &callback, &delay, self->time_ids) < 0) {
        return NULL;
    }
    Py_RETURN_NONE;
}
