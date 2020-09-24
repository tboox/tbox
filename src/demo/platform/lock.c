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
#define TB_TEST_LOCK_MUTEX
//#define TB_TEST_LOCK_SPINLOCK
//#define TB_TEST_LOCK_ATOMIC

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
static __tb_volatile__ tb_atomic32_t g_value = 0;

/* //////////////////////////////////////////////////////////////////////////////////////
 * loop
 */
static tb_int_t tb_test_mutx_loop(tb_cpointer_t priv)
{
    // check
    tb_uint32_t self = (tb_uint32_t)tb_thread_self(); tb_used(&self);

    // get lock
#if defined(TB_TEST_LOCK_MUTEX)
    tb_mutex_ref_t lock = (tb_mutex_ref_t)priv;
#elif defined(TB_TEST_LOCK_SPINLOCK)
    tb_spinlock_ref_t lock = (tb_spinlock_ref_t)priv;
#endif

    // get cpu core index
    static tb_size_t cpuidx = 0;
    tb_size_t cpu = cpuidx;
    cpuidx = (cpuidx + 1) % tb_cpu_count();

    // set thread affinity
    tb_cpuset_t cpuset;
    TB_CPUSET_ZERO(&cpuset);
    TB_CPUSET_SET(cpu, &cpuset);
    if (!tb_thread_setaffinity(tb_null, &cpuset))
    {
        tb_trace_e("thread[%x]: set cpu core(%zu) failed!", self, cpu);
    }

    // get cpu core again
    TB_CPUSET_ZERO(&cpuset);
    if (tb_thread_getaffinity(tb_null, &cpuset))
    {
        tb_size_t i;
        for (i = 0; i < TB_CPUSET_SIZE; i++)
        {
            if (TB_CPUSET_ISSET(i, &cpuset))
            {
                tb_trace_d("thread[%x]: init cpu core(%zu), cpu: %zu", self, i, cpu);
                break;
            }
        }
    }

    // loop
    __tb_volatile__ tb_size_t n = 100000;
    while (n--)
    {
#if defined(TB_TEST_LOCK_MUTEX)
        {
            // enter
            tb_mutex_enter(lock);

            // value++
            __tb_volatile__ tb_size_t n = 100;
            while (n--) g_value++;

            // leave
            tb_mutex_leave(lock);
        }
#elif defined(TB_TEST_LOCK_SPINLOCK)
        {
            // enter
            tb_spinlock_enter(lock);

            // value++
            __tb_volatile__ tb_size_t n = 100;
            while (n--) g_value++;

            // leave
            tb_spinlock_leave(lock);
        }
#elif defined(TB_TEST_LOCK_ATOMIC)
        tb_atomic32_fetch_and_add_explicit(&g_value, 1, TB_ATOMIC_RELAXED);
#else
        // value++
        __tb_volatile__ tb_size_t n = 100;
        while (n--) g_value++;
#endif
    }

    tb_trace_d("thread[%x]: exit cpu core(%zu)", self, cpu);
    return 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_lock_main(tb_int_t argc, tb_char_t** argv)
{
    // init lock
#if defined(TB_TEST_LOCK_MUTEX)
    tb_mutex_ref_t  lock = tb_mutex_init();
    tb_lock_profiler_register(tb_lock_profiler(), (tb_pointer_t)lock, "demo_mutex");
#elif defined(TB_TEST_LOCK_SPINLOCK)
    tb_spinlock_t   lock = TB_SPINLOCK_INIT;
    tb_lock_profiler_register(tb_lock_profiler(), (tb_pointer_t)&lock, "demo_spinlock");
#endif

    // init time
    tb_hong_t       time = tb_mclock();

    // init loop
    tb_size_t       i = 0;
    tb_size_t       n = argv[1]? tb_atoi(argv[1]) : TB_TEST_LOOP_MAXN;
    tb_thread_ref_t loop[TB_TEST_LOOP_MAXN] = {0};
    for (i = 0; i < n; i++)
    {
#if defined(TB_TEST_LOCK_MUTEX)
        loop[i] = tb_thread_init(tb_null, tb_test_mutx_loop, lock, 0);
#elif defined(TB_TEST_LOCK_SPINLOCK)
        loop[i] = tb_thread_init(tb_null, tb_test_mutx_loop, (tb_pointer_t)&lock, 0);
#else
        loop[i] = tb_thread_init(tb_null, tb_test_mutx_loop, tb_null, 0);
#endif
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
        tb_spinlock_exit(&lock);
#endif
    }

    // exit time
    time = tb_mclock() - time;

    // trace
    tb_trace_i("time: %lld ms", time);

    return 0;
}
