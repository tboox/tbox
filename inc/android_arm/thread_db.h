/*
 * Copyright 2006 The Android Open Source Project
 */

#ifndef _LIBTHREAD_DB__THREAD_DB_H
#define _LIBTHREAD_DB__THREAD_DB_H

#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <sys/types.h>


#define TD_THR_ANY_USER_FLAGS       0xffffffff
#define TD_THR_LOWEST_PRIORITY      -20
#define TD_SIGNO_MASK               NULL

/* td_err_e values */
enum {
    TD_OK,
    TD_ERR,
    TD_NOTHR,
    TD_NOSV,
    TD_NOLWP,
    TD_BADPH,
    TD_BADTH,
    TD_BADSH,
    TD_BADTA,
    TD_BADKEY,
    TD_NOMSG,
    TD_NOFPREGS,
    TD_NOLIBTHREAD,
    TD_NOEVENT,
    TD_NOCAPAB,
    TD_DBERR,
    TD_NOAPLIC,
    TD_NOTSD,
    TD_MALLOC,
    TD_PARTIALREG,
    TD_NOXREGS,
    TD_VERSION
};

/*
 * td_event_e values 
 * NOTE: There is a max of 32 events
 */
enum {
    TD_CREATE,
    TD_DEATH
};

/* td_thr_state_e values */
enum {
    TD_THR_ANY_STATE,
    TD_THR_UNKNOWN,
    TD_THR_SLEEP,
    TD_THR_ZOMBIE
};

typedef int32_t td_err_e;
typedef uint32_t td_event_e;
typedef uint32_t td_notify_e;
typedef uint32_t td_thr_state_e;
typedef pthread_t thread_t;

typedef struct
{
    pid_t pid;
} td_thragent_t;

typedef struct
{
    pid_t pid;
    pid_t tid;
} td_thrhandle_t;

typedef struct
{
    td_event_e event;
    td_thrhandle_t const * th_p;
    union {
        void * data;
    } msg;
} td_event_msg_t;

typedef struct
{
    uint32_t events;
} td_thr_events_t;

typedef struct
{
    union {
        void * bptaddr;
    } u;
} td_notify_t;

typedef struct
{
    td_thr_state_e ti_state;
    thread_t ti_tid; // pthread's id for the thread
    int32_t ti_lid; // the kernel's id for the thread
} td_thrinfo_t;


#define td_event_emptyset(set) \
    (set)->events = 0

#define td_event_fillset(set) \
    (set)->events = 0xffffffff

#define td_event_addset(set, n) \
    (set)->events |= (1 << n)


typedef int td_thr_iter_f(td_thrhandle_t const *, void *);


struct ps_prochandle;

#ifdef __cplusplus
extern "C"{
#endif

extern td_err_e td_ta_new(struct ps_prochandle const * proc_handle, td_thragent_t ** thread_agent);

extern td_err_e td_ta_set_event(td_thragent_t const * agent, td_thr_events_t * event);

extern td_err_e td_ta_event_addr(td_thragent_t const * agent, td_event_e event, td_notify_t * notify);

extern td_err_e td_ta_event_getmsg(td_thragent_t const * agent, td_event_msg_t * event);

extern td_err_e td_ta_thr_iter(td_thragent_t const * agent, td_thr_iter_f * func, void * cookie,
                               td_thr_state_e state, int32_t prio, sigset_t * sigmask, uint32_t user_flags);

extern char const ** td_symbol_list(void);

#ifdef __cplusplus
}
#endif

#endif
