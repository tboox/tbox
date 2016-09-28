/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the loop maxn
#define TB_TEST_LOOP_MAXN   (20)

// the lock type
//#define TB_TEST_LOCK_MUTEX
#define TB_TEST_LOCK_SPINLOCK
//#define TB_TEST_LOCK_ATOMIC

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
static __tb_volatile__ tb_atomic_t g_value = 0;

/* //////////////////////////////////////////////////////////////////////////////////////
 * loop
 */
static tb_int_t tb_test_mutx_loop(tb_cpointer_t priv)
{
    // check
    tb_uint32_t self = (tb_uint32_t)tb_thread_self();
    tb_handle_t lock = (tb_handle_t)priv; tb_used(lock);
    tb_trace_i("[loop: %x]: init", self);

    // loop
    __tb_volatile__ tb_size_t n = 100000;
    while (n--)
    {
#if defined(TB_TEST_LOCK_MUTEX)
        {
            // enter
            tb_mutex_enter((tb_mutex_ref_t)lock);

            // value++
            g_value++;

            // leave
            tb_mutex_leave((tb_mutex_ref_t)lock);
        }
#elif defined(TB_TEST_LOCK_SPINLOCK)
        {
            // enter
            tb_spinlock_enter((tb_spinlock_ref_t)&lock);

            // value++
            g_value++;

            // leave
            tb_spinlock_leave((tb_spinlock_ref_t)&lock);
        }
#elif defined(TB_TEST_LOCK_ATOMIC)
        tb_atomic_fetch_and_inc(&g_value);
#else
        // value++
        g_value++;
#endif

        // yield
        tb_sched_yield();
    }

    tb_trace_i("[loop: %x]: exit", self);
    return 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_lock_main(tb_int_t argc, tb_char_t** argv)
{
    // init lock
#if defined(TB_TEST_LOCK_MUTEX)
    tb_handle_t     lock = (tb_handle_t)tb_mutex_init();
    tb_assert_and_check_return_val(lock, 0);
#elif defined(TB_TEST_LOCK_SPINLOCK)
    tb_handle_t     lock = TB_SPINLOCK_INIT;
#elif defined(TB_TEST_LOCK_ATOMIC)
    tb_handle_t     lock = tb_null; 
#else
    tb_handle_t     lock = tb_null; 
#endif

    // init time
    tb_hong_t       time = tb_mclock();

    // init loop
    tb_size_t       i = 0;
    tb_size_t       n = argv[1]? tb_atoi(argv[1]) : TB_TEST_LOOP_MAXN;
    tb_thread_ref_t loop[TB_TEST_LOOP_MAXN] = {0};
    for (i = 0; i < n; i++)
    {
        loop[i] = tb_thread_init(tb_null, tb_test_mutx_loop, lock, 0);
        tb_assert_and_check_break(loop[i]);
    }

    // exit thread
    for (i = 0; i < TB_TEST_LOOP_MAXN; i++)
    {
        // kill thread
        if (loop[i]) 
        {
            tb_thread_wait(loop[i], -1, tb_null);
            tb_thread_exit(loop[i]);
            loop[i] = tb_null;
        }

        // exit lock
#if defined(TB_TEST_LOCK_MUTEX)
        if (lock) tb_mutex_exit(lock);
        lock = tb_null;
#elif defined(TB_TEST_LOCK_SPINLOCK)
        tb_spinlock_exit((tb_spinlock_ref_t)&lock);
#elif defined(TB_TEST_LOCK_ATOMIC)
        lock = tb_null;
#else
        lock = tb_null;
#endif
    }

    // exit time
    time = tb_mclock() - time;

    // trace
    tb_trace_i("time: %lld ms", time);

    return 0;
}
