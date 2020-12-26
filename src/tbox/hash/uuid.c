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
 * @file        uuid.c
 * @ingroup     hash
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "uuid.h"
#include "md5.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

// http://xorshift.di.unimi.it/xorshift128plus.c
static tb_uint64_t tb_uuid4_xorshift128plus(tb_uint64_t* s)
{
    tb_uint64_t s1       = s[0];
    tb_uint64_t const s0 = s[1];
    s[0] = s0;
    s1 ^= s1 << 23;
    s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);
    return s[1] + s0;
}
static tb_bool_t tb_uuid4_generate(tb_byte_t uuid[16], tb_uint64_t seed[2])
{
    // init seed data
    union { tb_byte_t b[16]; tb_uint64_t word[2]; } s;
    s.word[0] = tb_uuid4_xorshift128plus(seed);
    s.word[1] = tb_uuid4_xorshift128plus(seed);

    /* generate uuid
     *
     * t: xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
     *    xxxxxxxxxxxx4xxxyxxxxxxxxxxxxxxx
     */
    tb_int_t i = 0;
    tb_int_t n = 0;
    tb_int_t t = 0;
    tb_int_t c1 = 0;
    tb_int_t c2 = 0;
    for (t = 0; t < 32; t++)
    {
        n = s.b[i >> 1];
        n = (i & 1) ? (n >> 4) : (n & 0xf);
        if (t == 16) // y
        {
            c2 = (n & 0x3) + 8;
            i++;
        }
        else if (t == 12) c2 = 4; // 4
        else // x
        {
            c2 = n;
            i++;
        }
        if (t & 1) uuid[t >> 1] = (tb_byte_t)(c1 * 16 + c2);
        c1 = c2;
    }
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_uuid_make(tb_byte_t uuid[16], tb_char_t const* name)
{
    return tb_uuid4_make(uuid, name);
}
tb_char_t const* tb_uuid_make_cstr(tb_char_t uuid_cstr[37], tb_char_t const* name)
{
    return tb_uuid4_make_cstr(uuid_cstr, name);
}
tb_bool_t tb_uuid4_make(tb_byte_t uuid[16], tb_char_t const* name)
{
    // check
    tb_assert_and_check_return_val(uuid, tb_false);

    // init seed
    tb_uint64_t seed[2];
    if (name)
    {
        tb_assert_static(sizeof(seed) == 16);
        tb_md5_make((tb_byte_t const*)name, tb_strlen(name), (tb_byte_t*)seed, 16);
    }
    else
    {
        // disable pseudo random
        tb_random_reset(tb_false);
        seed[0] = (tb_uint64_t)tb_random();
        seed[1] = (tb_uint64_t)tb_random();
    }

    // generate uuid v4
    return tb_uuid4_generate(uuid, seed);
}
tb_char_t const* tb_uuid4_make_cstr(tb_char_t uuid_cstr[37], tb_char_t const* name)
{
    // check
    tb_assert_and_check_return_val(uuid_cstr, tb_null);

    // make uuid bytes
    tb_byte_t uuid[16];
    if (!tb_uuid4_make(uuid, name)) return tb_null;

    // make uuid string
    tb_long_t size = tb_snprintf(   uuid_cstr
                                ,   37
                                ,   "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X"
                                ,   uuid[0], uuid[1], uuid[2], uuid[3]
                                ,   uuid[4], uuid[5]
                                ,   uuid[6], uuid[7]
                                ,   uuid[8], uuid[9]
                                ,   uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
    tb_assert_and_check_return_val(size == 36, tb_null);

    // end
    uuid_cstr[36] = '\0';

    // ok
    return uuid_cstr;
}
