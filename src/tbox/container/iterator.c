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
 * Copyright (C) 2009-present, TBOOX op->n Source Group.
 *
 * @author      ruki
 * @file        iterator.c
 * @ingroup     container
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "iterator.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_iterator_mode(tb_iterator_ref_t iterator)
{
    tb_assert(iterator);
    return (tb_size_t)iterator->mode;
}
tb_size_t tb_iterator_flag(tb_iterator_ref_t iterator)
{
    tb_assert(iterator);
    return (tb_size_t)iterator->flag;
}
tb_size_t tb_iterator_step(tb_iterator_ref_t iterator)
{
    tb_assert(iterator);
    return (tb_size_t)iterator->step;
}
tb_size_t tb_iterator_size(tb_iterator_ref_t iterator)
{
    tb_assert(iterator && iterator->op && iterator->op->size);
    return iterator->op->size(iterator);
}
tb_size_t tb_iterator_head(tb_iterator_ref_t iterator)
{
    tb_assert(iterator && iterator->op && iterator->op->head);
    return iterator->op->head(iterator);
}
tb_size_t tb_iterator_last(tb_iterator_ref_t iterator)
{
    tb_assert(iterator && iterator->op && iterator->op->last);
    return iterator->op->last(iterator);
}
tb_size_t tb_iterator_tail(tb_iterator_ref_t iterator)
{
    tb_assert(iterator && iterator->op && iterator->op->tail);
    return iterator->op->tail(iterator);
}
tb_size_t tb_iterator_prev(tb_iterator_ref_t iterator, tb_size_t itor)
{
    tb_assert(iterator && iterator->op && iterator->op->prev);
    return iterator->op->prev(iterator, itor);
}
tb_size_t tb_iterator_next(tb_iterator_ref_t iterator, tb_size_t itor)
{
    tb_assert(iterator && iterator->op && iterator->op->next);
    return iterator->op->next(iterator, itor);
}
tb_pointer_t tb_iterator_item(tb_iterator_ref_t iterator, tb_size_t itor)
{
    tb_assert(iterator && iterator->op && iterator->op->item);
    return iterator->op->item(iterator, itor);
}
tb_void_t tb_iterator_remove(tb_iterator_ref_t iterator, tb_size_t itor)
{
    tb_assert(iterator && iterator->op && iterator->op->remove);
    return iterator->op->remove(iterator, itor);
}
tb_void_t tb_iterator_nremove(tb_iterator_ref_t iterator, tb_size_t prev, tb_size_t next, tb_size_t size)
{
    tb_assert(iterator && iterator->op && iterator->op->nremove);
    return iterator->op->nremove(iterator, prev, next, size);
}
tb_void_t tb_iterator_copy(tb_iterator_ref_t iterator, tb_size_t itor, tb_cpointer_t item)
{
    tb_assert(iterator && iterator->op && iterator->op->copy);
    return iterator->op->copy(iterator, itor, item);
}
tb_long_t tb_iterator_comp(tb_iterator_ref_t iterator, tb_cpointer_t litem, tb_cpointer_t ritem)
{
    tb_assert(iterator && iterator->op && iterator->op->comp);
    return iterator->op->comp(iterator, litem, ritem);
}

