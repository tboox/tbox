/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"
#if defined(TB_CONFIG_POSIX_HAVE_PTHREAD_CREATE)
#   include <pthread.h>
#elif defined(TB_CONFIG_OS_WINDOWS)
#   include <windows.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

#if defined(TB_CONFIG_POSIX_HAVE_PTHREAD_CREATE)
static pthread_key_t g_local_key = 0;
static tb_void_t tb_thread_local_exit(tb_pointer_t priv)  
{  
    tb_trace_i("thread[%lx]: free: %p", tb_thread_self(), priv);
}  
static tb_void_t tb_thread_local_init(tb_void_t)
{
	pthread_key_create(&g_local_key, tb_thread_local_exit);
}
static tb_pointer_t tb_thread_local_test(tb_cpointer_t priv)
{
    // self
    tb_size_t self = tb_thread_self();
    
    // trace
    tb_trace_i("thread[%lx]: init", self);

    // init start time
    tb_hong_t time = tb_mclock();

    // init local once
    static pthread_once_t s_once = PTHREAD_ONCE_INIT;
    pthread_once(&s_once, tb_thread_local_init);

    // done
    __tb_volatile__ tb_size_t count = 10000000;
    while (count--)
    {
        // init local variable
        tb_size_t local;
        if (!(local = (tb_size_t)pthread_getspecific(g_local_key)))
        {
            local = self;
            pthread_setspecific(g_local_key, (tb_pointer_t)local);
        }

        // check
        if (local != self)
        {
            // trace
            tb_trace_i("thread[%lx]: invalid value: %lx", self, local);
        }
    }

    // compile the interval time
    time = tb_mclock() - time;

    // trace
    tb_trace_i("thread[%lx]: exit: %lld ms", self, time);
    tb_thread_return(tb_null);
    return tb_null;
}
#elif defined(TB_COMPILER_IS_GCC) || defined(TB_COMPILER_IS_CLANG)
static tb_pointer_t tb_thread_local_test(tb_cpointer_t priv)
{
    // self
    tb_size_t self = tb_thread_self();
    
    // trace
    tb_trace_i("thread[%lx]: init", self);

    // init start time
    tb_hong_t time = tb_mclock();

    // done
    static __thread tb_size_t   local = 0;
    __tb_volatile__ tb_size_t   count = 10000000;
    while (count--)
    {
        // init local variable
        if (!local) local = self;

        // check
        if (local != self)
        {
            // trace
            tb_trace_i("thread[%lx]: invalid value: %lx", self, local);
        }
    }

    // compile the interval time
    time = tb_mclock() - time;

    // trace
    tb_trace_i("thread[%lx]: exit: %lld ms", self, time);
    tb_thread_return(tb_null);
    return tb_null;
}
#elif defined(TB_COMPILER_IS_MSVC)
static tb_pointer_t tb_thread_local_test(tb_cpointer_t priv)
{
    // self
    tb_size_t self = tb_thread_self();
    
    // trace
    tb_trace_i("thread[%lx]: init", self);

    // init start time
    tb_hong_t time = tb_mclock();

    // done
    static __declspec(thread) tb_size_t local = 0;
    __tb_volatile__ tb_size_t           count = 10000000;
    while (count--)
    {
        // init local variable
        if (!local) local = self;

        // check
        if (local != self)
        {
            // trace
            tb_trace_i("thread[%lx]: invalid value: %lx", self, local);
        }
    }

    // compile the interval time
    time = tb_mclock() - time;

    // trace
    tb_trace_i("thread[%lx]: exit: %lld ms", self, time);
    tb_thread_return(tb_null);
    return tb_null;
}
#elif defined(TB_CONFIG_OS_WINDOWS)
static tb_pointer_t tb_thread_local_test(tb_cpointer_t priv)
{
    // self
    tb_size_t self = tb_thread_self();
    
    // trace
    tb_trace_i("thread[%lx]: init", self);

    // init start time
    tb_hong_t time = tb_mclock();

    // init key
    static DWORD s_key = 0;
    if (!s_key) s_key = TlsAlloc();

    // done
    __tb_volatile__ tb_size_t count = 10000000;
    while (count--)
    {
        // init local variable
        tb_size_t local;
        if (!(local = (DWORD)TlsGetValue(s_key))) 
        {
            local = self;
            TlsSetValue(s_key, (LPVOID)local);
        }

        // check
        if (local != self)
        {
            // trace
            tb_trace_i("thread[%lx]: invalid value: %lx", self, local);
        }
    }

    // compile the interval time
    time = tb_mclock() - time;

    // trace
    tb_trace_i("thread[%lx]: exit: %lld ms", self, time);
    tb_thread_return(tb_null);
    return tb_null;
}
#elif defined(TB_CONFIG_API_HAVE_DEPRECATED)
static tb_void_t tb_thread_local_exit(tb_thread_store_data_t* data)
{
    if (data) tb_free(data);
}
static tb_pointer_t tb_thread_local_test(tb_cpointer_t priv)
{
    // self
    tb_size_t self = tb_thread_self();
    
    // trace
    tb_trace_i("thread[%lx]: init", self);

    // init start time
    tb_hong_t time = tb_mclock();

    // done
    tb_thread_store_data_t*     item = tb_null;
    __tb_volatile__ tb_size_t   count = 10000000;
    while (count--)
    {
        // init local variable
        if (!(item = tb_thread_store_getp()))
        {
            item = tb_malloc0_type(tb_thread_store_data_t);
            if (item)
            {
                item->type = self;
                item->free = tb_thread_local_exit;
                tb_thread_store_setp(item);
            }
        }
        // check
        else if (item->type != self)
        {
            // trace
            tb_trace_i("thread[%lx]: invalid value: %lx", self, item->type);
        }
    }

    // compile the interval time
    time = tb_mclock() - time;

    // trace
    tb_trace_i("thread[%lx]: exit: %lld ms", self, time);
    tb_thread_return(tb_null);
    return tb_null;
}
#else
static tb_pointer_t tb_thread_local_test(tb_cpointer_t priv)
{
    // self
    tb_size_t self = tb_thread_self();
    
    // trace
    tb_trace_i("thread[%lx]: init", self);

    // init start time
    tb_hong_t time = tb_mclock();

    // done
    __tb_volatile__ tb_size_t   count = 10000000;
    while (count--)
    {

    }

    // compile the interval time
    time = tb_mclock() - time;

    // trace
    tb_trace_i("thread[%lx]: exit: %lld ms", self, time);
    tb_thread_return(tb_null);
    return tb_null;
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_thread_local_main(tb_int_t argc, tb_char_t** argv)
{
    // done
    tb_thread_init(tb_null, tb_thread_local_test, tb_null, 0);
    tb_thread_init(tb_null, tb_thread_local_test, tb_null, 0);
    tb_thread_init(tb_null, tb_thread_local_test, tb_null, 0);
    tb_thread_init(tb_null, tb_thread_local_test, tb_null, 0);

    // wait
    getchar();
    return 0;
}
