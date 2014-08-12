/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the demo entry type
typedef struct __tb_demo_entry_t 
{
    // the heap entry
    tb_heap_entry_t     entry;

    // the data
    tb_size_t           data;

}tb_demo_entry_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * comparer
 */
#if 0
static tb_long_t tb_demo_entry_comp(tb_iterator_ref_t iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
    // check
    tb_assert_return_val(ltem && rtem, 0);

    // the data
    tb_size_t ldata = ((tb_demo_entry_t*)ltem)->data;
    tb_size_t rdata = ((tb_demo_entry_t*)rtem)->data;

    // comp
    return (ldata > rdata? 1 : (ldata < rdata? -1 : 0));
}
#endif
static tb_void_t tb_demo_entry_copy(tb_pointer_t ltem, tb_pointer_t rtem)
{
    // check
    tb_assert_return(ltem && rtem);

    // copy it
    ((tb_demo_entry_t*)ltem)->data = ((tb_demo_entry_t*)rtem)->data;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_container_heap_entry_main(tb_int_t argc, tb_char_t** argv)
{
#if 0
    // init the entries
    tb_demo_entry_t entries[12] = 
    {
        {{0}, 0}
    ,   {{0}, 1}
    ,   {{0}, 2}
    ,   {{0}, 3}
    ,   {{0}, 4}
    ,   {{0}, 5}
    ,   {{0}, 6}
    ,   {{0}, 7}
    ,   {{0}, 8}
    ,   {{0}, 9}
    ,   {{0}, 10}
    ,   {{0}, 11}
    };
#endif

    // init the heap
    tb_heap_entry_head_t heap;
    tb_heap_entry_init(&heap, tb_demo_entry_t, entry, tb_demo_entry_copy);

#if 0
    // insert entries
    tb_heap_entry_insert_tail(&heap, &entries[5].entry);
    tb_heap_entry_insert_tail(&heap, &entries[6].entry);
    tb_heap_entry_insert_tail(&heap, &entries[7].entry);
    tb_heap_entry_insert_tail(&heap, &entries[8].entry);
    tb_heap_entry_insert_tail(&heap, &entries[9].entry);
    tb_heap_entry_insert_head(&heap, &entries[4].entry);
    tb_heap_entry_insert_head(&heap, &entries[3].entry);
    tb_heap_entry_insert_head(&heap, &entries[2].entry);
    tb_heap_entry_insert_head(&heap, &entries[1].entry);
    tb_heap_entry_insert_head(&heap, &entries[0].entry);

    // the entry
    tb_demo_entry_t* entry = (tb_demo_entry_t*)tb_heap_entry(&heap, &entries[5].entry);
    tb_trace_i("entry: %lu", entry->data);
    tb_trace_i("");

    // walk it
    tb_trace_i("insert: %lu", tb_heap_entry_size(&heap));
    tb_for_all_if(tb_demo_entry_t*, item0, tb_heap_entry_itor(&heap), item0)
    {
        tb_trace_i("%lu", item0->data);
    }

    // trace
    tb_trace_i("");

    // replace entries
    tb_heap_entry_replace_head(&heap, &entries[10].entry);
    tb_heap_entry_replace_last(&heap, &entries[11].entry);

    // walk it
    tb_trace_i("replace: %lu", tb_heap_entry_size(&heap));
    tb_for_all_if(tb_demo_entry_t*, item1, tb_heap_entry_itor(&heap), item1)
    {
        tb_trace_i("%lu", item1->data);
    }

    // trace
    tb_trace_i("");

    // remove entries
    tb_heap_entry_remove_head(&heap);
    tb_heap_entry_remove_last(&heap);

    // walk it
    tb_trace_i("remove: %lu", tb_heap_entry_size(&heap));
    tb_for_all_if(tb_demo_entry_t*, item2, tb_heap_entry_itor(&heap), item2)
    {
        tb_trace_i("%lu", item2->data);
    }

    // trace
    tb_trace_i("");

    // moveto entries
    tb_heap_entry_ref_t head = tb_heap_entry_head(&heap);
    tb_heap_entry_moveto_head(&heap, tb_heap_entry_last(&heap));
    tb_heap_entry_moveto_tail(&heap, head);

    // walk it
    tb_trace_i("moveto: %lu", tb_heap_entry_size(&heap));
    tb_for_all_if(tb_demo_entry_t*, item3, tb_heap_entry_itor(&heap), item3)
    {
        tb_trace_i("%lu", item3->data);
    }

    // trace
    tb_trace_i("");

    // sort entries
    tb_sort_all(tb_heap_entry_itor(&heap), tb_demo_entry_comp);

    // walk it
    tb_trace_i("sort: %lu", tb_heap_entry_size(&heap));
    tb_for_all_if(tb_demo_entry_t*, item4, tb_heap_entry_itor(&heap), item4)
    {
        tb_trace_i("%lu", item4->data);
    }

    // trace
    tb_trace_i("");

    // clear entries
    tb_heap_entry_clear(&heap);

    // walk it
    tb_trace_i("clear: %lu", tb_heap_entry_size(&heap));
    tb_for_all_if(tb_demo_entry_t*, item5, tb_heap_entry_itor(&heap), item5)
    {
        tb_trace_i("%lu", item5->data);
    }
#endif

    // exit the heap
    tb_heap_entry_exit(&heap);

    // trace
    tb_trace_i("");
    return 0;
}
