/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        cookies.c
 * @ingroup     network
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "cookies"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "cookies.h"
#include "url.h"
#include "http.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../string/string.h"
#include "../platform/platform.h"
#include "../algorithm/algorithm.h"
#include "../container/container.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the impl entry type
typedef struct __tb_cookies_entry_t
{
    // the domain 
    tb_char_t const*        domain;

    // the path
    tb_char_t const*        path;

    // the name
    tb_char_t const*        name;

    // the value
    tb_char_t const*        value;

    // the expires
    tb_time_t               expires;

    // the max-age, default: 1 and storage: 0
    tb_uint32_t             maxage  : 30;

    // storage impl to file? remove it immediately if maxage == 0 and storage: 0
    tb_uint32_t             storage : 1;

    // is secure?
    tb_uint32_t             secure  : 1;

}tb_cookies_entry_t;

// the cookies impl type
typedef struct __tb_cookies_impl_t
{
    // the lock
    tb_spinlock_t           lock;

    // the string func
    tb_item_func_t          string_func;

    // the string pool
    tb_string_pool_ref_t    string_pool;
    
    // the cookie pool, key: "domain+path+name"
    tb_hash_ref_t           cookie_pool;

}tb_cookies_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_bool_t tb_cookies_get_domain_and_path_from_url(tb_char_t const* url, tb_char_t* pdomain, tb_size_t ndomain, tb_char_t* ppath, tb_size_t npath, tb_bool_t* psecure)
{
    // check
    tb_assert_and_check_return_val(url && pdomain && ndomain && ppath && npath && psecure, tb_false);

    // get url pointer
    tb_char_t const* p = url;
    tb_char_t const* e = url + tb_strlen(url);
    tb_assert_and_check_return_val(p < e, tb_false);

    // skip "http://"
    if (p + 7 < e && !tb_strnicmp(p, "http://", 7))
    {
        p += 7;
    }
    // skip "https://"
    else if (p + 8 < e && !tb_strnicmp(p, "https://", 8))
    {
        p += 8;
        *psecure = tb_true;
    }

    // skip www
    if (p + 3 < e && !tb_strnicmp(p, "www", 3)) p += 3;

    // skip .
    if (p + 1 < e && *p == '.') p++;

    // get domain
    tb_char_t* pb = (tb_char_t*)pdomain;
    tb_char_t* pe = (tb_char_t*)pdomain + ndomain - 1;
    while (p < e && pb < pe && *p && *p != '/' && *p != ':') *pb++ = *p++;
    *pb = '\0';
    
    // trace
    tb_trace_d("domain: %s", pdomain);

    // skip port
    if (*p && *p == ':') for (p++; p < e && *p && *p != '/'; p++) ;

    // get path
    pb = ppath;
    pe = ppath + npath - 1;
    while (p < e && pb < pe && *p && *p != '?') *pb++ = *p++;
    *pb = '\0';
    
    // no path?
    if (!*ppath) 
    {
        *ppath++ = '/';
        *ppath = '\0';
    }

    // trace
    tb_trace_d("path: %s", ppath);

    // ok?
    return *pdomain? tb_true : tb_false;
}
static tb_bool_t tb_cookies_is_child_domain(tb_char_t const* parent, tb_char_t const* child)
{
    // check
    tb_assert_and_check_return_val(parent && child, tb_false);

    // init
    tb_char_t const*    pb = parent;
    tb_char_t const*    cb = child;
    tb_size_t           pn = tb_strlen(pb);
    tb_size_t           cn = tb_strlen(cb);
    tb_assert_and_check_return_val(pn > 3 && cn > 3, tb_false);

    // is child?
    tb_size_t           n = 0;
    tb_char_t const*    pe = pb + pn - 1;
    tb_char_t const*    ce = cb + cn - 1;
    for (; pe >= pb && ce >= cb && *pe == *ce; pe--, ce--)
    {
        if (*ce == '.') n++;
    }

    // ok?
    return (pe < pb && n >= 1 && (ce < cb || *ce == '.'))? tb_true : tb_false;
}
static tb_bool_t tb_cookies_is_child_path(tb_char_t const* parent, tb_char_t const* child)
{
    // check
    tb_assert_and_check_return_val(parent && child, tb_false);

    // parent is root?
    if (parent[0] == '/' && !parent[1]) return tb_true;

    // is child?
    tb_char_t const*    p = parent;
    tb_char_t const*    c = child;
    tb_size_t           n = tb_strlen(parent);
    while (n-- && *p && *c && *p++ == *c++) ; 

    // ok?
    return (!*p && (!*c || *c == '/'))? tb_true : tb_false;
}
static tb_void_t tb_cookies_entry_exit(tb_cookies_impl_t* impl, tb_cookies_entry_t* entry)
{
    // check
    tb_assert_and_check_return(impl && entry);

    // exit domain
    if (entry->domain) tb_string_pool_del(impl->string_pool, entry->domain);
    entry->domain = tb_null;
    
    // exit path
    if (entry->path) tb_string_pool_del(impl->string_pool, entry->path);
    entry->path = tb_null;
    
    // exit name
    if (entry->name) tb_string_pool_del(impl->string_pool, entry->name);
    entry->name = tb_null;
    
    // exit value
    if (entry->value) tb_string_pool_del(impl->string_pool, entry->value);
    entry->value = tb_null;
}
static tb_void_t tb_cookies_entry_free(tb_item_func_t* func, tb_pointer_t item)
{
    // check
    tb_cookies_entry_t* entry = (tb_cookies_entry_t*)item;
    tb_assert_and_check_return(func && entry);

    // the impl
    tb_cookies_impl_t* impl = (tb_cookies_impl_t*)func->priv;
    tb_assert_and_check_return(impl && impl->string_pool);

    // exit it
    tb_cookies_entry_exit(impl, entry);
}
static tb_size_t tb_cookies_entry_hash(tb_item_func_t* func, tb_cpointer_t data, tb_size_t mask, tb_size_t index)
{
    // check
    tb_cookies_entry_t* entry = (tb_cookies_entry_t*)data;
    tb_assert_and_check_return_val(func && entry && entry->domain && entry->path && entry->name, 0);

    // the impl
    tb_cookies_impl_t* impl = (tb_cookies_impl_t*)func->priv;
    tb_assert_and_check_return_val(impl && impl->string_func.hash, 0);

    // compute the three hash values
    tb_size_t v0 = impl->string_func.hash(&impl->string_func, entry->domain, mask, index);
    tb_size_t v1 = impl->string_func.hash(&impl->string_func, entry->path, mask, index);
    tb_size_t v2 = impl->string_func.hash(&impl->string_func, entry->name, mask, index);

    // the hash value
    return (v0 ^ v1 ^ v2) & mask;
}
static tb_long_t tb_cookies_entry_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
    // check
    tb_cookies_entry_t* lentry = (tb_cookies_entry_t*)ldata;
    tb_cookies_entry_t* rentry = (tb_cookies_entry_t*)rdata;
    tb_assert_and_check_return_val(lentry && lentry->domain && lentry->path && lentry->name, 0);
    tb_assert_and_check_return_val(rentry && rentry->domain && rentry->path && rentry->name, 0);

    // compare domain
    tb_long_t ok = tb_strcmp(lentry->domain, rentry->domain);
    tb_check_return_val(!ok, ok);

    // compare domain
    ok = tb_strcmp(lentry->path, rentry->path);
    tb_check_return_val(!ok, ok);

    // compare name
    return tb_strcmp(lentry->name, rentry->name);
}
static tb_bool_t tb_cookies_entry_init(tb_cookies_impl_t* impl, tb_cookies_entry_t* entry, tb_char_t const* domain, tb_char_t const* path, tb_bool_t secure, tb_char_t const* value)
{
    // check
    tb_assert_and_check_return_val(impl && impl->string_pool && entry && value, tb_false);

    // init maxage: -1
    entry->maxage = 1;
    entry->storage = 0;

    // done 
    tb_char_t const* p = value;
    tb_char_t const* b = tb_null;
    tb_char_t const* v = tb_null;
    tb_char_t data[4096] = {0};
    while (1)
    {
        // key=value; or value;
        if (!*p || *p == ';')
        {
            // end?
            tb_check_break(b);

            // trace
            tb_trace_d("entry: %s: %s", b? b : "", v? v : "");

            // done value
            if (!tb_strnicmp(b, "expires", 7))
            {   
                // must have value
                tb_assert_and_check_return_val(v, tb_false);

                // make expires time
                entry->expires = tb_http_date_from_cstr(v, p - v);
            }
            else if (!tb_strnicmp(b, "max-age", 7))
            {           
                // must have value
                tb_assert_and_check_return_val(v, tb_false);

                // the maxage
                tb_long_t maxage = tb_stoi32(v);

                // storage to file?
                entry->storage = maxage > 0? 1 : 0;
                
                // save maxage
                entry->maxage = tb_abs(maxage);
            }
            else if (!tb_strnicmp(b, "domain", 6))
            {           
                // must have value
                tb_assert_and_check_return_val(v, tb_false);

                // save value
                tb_assert_and_check_return_val(p - v < sizeof(data) - 1, tb_false);
                if (v < p)
                {
                    tb_strlcpy(data, v, p - v); data[p - v] = '\0';
                    entry->domain = tb_string_pool_put(impl->string_pool, data[0] == '.'? data + 1 : data);
                }
            }
            else if (!tb_strnicmp(b, "path", 4))
            {               
                // must have value
                tb_assert_and_check_return_val(v, tb_false);

                // save value
                tb_assert_and_check_return_val(p - v < sizeof(data) - 1, tb_false);
                if (v < p)
                {
                    tb_strlcpy(data, v, p - v); data[p - v] = '\0';
                    entry->path = tb_string_pool_put(impl->string_pool, data);
                }
            }   
            else if (!tb_strnicmp(b, "version", 7))
            {
                // must have value
                tb_assert_and_check_return_val(v, tb_false);

            }   
            else if (!tb_strnicmp(b, "secure", 6))
            {
                entry->secure = 1;
            }
            // ignore it
            else if (!tb_strnicmp(b, "HttpOnly", 8)) ;
            // key=value
            else if (v)
            {
                // must have value
                tb_assert_and_check_return_val(v > b, tb_false);

                // save name
                tb_assert_and_check_return_val(v - b - 1 < sizeof(data) - 1, tb_false);
                tb_strlcpy(data, b, v - b - 1); data[v - b - 1] = '\0';
                entry->name = tb_string_pool_put(impl->string_pool, data);
                tb_assert_and_check_return_val(entry->name, tb_false);

                // save value
                tb_assert_and_check_return_val(p - v < sizeof(data) - 1, tb_false);
                if (v < p)
                {
                    tb_strlcpy(data, v, p - v); data[p - v] = '\0';
                    entry->value = tb_string_pool_put(impl->string_pool, data);
                    tb_assert_and_check_return_val(entry->value, tb_false);
                }
            }

            // next key-value pair
            if (*p) 
            {
                b = tb_null;
                v = tb_null;
                p++;
            }
            // end
            else break;
        }
        // skip space for name 
        else if (!b && !v && *p == ' ') p++;
        // point to name
        else if (!b && *p) b = p++;
        // point to value
        else if (!v && *p == '=') v = ++p;
        // next 
        else p++;
    }

    // check name
    tb_assert_and_check_return_val(entry->name, tb_false);

    // domain not exists? using the given domain
    if (!entry->domain && domain)
    {
        // the domain size
        tb_size_t n = tb_strlen(domain);

        // skip www
        if (n > 3 && !tb_strnicmp(domain, "www", 3)) 
        {
            domain += 3;
            n -= 3;
        }

        // skip .
        if (n && *domain == '.') domain++;

        // save domain
        entry->domain = tb_string_pool_put(impl->string_pool, domain);
    }
    if (!entry->domain)
    {
        // trace
        tb_trace_e("no domain for value: %s", value);
        return tb_false;
    }

    // path not exists? using the given path
    if (!entry->path) entry->path = tb_string_pool_put(impl->string_pool, path? path : "/");
    tb_assert_and_check_return_val(entry->path, tb_false);

    // no secure? using the given secure value
    if (!entry->secure && secure) entry->secure = 1;
    
    // ok
    return tb_true;
}
static tb_bool_t tb_cookies_entry_walk(tb_hash_ref_t hash, tb_hash_item_t* item, tb_bool_t* bdel, tb_cpointer_t priv)
{
    // check
    tb_value_t* tuple = (tb_value_t*)priv;
    tb_assert_and_check_return_val(hash && item && bdel && tuple, tb_false);

    // the entry
    tb_cookies_entry_t* entry = (tb_cookies_entry_t*)item->name;
    tb_assert_and_check_return_val(entry && entry->domain && entry->path && entry->name, tb_false);

    // the domain
    tb_char_t const* domain = tuple[0].cstr;

    // the path
    tb_char_t const* path = tuple[1].cstr;

    // the secure
    tb_size_t secure = tuple[2].ul;

    // the data and maxn
    tb_string_t* value = (tb_string_t*)tuple[3].ptr;
    tb_assert_and_check_return_val(value, tb_false);

    // expired?
    if (entry->expires && tb_cache_time() >= entry->expires)
    {
        // trace
        tb_trace_d("expired: %s%s%s: %s = %s", entry->secure? "https://" : "http://", entry->domain, entry->path, entry->name, entry->value? entry->value : "");

        // remove it
        *bdel = tb_true;
        return tb_true;
    }

    // this impl is at domain/path?
    if (    tb_cookies_is_child_domain(entry->domain, domain)
        &&  tb_cookies_is_child_path(entry->path, path)
        &&  entry->secure == secure)
    {
        // append "key=value; "
        tb_string_cstrfcat(value, "%s=%s; ", entry->name, entry->value? entry->value : "");
    }

    // ok
    tuple[4].b = tb_true;
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * instance implementation
 */
static tb_handle_t tb_cookies_instance_init(tb_cpointer_t* ppriv)
{
    return tb_cookies_init();
}
static tb_void_t tb_cookies_instance_exit(tb_handle_t cookies, tb_cpointer_t priv)
{
    // dump it
#ifdef __tb_debug__
    tb_cookies_dump((tb_cookies_ref_t)cookies);
#endif

    // exit it
    tb_cookies_exit((tb_cookies_ref_t)cookies);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interface implementation
 */
tb_cookies_ref_t tb_cookies()
{
    return (tb_cookies_ref_t)tb_singleton_instance(TB_SINGLETON_TYPE_COOKIES, tb_cookies_instance_init, tb_cookies_instance_exit, tb_null);
}
tb_cookies_ref_t tb_cookies_init()
{
    // done
    tb_bool_t           ok = tb_false;
    tb_cookies_impl_t*  impl = tb_null;
    do
    {
        // make impl
        impl = tb_malloc0_type(tb_cookies_impl_t);
        tb_assert_and_check_break(impl);

        // init lock
        if (!tb_spinlock_init(&impl->lock)) break;

        // init string pool
        impl->string_pool = tb_string_pool_init(tb_true);
        tb_assert_and_check_break(impl->string_pool);

        // init cookie pool
        tb_item_func_t func = tb_item_func_mem(sizeof(tb_cookies_entry_t), tb_cookies_entry_free, impl);
        func.hash = tb_cookies_entry_hash;
        func.comp = tb_cookies_entry_comp;
        impl->cookie_pool = tb_hash_init(TB_HASH_BULK_SIZE_MICRO, func, tb_item_func_true());
        tb_assert_and_check_break(impl->cookie_pool);

        // init string func
        impl->string_func = tb_item_func_str(tb_true, tb_string_pool());

        // register lock profiler
#ifdef TB_LOCK_PROFILER_ENABLE
        tb_lock_profiler_register(tb_lock_profiler(), (tb_pointer_t)&impl->lock, TB_TRACE_MODULE_NAME);
#endif

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit impl
        if (impl) tb_cookies_exit((tb_cookies_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_cookies_ref_t)impl;
}
tb_void_t tb_cookies_exit(tb_cookies_ref_t cookies)
{
    // check
    tb_cookies_impl_t* impl = (tb_cookies_impl_t*)cookies;
    tb_assert_and_check_return(impl);

    // enter
    tb_spinlock_enter(&impl->lock);

    // exit cookie pool
    if (impl->cookie_pool) tb_hash_exit(impl->cookie_pool);
    impl->cookie_pool = tb_null;
    
    // exit string pool
    if (impl->string_pool) tb_string_pool_exit(impl->string_pool);
    impl->string_pool = tb_null;

    // leave
    tb_spinlock_leave(&impl->lock);

    // exit lock
    tb_spinlock_exit(&impl->lock);

    // exit it
    tb_free(impl);
}
tb_void_t tb_cookies_clear(tb_cookies_ref_t cookies)
{
    // check
    tb_cookies_impl_t* impl = (tb_cookies_impl_t*)cookies;
    tb_assert_and_check_return(impl);

    // enter
    tb_spinlock_enter(&impl->lock);

    // clear cookie pool
    if (impl->cookie_pool) tb_hash_clear(impl->cookie_pool);
    
    // clear string pool
    if (impl->string_pool) tb_string_pool_clear(impl->string_pool);

    // leave
    tb_spinlock_leave(&impl->lock);
}
tb_bool_t tb_cookies_set(tb_cookies_ref_t cookies, tb_char_t const* domain, tb_char_t const* path, tb_bool_t secure, tb_char_t const* value)
{
    // check
    tb_cookies_impl_t* impl = (tb_cookies_impl_t*)cookies;
    tb_assert_and_check_return_val(impl, tb_false);

    // enter
    tb_spinlock_enter(&impl->lock);

    // done
    tb_bool_t           ok = tb_false;
    tb_cookies_entry_t  entry = {0};
    do
    {
        // check
        tb_assert_and_check_break(impl->string_pool && impl->cookie_pool);

        // init entry
        if (!tb_cookies_entry_init(impl, &entry, domain, path, secure, value)) break;

        // maxage is zero? remove it
        if (!entry.maxage && !entry.storage)
        {
            // remove it
            tb_hash_del(impl->cookie_pool, &entry);

            // exit it
            tb_cookies_entry_exit(impl, &entry);
        }
        // set entry
        else tb_hash_set(impl->cookie_pool, &entry, (tb_cpointer_t)tb_true);

        // storage to file?
        if (entry.storage)
        {
            // TODO
            tb_trace1_w("not supports storaging impl to file now!");
        }

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        tb_cookies_entry_exit(impl, &entry);
    }
    
    // leave
    tb_spinlock_leave(&impl->lock);

    // ok?
    return ok;
}
tb_bool_t tb_cookies_set_from_url(tb_cookies_ref_t cookies, tb_char_t const* url, tb_char_t const* value)
{
    // check
    tb_cookies_impl_t* impl = (tb_cookies_impl_t*)cookies;
    tb_assert_and_check_return_val(impl, tb_false);
    
    // get domain and path from the given url
    tb_bool_t secure = tb_false;
    tb_char_t path[TB_URL_PATH_MAXN] = {0};
    tb_char_t domain[TB_URL_HOST_MAXN] = {0};
    if (tb_cookies_get_domain_and_path_from_url(url, domain, sizeof(domain) - 1, path, sizeof(path) - 1, &secure))
    {
        // trace
        tb_trace_d("domain: %s, path: %s, secure: %s", domain, path, secure? "ok" : "no");

        // set it with domain and path
        return tb_cookies_set(cookies, domain, path, secure, value);
    }

    // try to set it without domain and path
    return tb_cookies_set(cookies, tb_null, tb_null, tb_false, value);
}
tb_char_t const* tb_cookies_get(tb_cookies_ref_t cookies, tb_char_t const* domain, tb_char_t const* path, tb_bool_t secure, tb_string_t* value)
{
    // check
    tb_cookies_impl_t* impl = (tb_cookies_impl_t*)cookies;
    tb_assert_and_check_return_val(impl && domain && value, tb_null);

    // clear value first
    tb_string_clear(value);

    // enter
    tb_spinlock_enter(&impl->lock);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // check
        tb_assert_and_check_break(impl->string_pool && impl->cookie_pool);
    
        // no path? using the root path
        if (!path || !path[0]) path = "/";

        // skip '.'
        if (*domain == '.') domain++;

        // spak the cached time
        tb_cache_time_spak();

        // get the matched values
        tb_value_t tuple[6];
        tuple[0].cstr   = domain;
        tuple[1].cstr   = path;
        tuple[2].ul     = secure? 1 : 0;
        tuple[3].ptr    = value;
        tuple[4].b      = tb_false;
        tb_hash_walk(impl->cookie_pool, tb_cookies_entry_walk, tuple);
        tb_check_break(tuple[4].b);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok) tb_string_clear(value);
    
    // leave
    tb_spinlock_leave(&impl->lock);

    // ok?
    return tb_string_cstr(value);
}
tb_char_t const* tb_cookies_get_from_url(tb_cookies_ref_t cookies, tb_char_t const* url, tb_string_t* value)
{
    // check
    tb_cookies_impl_t* impl = (tb_cookies_impl_t*)cookies;
    tb_assert_and_check_return_val(impl && value, tb_null);
    
    // get domain and path from the given url
    tb_bool_t secure = tb_false;
    tb_char_t path[TB_URL_PATH_MAXN] = {0};
    tb_char_t domain[TB_URL_HOST_MAXN] = {0};
    if (!tb_cookies_get_domain_and_path_from_url(url, domain, sizeof(domain) - 1, path, sizeof(path) - 1, &secure)) return tb_null;

    // trace
    tb_trace_d("domain: %s, path: %s, secure: %s", domain, path, secure? "ok" : "no");

    // get it from domain and path
    return tb_cookies_get(cookies, domain, path, secure, value);
}
#ifdef __tb_debug__
tb_void_t tb_cookies_dump(tb_cookies_ref_t cookies)
{
    // check
    tb_cookies_impl_t* impl = (tb_cookies_impl_t*)cookies;
    tb_assert_and_check_return(impl && impl->cookie_pool);

    // enter
    tb_spinlock_enter(&impl->lock);

    // dump
    tb_trace_i("======================================================================");
    tb_trace_i("cookie: size: %lu", tb_hash_size(impl->cookie_pool));
    tb_for_all_if (tb_hash_item_t*, item, impl->cookie_pool, item)
    {
        // the entry
        tb_cookies_entry_t* entry = (tb_cookies_entry_t*)item->name;
        tb_assert_and_check_continue(entry && entry->domain && entry->path && entry->name);

        // the date
        tb_tm_t date = {0};
        tb_gmtime(entry->expires, &date);

        // trace
        tb_trace_i("%s%s%s: %s = %s, expires: %04ld-%02ld-%02ld %02ld:%02ld:%02ld GMT, week: %d", entry->secure? "https://" : "http://", entry->domain, entry->path, entry->name, entry->value? entry->value : "", date.year, date.month, date.mday, date.hour, date.minute, date.second, date.week);
    }

    // leave
    tb_spinlock_leave(&impl->lock);
    tb_trace_i("======================================================================");
}
#endif

