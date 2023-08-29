/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2009-present, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        sched.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_SCHED_H
#define TB_PLATFORM_SCHED_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../utils/bits.h"
#if defined(TB_CONFIG_OS_WINDOWS)
#   include "windows/prefix.h"
#elif defined(TB_CONFIG_POSIX_HAVE_SCHED_YIELD)
#   include <sched.h>
#else
#   include "time.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// sched yield
#if defined(TB_CONFIG_OS_WINDOWS)
#   define tb_sched_yield()             YieldProcessor()
#elif defined(TB_CONFIG_POSIX_HAVE_SCHED_YIELD)
#   define tb_sched_yield()             sched_yield()
#else
#   define tb_sched_yield()             tb_usleep(1)
#endif

// cpu affinity
#define TB_CPUSET_SIZE                              TB_CPU_BITSIZE
#define TB_CPUSET_FFS(pset)                         (tb_sched_affinity_cpu_ffs(pset))
#define TB_CPUSET_EMPTY(pset)                       (tb_sched_affinity_cpu_empty(pset))
#define TB_CPUSET_COUNT(pset)                       (tb_sched_affinity_cpu_count(pset))
#define TB_CPUSET_ZERO(pset)                        (tb_sched_affinity_cpu_zero(pset))
#define TB_CPUSET_SET(cpu, pset)                    (tb_sched_affinity_cpu_set((tb_int_t)(cpu), (pset)))
#define TB_CPUSET_CLR(cpu, pset)                    (tb_sched_affinity_cpu_clr((tb_int_t)(cpu), (pset)))
#define TB_CPUSET_ISSET(cpu, pset)                  (tb_sched_affinity_cpu_isset((tb_int_t)(cpu), (pset)))
#define TB_CPUSET_AND(pdstset, psrcset1, psrcset2)  (tb_sched_affinity_cpu_and((pdstset), (psrcset1), (psrcset2)))
#define TB_CPUSET_OR(pdstset, psrcset1, psrcset2)   (tb_sched_affinity_cpu_or((pdstset), (psrcset1), (psrcset2)))
#define TB_CPUSET_XOR(pdstset, psrcset1, psrcset2)  (tb_sched_affinity_cpu_xor((pdstset), (psrcset1), (psrcset2)))
#define TB_CPUSET_EQUAL(set1ptr, pset2)             (tb_sched_affinity_cpu_equal((set1ptr), (pset2)))

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the cpuset type
typedef union __tb_cpuset_t
{
    tb_byte_t cpuset[TB_CPU_BITBYTE];
    tb_size_t _cpuset;

}tb_cpuset_t, *tb_cpuset_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! set cpu affinity for the given process id
 *
 * @param pid       the process id, set to the current process if be zero
 * @param cpuset    the cpu set
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_sched_setaffinity(tb_size_t pid, tb_cpuset_ref_t cpuset);

/*! get cpu affinity from the given process id
 *
 * @param pid       the process id, get the current process if be zero
 * @param cpuset    the cpu set
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_sched_getaffinity(tb_size_t pid, tb_cpuset_ref_t cpuset);

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

/*! get the first cpu index from the given cpu set
 *
 * @param pset  the cpu set
 *
 * @return      the cpu index
 */
static __tb_inline__ tb_int_t tb_sched_affinity_cpu_ffs(tb_cpuset_ref_t pset)
{
    tb_assert(pset);
#if TB_CPU_BIT64
    return (tb_int_t)tb_bits_fb1_u64_le(pset->_cpuset);
#else
    return (tb_int_t)tb_bits_fb1_u32_le(pset->_cpuset);
#endif
}

/*! get cpu count from the given cpu set
 *
 * @param pset  the cpu set
 *
 * @return      the cpu count
 */
static __tb_inline__ tb_int_t tb_sched_affinity_cpu_count(tb_cpuset_ref_t pset)
{
    tb_assert(pset);
#if TB_CPU_BIT64
    return (tb_int_t)tb_bits_cb1_u64(pset->_cpuset);
#else
    return (tb_int_t)tb_bits_cb1_u32(pset->_cpuset);
#endif
}

/*! the given cpu set is empty?
 *
 * @param pset  the cpu set
 *
 * @return      tb_true or tb_false
 */
static __tb_inline__ tb_bool_t tb_sched_affinity_cpu_empty(tb_cpuset_ref_t pset)
{
    tb_assert(pset);
    return pset->_cpuset == 0;
}

/*! clear the given cpu set
 *
 * @param pset  the cpu set
 */
static __tb_inline__ tb_void_t tb_sched_affinity_cpu_zero(tb_cpuset_ref_t pset)
{
    tb_assert(pset);
    pset->_cpuset = 0;
}

/*! set cpu to the given cpu set
 *
 * @param cpu   the cpu index
 * @param pset  the cpu set
 */
static __tb_inline__ tb_void_t tb_sched_affinity_cpu_set(tb_int_t cpu, tb_cpuset_ref_t pset)
{
    tb_assert(pset && cpu >= 0 && cpu < TB_CPUSET_SIZE);
    pset->_cpuset |= ((tb_size_t)1 << cpu);
}

/*! clear cpu index in the given cpu set
 *
 * @param cpu   the cpu index
 * @param pset  the cpu set
 */
static __tb_inline__ tb_void_t tb_sched_affinity_cpu_clr(tb_int_t cpu, tb_cpuset_ref_t pset)
{
    tb_assert(pset && cpu >= 0 && cpu < TB_CPUSET_SIZE);
    pset->_cpuset &= ~((tb_size_t)1 << cpu);
}

/*! Is the given cpu index is setted?
 *
 * @param cpu   the cpu index
 * @param pset  the cpu set
 *
 * @return      exists this cpu (not zero)
 */
static __tb_inline__ tb_int_t tb_sched_affinity_cpu_isset(tb_int_t cpu, tb_cpuset_ref_t pset)
{
    tb_assert(pset && cpu >= 0 && cpu < TB_CPUSET_SIZE);
    return ((pset->_cpuset & ((tb_size_t)1 << cpu)) != (tb_size_t)0);
}

/*! compute bits(and) for the given cpusets
 *
 * @param pdstset   the dest cpu set
 * @param psrcset1  the src cpu set1
 * @param psrcset2  the src cpu set2
 */
static __tb_inline__ tb_void_t tb_sched_affinity_cpu_and(tb_cpuset_ref_t pdstset, tb_cpuset_ref_t psrcset1, tb_cpuset_ref_t psrcset2)
{
    tb_assert(pdstset && psrcset1 && psrcset2);
    pdstset->_cpuset = psrcset1->_cpuset & psrcset2->_cpuset;
}

/*! compute bits(or) for the given cpusets
 *
 * @param pdstset   the dest cpu set
 * @param psrcset1  the src cpu set1
 * @param psrcset2  the src cpu set2
 */
static __tb_inline__ tb_void_t tb_sched_affinity_cpu_or(tb_cpuset_ref_t pdstset, tb_cpuset_ref_t psrcset1, tb_cpuset_ref_t psrcset2)
{
    tb_assert(pdstset && psrcset1 && psrcset2);
    pdstset->_cpuset = psrcset1->_cpuset | psrcset2->_cpuset;
}

/*! compute bits(xor) for the given cpusets
 *
 * @param pdstset   the dest cpu set
 * @param psrcset1  the src cpu set1
 * @param psrcset2  the src cpu set2
 */
static __tb_inline__ tb_void_t tb_sched_affinity_cpu_xor(tb_cpuset_ref_t pdstset, tb_cpuset_ref_t psrcset1, tb_cpuset_ref_t psrcset2)
{
    tb_assert(pdstset && psrcset1 && psrcset2);
    pdstset->_cpuset = psrcset1->_cpuset ^ psrcset2->_cpuset;
}

/*! Is equal with the given cpu sets ?
 *
 * @param pset1     the cpu set1
 * @param pset2     the cpu set2
 *
 * @return          is equal? (not zero)
 */
static __tb_inline__ tb_int_t tb_sched_affinity_cpu_equal(tb_cpuset_ref_t pset1, tb_cpuset_ref_t pset2)
{
    tb_assert(pset1 && pset2);
    return pset1->_cpuset == pset2->_cpuset;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
