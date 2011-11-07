/*!The Tiny Box Library
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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		cookies.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "cookies.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../libc/libc.h"

/* /////////////////////////////////////////////////////////
 * macros
 */
#if 1
#	define TB_COOKIES_DBG(fmt, arg...) do { tb_trace("[cookies]:" fmt , ## arg); } while (0)
#else
# 	define TB_COOKIES_DBG(fmt, arg...)
#endif

#define TB_COOKIES_SPOOL_GROW 		(64)
#define TB_COOKIES_CPOOL_GROW 		(16)

/* /////////////////////////////////////////////////////////
 * types
 */

// the cookie entry type
typedef struct __tb_cookie_entry_t
{
	// the string data
	tb_char_t const* 	pdomain;
	tb_char_t const* 	ppath;
	tb_char_t const* 	pname;
	tb_char_t const* 	pvalue;
	tb_char_t const* 	pexpires;
	tb_char_t const* 	pmaxage;
	tb_char_t const* 	pversion;

	// the string size
	tb_uint16_t 		ndomain;
	tb_uint16_t 		npath;
	tb_uint16_t 		nname;
	tb_uint16_t 		nvalue;
	tb_uint16_t 		nexpires;
	tb_uint16_t 		nmaxage;
	tb_uint16_t 		nversion;

	// the boolean value
	tb_uint16_t 		bsecure : 1;

}tb_cookie_entry_t;

// the cookie string
typedef struct __tb_cookie_string_t
{
	// the string refn
	tb_size_t 			refn;

	// the string data
	tb_char_t const* 	data;

}tb_cookie_string_t;

/* /////////////////////////////////////////////////////////
 * details
 */
static tb_bool_t tb_cookies_split_url(tb_char_t const* url, tb_char_t const* phost, tb_size_t* nhost, tb_char_t* ppath, tb_size_t* npath, tb_bool_t* psecure)
{
	tb_assert_and_check_return_val(url && phost && ppath && psecure, TB_FALSE);

	// get url size
	tb_size_t n = tb_strlen(url);
	if (!n) return TB_FALSE;

	// get url pointer
	tb_char_t const* p = url;
	tb_char_t const* e = url + n;

	// init
	*psecure = TB_FALSE;

	// is "http://" ?
	if (n > 7 
		&& p[0] == 'h'
		&& p[1] == 't'
		&& p[2] == 't'
		&& p[3] == 'p'
		&& p[4] == ':'
		&& p[5] == '/'
		&& p[6] == '/')
	{
		p += 7;
	}
	else if (n > 8 
		&& p[0] == 'h'
		&& p[1] == 't'
		&& p[2] == 't'
		&& p[3] == 'p'
		&& p[4] == 's'
		&& p[5] == ':'
		&& p[6] == '/'
		&& p[7] == '/')
	{
		p += 8;

		// is https?
		*psecure = TB_TRUE;
	}

	// get host
	tb_char_t* pb = phost;
	tb_char_t* pe = phost + *nhost - 1;
	while (p < e && pb < pe && *p && *p != '/' && *p != ':') *pb++ = *p++;
	*pb = '\0';
	*nhost = pb - phost;
	//TB_COOKIES_DBG("host: %s", phost);

	// skip port
	if (*p && *p == ':')
	{
		for (p++; p < e && *p && *p != '/'; p++) ;
	}

	// get path
	pb = ppath;
	pe = ppath + *npath - 1;
	while (p < e && pb < pe && *p) *pb++ = *p++;
	*pb = '\0';
	*npath = pb - ppath;
	
	// no path?
	if (!*ppath) 
	{
		*ppath++ = '/';
		*ppath = '\0';
		*npath = 1;
	}
	//TB_COOKIES_DBG("path: %s", ppath);

	return *phost? TB_TRUE : TB_FALSE;
}
// parse cookies entry
static tb_bool_t tb_cookies_parse(tb_cookie_entry_t* entry, tb_char_t const* domain, tb_char_t const* path, tb_bool_t secure, tb_char_t const* value)
{
	// init entry
	tb_memset(entry, 0, sizeof(tb_cookie_entry_t));

	// parse item
	tb_char_t const* p = value;
	tb_char_t const* b = TB_NULL;
	tb_char_t const* v = TB_NULL;
	while (1)
	{
		// parse field
		if (!*p || *p == ';')
		{
			// end?
			if (!b) break;

			//TB_COOKIES_DBG("name: %s", b? b : "");
			//TB_COOKIES_DBG("value[%d]: %s", p - v, v? v : "");

			// parse field
			if (!tb_strnicmp(b, "expires", 7))
			{	
				// invalid format?
				tb_assert_and_check_return_val(v, TB_FALSE);

				entry->pexpires = v;
				entry->nexpires = p - v;
			}
			else if (!tb_strnicmp(b, "max-age", 7))
			{			
				// invalid format?
				tb_assert_and_check_return_val(v, TB_FALSE);

				entry->pmaxage = v;
				entry->nmaxage = p - v;
			}
			else if (!tb_strnicmp(b, "domain", 6))
			{			
				// invalid format?
				tb_assert_and_check_return_val(v, TB_FALSE);

				entry->pdomain = v;
				entry->ndomain = p - v;
			}
			else if (!tb_strnicmp(b, "path", 4))
			{				
				// invalid format?
				tb_assert_and_check_return_val(v, TB_FALSE);

				entry->ppath = v;
				entry->npath = p - v;
			}	
			else if (!tb_strnicmp(b, "version", 7))
			{
				// invalid format?
				tb_assert_and_check_return_val(v, TB_FALSE);

				entry->pversion = v;
				entry->nversion = p - v;
			}	
			else if (!tb_strnicmp(b, "secure", 6))
			{
				entry->bsecure = 1;
			}
			// ignore 
			else if (!tb_strnicmp(b, "HttpOnly", 8)) ;
			else if (v)
			{
				// invalid format?
				tb_assert_and_check_return_val(v > b, TB_FALSE);

				entry->pname = b;
				entry->nname = v - b - 1;
				entry->pvalue = v;
				entry->nvalue = p - v;
			}

			// next field
			if (*p) 
			{
				b = TB_NULL;
				v = TB_NULL;
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
	tb_assert_and_check_return_val(entry->pname && entry->nname, TB_FALSE);

	// domain not exists?
	if (!entry->pdomain)
	{
		if (domain)
		{
			entry->pdomain = domain;
			entry->ndomain = tb_strlen(domain);
		}
		else return TB_FALSE;
	}

	// skip '.'
	if (*entry->pdomain == '.')
	{
		entry->pdomain++;
		entry->ndomain--;
	}

	// path not exists?
	if (!entry->ppath)
	{
		if (path)
		{
			entry->ppath = path;
			entry->npath = tb_strlen(path);
		}
		else 
		{
			entry->ppath = "/";
			entry->npath = 1;
		}
	}

	// secure not exists?
	if (!entry->bsecure && secure) entry->bsecure = 1;
	
	return TB_TRUE;
}

// set cookies string
static tb_size_t tb_cookie_set_string(tb_cookies_t* cookies, tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(s && n, 0);
 
	// reuse it if exists
	tb_slist_t* 	spool = cookies->spool;
	tb_size_t 		itor = tb_slist_itor_head(spool);
	tb_size_t 		tail = tb_slist_itor_tail(spool);
	for (; itor != tail; itor = tb_slist_itor_next(spool, itor))
	{
		tb_cookie_string_t* string = (tb_cookie_string_t*)tb_slist_itor_at(spool, itor);
		if (string && string->data && !tb_strncmp(string->data, s, n))
		{
			string->refn++;
			return itor;
		}
	}

	// add it if not exists
	tb_cookie_string_t string;
	string.refn = 1;
	string.data = tb_strndup(s, n);
	tb_assert(string.data);
	if (string.data) return tb_slist_insert_tail(spool, &string);
	else return 0;
}

// set cookies entry
static tb_bool_t tb_cookies_set_entry(tb_cookies_t* cookies, tb_cookie_entry_t const* entry)
{
	tb_mutex_lock(cookies->hmutex);

	tb_size_t 		i = 0;	
	tb_vector_t* 	cpool = cookies->cpool;
	tb_slist_t* 	spool = cookies->spool;
	tb_size_t 		size = tb_vector_size(cpool);
	for (i = 0; i < size; i++)
	{
		tb_cookie_t* cookie = (tb_cookie_t*)tb_vector_itor_at(cpool, i);
		if (cookie)
		{
			tb_cookie_string_t* sdomain = (tb_cookie_string_t*)tb_slist_itor_at(spool, cookie->domain);
			tb_cookie_string_t* spath = (tb_cookie_string_t*)tb_slist_itor_at(spool, cookie->path);
			tb_cookie_string_t* sname = (tb_cookie_string_t*)tb_slist_itor_at(spool, cookie->name);
			tb_cookie_string_t* svalue = (tb_cookie_string_t*)tb_slist_itor_at(spool, cookie->value);

			// is this?
			if ( 	sdomain && sdomain->data && !tb_strnicmp(sdomain->data, entry->pdomain, entry->ndomain)
				&& 	spath && spath->data && !tb_strncmp(spath->data, entry->ppath, entry->npath)
				&& 	sname && sname->data && !tb_strncmp(sname->data, entry->pname, entry->nname)
				)
			{
				// update secure
				cookie->secure = entry->bsecure;

				// update expires
				cookie->expires = tb_uint32_to_uint64(0);

				// update value
				if (svalue->refn > 1) svalue->refn--;
				else tb_slist_remove(spool, cookie->value);
				cookie->value = tb_cookie_set_string(cookies, entry->pvalue, entry->nvalue);
				tb_assert_abort(cookie->value);

				tb_mutex_unlock(cookies->hmutex);
				return TB_TRUE;
			}
		}
	}

	// no find
	tb_mutex_unlock(cookies->hmutex);
	return TB_FALSE;
}

// del cookies entry
static tb_void_t tb_cookies_del_entry(tb_cookies_t* cookies, tb_cookie_entry_t const* entry)
{
	tb_mutex_lock(cookies->hmutex);

	tb_size_t 		i = 0;	
	tb_vector_t* 	cpool = cookies->cpool;
	tb_slist_t* 	spool = cookies->spool;
	tb_size_t 		size = tb_vector_size(cpool);
	for (i = 0; i < size; i++)
	{
		tb_cookie_t* cookie = (tb_cookie_t*)tb_vector_itor_at(cpool, i);
		if (cookie)
		{
			tb_cookie_string_t* sdomain = (tb_cookie_string_t*)tb_slist_itor_at(spool, cookie->domain);
			tb_cookie_string_t* spath = (tb_cookie_string_t*)tb_slist_itor_at(spool, cookie->path);
			tb_cookie_string_t* sname = (tb_cookie_string_t*)tb_slist_itor_at(spool, cookie->name);

			// is this?
			if ( 	sdomain && sdomain->data && !tb_strnicmp(sdomain->data, entry->pdomain, entry->ndomain)
				&& 	spath && spath->data && !tb_strncmp(spath->data, entry->ppath, entry->npath)
				&& 	sname && sname->data && !tb_strncmp(sname->data, entry->pname, entry->nname)
				)
			{
				// remove domain
				if (sdomain->refn > 1) sdomain->refn--;
				else tb_slist_remove(spool, cookie->domain);

				// remove path
				if (spath->refn > 1) spath->refn--;
				else tb_slist_remove(spool, cookie->path);
	
				// remove name
				if (sname->refn > 1) sname->refn--;
				else tb_slist_remove(spool, cookie->name);

				// remove cookie
				tb_vector_remove(cpool, i);

				break;
			}
		}
	}

	tb_mutex_unlock(cookies->hmutex);
}
// add cookies entry
static tb_void_t tb_cookies_add_entry(tb_cookies_t* cookies, tb_cookie_entry_t const* entry)
{
	// lock
	tb_mutex_lock(cookies->hmutex);

	// set secure
	tb_cookie_t cookie;
	cookie.secure = entry->bsecure;

	// set expires
	cookie.expires = tb_uint32_to_uint64(0);

	// set strings
	cookie.domain = tb_cookie_set_string(cookies, entry->pdomain, entry->ndomain);
	cookie.path = tb_cookie_set_string(cookies, entry->ppath, entry->npath);
	cookie.name = tb_cookie_set_string(cookies, entry->pname, entry->nname);
	cookie.value = tb_cookie_set_string(cookies, entry->pvalue, entry->nvalue);
	tb_assert_abort(cookie.domain && cookie.path && cookie.name && cookie.value);

	// add cookie
	tb_vector_insert_tail(cookies->cpool, &cookie);

	// unlock
	tb_mutex_unlock(cookies->hmutex);
}

// is child domain?
static tb_bool_t tb_cookies_domain_ischild(tb_char_t const* parent, tb_char_t const* child)
{
	tb_assert_and_check_return_val(parent && child, TB_FALSE);

	tb_char_t const* 	pb = parent;
	tb_char_t const* 	cb = child;
	tb_size_t 			pn = tb_strlen(pb);
	tb_size_t 			cn = tb_strlen(cb);
	tb_assert_and_check_return_val(pn > 3 && cn > 3, TB_FALSE);

	tb_size_t 			n = 0;
	tb_char_t const* 	pe = pb + pn - 1;
	tb_char_t const* 	ce = cb + cn - 1;
	for (; pe >= pb && ce >= cb && *pe == *ce; pe--, ce--)
	{
		if (*ce == '.') n++;
	}

	// ok?
	if (pe < pb && n >= 1 && (ce < cb || *ce == '.')) return TB_TRUE;

	return TB_FALSE;
}
// is child path?
static tb_bool_t tb_cookies_path_ischild(tb_char_t const* parent, tb_char_t const* child)
{
	tb_assert_and_check_return_val(parent && child, TB_FALSE);

	// parent is root?
	if (parent[0] == '/' && !parent[1]) return TB_TRUE;

	// is child?
	tb_char_t const* 	p = parent;
	tb_char_t const* 	c = child;
	tb_size_t 			n = tb_strlen(parent);
	while (n-- && *p && *c && *p++ == *c++) ; 

	// ok?
	if (!*p && (!*c || *c == '/')) return TB_TRUE;

	return TB_FALSE;
}

// the dtor of string
static tb_void_t tb_cookies_spool_free(tb_void_t* data, tb_void_t* priv)
{
	if (data) 
	{
		tb_cookie_string_t* s = (tb_cookie_string_t*)data;
		//TB_COOKIES_DBG("[string]::dtor: %s", s->data? s->data : "");
		if (s->data) 
		{
			tb_free(s->data);
			s->data = TB_NULL;
		}
	}
}

/* /////////////////////////////////////////////////////////
 * implemention
 */
tb_cookies_t* tb_cookies_init()
{
	// calloc
	tb_cookies_t* cookies = (tb_cookies_t*)tb_calloc(1, sizeof(tb_cookies_t));
	tb_assert_and_check_return_val(cookies, TB_NULL);

	// init hmutex
	cookies->hmutex = tb_mutex_init("cookies");
	tb_assert_and_check_goto(cookies->hmutex, fail);

	// init spool
	tb_slist_item_func_t func = {tb_cookies_spool_free, TB_NULL};
	cookies->spool = tb_slist_init(sizeof(tb_cookie_string_t), TB_COOKIES_SPOOL_GROW, &func);
	tb_assert_and_check_goto(cookies->spool, fail);

	// init cpool
	cookies->cpool = tb_vector_init(sizeof(tb_cookie_t), TB_COOKIES_CPOOL_GROW, TB_NULL);
	tb_assert_and_check_goto(cookies->cpool, fail);

	return cookies;

fail:
	if (cookies) tb_cookies_exit(cookies);
	return TB_NULL;
}
tb_void_t tb_cookies_exit(tb_cookies_t* cookies)
{
	if (cookies)
	{
		tb_assert_and_check_return(cookies->hmutex);

		// clear cookies
		tb_cookies_clear(cookies);

		// free cpool & spool
		tb_mutex_lock(cookies->hmutex);

		// free cpool
		if (cookies->cpool) tb_vector_exit(cookies->cpool);
		cookies->cpool = TB_NULL;

		// free spool
		if (cookies->spool) tb_slist_exit(cookies->spool);
		cookies->spool = TB_NULL;

		tb_mutex_unlock(cookies->hmutex);

		// free mutex
		tb_mutex_exit(cookies->hmutex);
		
		// free it
		tb_free(cookies);
	}
}

tb_void_t tb_cookies_clear(tb_cookies_t* cookies)
{
	tb_assert_and_check_return(cookies && cookies->cpool && cookies->spool && cookies->hmutex);

	tb_mutex_lock(cookies->hmutex);
	tb_vector_clear(cookies->cpool);
	tb_slist_clear(cookies->spool);
	cookies->value[0] = '\0';
	tb_mutex_unlock(cookies->hmutex);
}

tb_void_t tb_cookies_set(tb_cookies_t* cookies, tb_char_t const* domain, tb_char_t const* path, tb_bool_t secure, tb_char_t const* value)
{
	tb_assert_and_check_return(cookies && value && cookies->cpool && cookies->spool && cookies->hmutex);
	//TB_COOKIES_DBG("[set]::%s%s%s = %s", secure == TB_TRUE? "https://" : "http://", domain? domain : "", path? path : "", value);

	// is null?
	tb_check_return(value[0]);

	// parse cookies entry
	tb_cookie_entry_t entry;
	if (!tb_cookies_parse(&entry, domain, path, secure, value)) return ;

#if 0
	TB_COOKIES_DBG("domain[%d]: %s", entry.ndomain, entry.pdomain? entry.pdomain : "");
	TB_COOKIES_DBG("path[%d]: %s", entry.npath, entry.ppath? entry.ppath : "");
	TB_COOKIES_DBG("name[%d]: %s", entry.nname, entry.pname? entry.pname : "");
	TB_COOKIES_DBG("value[%d]: %s", entry.nvalue, entry.pvalue? entry.pvalue : "");
	TB_COOKIES_DBG("expires[%d]: %s", entry.nexpires, entry.pexpires? entry.pexpires : "");
	TB_COOKIES_DBG("maxage[%d]: %s", entry.nmaxage, entry.pmaxage? entry.pmaxage : "");
	TB_COOKIES_DBG("version[%d]: %s", entry.nversion, entry.pversion? entry.pversion : "");
	TB_COOKIES_DBG("secure: %s", entry.bsecure? "true" : "false");
#endif

	// delete cookie entry if value is null
	if (!entry.pvalue || !entry.nvalue)
	{
		tb_cookies_del_entry(cookies, &entry);
		return ;
	}

	// set or add cookie entry
	if (!tb_cookies_set_entry(cookies, &entry))
		tb_cookies_add_entry(cookies, &entry);
}
tb_char_t const* tb_cookies_get(tb_cookies_t* cookies, tb_char_t const* domain, tb_char_t const* path, tb_bool_t secure)
{
	tb_assert_and_check_return_val(cookies && domain && *domain && cookies->cpool && cookies->spool && cookies->hmutex, TB_NULL);
	
	// no path?
	if (!path || !path[0]) path = "/";

	// skip '.'
	if (*domain == '.') domain++;

	// find the matched cookie values
	tb_size_t 		i = 0;
	tb_size_t 		n = 0;
	tb_vector_t* 	cpool = cookies->cpool;
	tb_slist_t* 	spool = cookies->spool;
	tb_size_t 		size = tb_vector_size(cpool);
	for (i = 0; i < size; i++)
	{
		tb_cookie_t const* cookie = (tb_cookie_t const*)tb_vector_itor_const_at(cpool, i);
		if (cookie)
		{
			tb_cookie_string_t const* sdomain = cookie->domain? (tb_cookie_string_t const*)tb_slist_itor_const_at(spool, cookie->domain) : TB_NULL;
			tb_cookie_string_t const* spath = cookie->path? (tb_cookie_string_t const*)tb_slist_itor_const_at(spool, cookie->path) : TB_NULL;
			
			// this cookies is at domain/path?
			if ( 	tb_cookies_domain_ischild(sdomain->data, domain)
				&& 	tb_cookies_path_ischild(spath->data, path))
			{
				tb_cookie_string_t const* sname = cookie->name? (tb_cookie_string_t const*)tb_slist_itor_const_at(spool, cookie->name) : TB_NULL;
				tb_cookie_string_t const* svalue = cookie->value? (tb_cookie_string_t const*)tb_slist_itor_const_at(spool, cookie->value) : TB_NULL;
				tb_assert_and_check_return_val(sname && sname->data, TB_NULL);
				tb_assert_and_check_return_val(svalue && svalue->data, TB_NULL);
			
				// enough?
				if (n + 8 < TB_COOKIES_VALUE_MAX)
				{
					tb_int_t ret = tb_snprintf(cookies->value + n, TB_COOKIES_VALUE_MAX - n, "%s=%s; ", sname->data, svalue->data);
					if (ret > 0) 
					{
						n += ret;
						cookies->value[n] = '\0';
					}
				}
				else break;
			}
		}
	}

	// strip tail
	if (n > 2) 
	{
		n -= 2;
		cookies->value[n] = '\0';
	}
	
	// no cookie?
	if (!n) return TB_NULL;

	// ok
	//TB_COOKIES_DBG("[get]::%s%s%s: %s", secure == TB_TRUE? "https://" : "http://", domain, path, cookies->value);
	return cookies->value;
}

tb_void_t tb_cookies_set_from_url(tb_cookies_t* cookies, tb_char_t const* url, tb_char_t const* value)
{
	tb_char_t phost[1024];
	tb_char_t ppath[4096];
	tb_size_t nhost = 1024;
	tb_size_t npath = 4096;
	tb_bool_t secure = TB_FALSE;
	if (tb_cookies_split_url(url, phost, &nhost, ppath, &npath, &secure))
	{
		// get domain
		tb_char_t const* domain = phost;
		if (nhost > 3
			&& (phost[0] == 'w' || phost[0] == 'W')
			&& (phost[1] == 'w' || phost[1] == 'W')
			&& (phost[2] == 'w' || phost[2] == 'W')
			)
		{
			domain += 3;
		}
		if (*domain == '.') domain++;

		// strip path
		tb_char_t* p = ppath;
		while (*p && *p != '?') p++;
		*p = '\0';
		
		//TB_COOKIES_DBG("domain: %s, path: %s", domain, ppath);

		// set it
		if (*domain) tb_cookies_set(cookies, domain, ppath, secure, value);
	}
	else tb_cookies_set(cookies, TB_NULL, TB_NULL, TB_FALSE, value);
}
tb_char_t const* tb_cookies_get_from_url(tb_cookies_t* cookies, tb_char_t const* url)
{
	tb_char_t phost[1024];
	tb_char_t ppath[4096];
	tb_size_t nhost = 1024;
	tb_size_t npath = 4096;
	tb_bool_t secure = TB_FALSE;
	if (tb_cookies_split_url(url, phost, &nhost, ppath, &npath, &secure))
	{
		// get domain
		tb_char_t const* domain = phost;
		if (nhost > 3
			&& (phost[0] == 'w' || phost[0] == 'W')
			&& (phost[1] == 'w' || phost[1] == 'W')
			&& (phost[2] == 'w' || phost[2] == 'W')
			)
		{
			domain += 3;
		}
		if (*domain == '.') domain++;

		// strip path
		tb_char_t* p = ppath;
		while (*p && *p != '?') p++;
		*p = '\0';
		
		//TB_COOKIES_DBG("domain: %s, path: %s", domain, ppath);

		// get it
		if (*domain) return tb_cookies_get(cookies, domain, ppath, secure);
	}
	return TB_NULL;
}


#ifdef TB_DEBUG
tb_void_t tb_cookies_dump(tb_cookies_t const* cookies)
{
	tb_assert_and_check_return(cookies && cookies->cpool && cookies->spool && cookies->hmutex);
	tb_mutex_lock(cookies->hmutex);

	TB_COOKIES_DBG("==================================================");
	TB_COOKIES_DBG("cookies:");
	
	tb_vector_t* cpool = cookies->cpool;
	tb_slist_t* spool = cookies->spool;

	// dump items
	tb_size_t i = 0;
	tb_size_t n = tb_vector_size(cpool);
	TB_COOKIES_DBG("[cpool]:size: %d, maxn: %d", n, tb_vector_maxn(cpool));
	for (i = 0; i < n; i++)
	{
		tb_cookie_t const* cookie = (tb_cookie_t const*)tb_vector_itor_const_at(cpool, i);
		if (cookie)
		{
			tb_cookie_string_t const* sdomain = cookie->domain? (tb_cookie_string_t const*)tb_slist_itor_const_at(spool, cookie->domain) : TB_NULL;
			tb_cookie_string_t const* spath = cookie->path? (tb_cookie_string_t const*)tb_slist_itor_const_at(spool, cookie->path) : TB_NULL;
			tb_cookie_string_t const* sname = cookie->name? (tb_cookie_string_t const*)tb_slist_itor_const_at(spool, cookie->name) : TB_NULL;
			tb_cookie_string_t const* svalue = cookie->value? (tb_cookie_string_t const*)tb_slist_itor_const_at(spool, cookie->value) : TB_NULL;

			TB_COOKIES_DBG("[cookie]: url = %s%s%s, %s = %s"
				, cookie->secure? "https://" : "http://"
				, sdomain && sdomain->data? sdomain->data : ""
				, spath && spath->data? spath->data : ""
				, sname && sname->data? sname->data : ""
				, svalue && svalue->data? svalue->data : ""
				);
		}
	}

	// dump strings
	tb_size_t itor = tb_slist_itor_head(spool);
	tb_size_t tail = tb_slist_itor_tail(spool);
	TB_COOKIES_DBG("[spool]:size: %d, maxn: %d", n, tb_slist_maxn(spool));
	for (; itor != tail; itor = tb_slist_itor_next(spool, itor))
	{
		tb_cookie_string_t const* s = (tb_cookie_string_t const*)tb_slist_itor_const_at(spool, itor);
		if (s)
		{
			TB_COOKIES_DBG("[string]:[%d]: %s ", s->refn, s->data? s->data : "");
		}
	}
	tb_mutex_unlock(cookies->hmutex);
}
#endif

