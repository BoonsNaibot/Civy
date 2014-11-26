#include "test.h"
#define CV_EVENT_LOOP_START switch(setjmp(to_main_loop)) { case 0: while(1) {
#define CV_EVENT_LOOP_END case 1: ; } break; case -1:
#define EXIT_CV break; }


static void cv_main_loop(void)
{
    CV_EVENT_LOOP_START

    switch(SDL_PollEvent(&main_event)) {
        case 0:
            Py_BEGIN_ALLOW_THREADS
            sleep(0.02);
            Py_END_ALLOW_THREADS
            break;
        default:
            switch(main_event.type) {
                case DISPATCHED_EVENT:
                    /* call some nonsense */
                    break;
                default:
                    /* call some other nonsense */
                    break;
            }
            break;
    }

    CV_EVENT_LOOP_END

    /* Call final function(s) */
    EXIT_CV
}


static int cv_app_init(PyObject *self, PyObject *args, PyObject *kwds)
{
    Uint32 mask;
    int *cv_a, *cv_gc, *cv_j, *cv_ff;
    static char *kwargs[] = {"CV_AUDIO", "CV_GAME_CONTROLLER", "CV_JOYSTICK", "CV_FORCE_FEEDBACK", NULL};

    if (args != NULL) {
        PyErr_SetString(PyExc_TypeError, "init flags must be specified as keyword arguments.");
        return -1;
    }
    SDL_assert(!SDL_WasInit(SDL_INIT_EVERYTHING));
    mask = SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|iiii", kwargs, &cv_a, &cv_gc, &cv_j, &cv_ff)) {
        return -1;
    }

    if cv_a {
        mask |= SDL_INIT_AUDIO;
    }
    if cv_gc {
        mask |= SDL_INIT_GAMECONTROLLER;
    }
    if cv_j {
        mask |= SDL_INIT_JOYSTICK;
    }
    if cv_ff {
        mask |= SDL_INIT_HAPTIC;
    }
    if (SDL_Init(mask) < 0) {
        PyErr_SetString(PyExc_RuntimeError, SDL_GetError());
        return -1;
    }
    tbd_global_app_pointer = self;
    return 0;
}
