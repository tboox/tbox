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
    // the list entry
    tb_single_list_entry_t     entry;

    // the data
    tb_size_t                   data;

}tb_demo_entry_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * comparer
 */
static tb_void_t tb_demo_entry_copy(tb_pointer_t litem, tb_pointer_t ritem)
{
    // check
    tb_assert(litem && ritem);

    // copy it
    ((tb_demo_entry_t*)litem)->data = ((tb_demo_entry_t*)ritem)->data;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_container_single_list_entry_main(tb_int_t argc, tb_char_t** argv)
{
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

    // init the list
    tb_single_list_entry_head_t list;
    tb_single_list_entry_init(&list, tb_demo_entry_t, entry, tb_demo_entry_copy);

    // insert entries
    tb_single_list_entry_insert_tail(&list, &entries[5].entry);
    tb_single_list_entry_insert_tail(&list, &entries[6].entry);
    tb_single_list_entry_insert_tail(&list, &entries[7].entry);
    tb_single_list_entry_insert_tail(&list, &entries[8].entry);
    tb_single_list_entry_insert_tail(&list, &entries[9].entry);
    tb_single_list_entry_insert_head(&list, &entries[4].entry);
    tb_single_list_entry_insert_head(&list, &entries[3].entry);
    tb_single_list_entry_insert_head(&list, &entries[2].entry);
    tb_single_list_entry_insert_head(&list, &entries[1].entry);
    tb_single_list_entry_insert_head(&list, &entries[0].entry);

    // the entry
    tb_demo_entry_t* entry = (tb_demo_entry_t*)tb_single_list_entry(&list, &entries[5].entry);
    tb_trace_i("entry: %lu", entry->data);
    tb_trace_i("");

    // walk it
    tb_trace_i("insert: %lu", tb_single_list_entry_size(&list));
    tb_for_all_if(tb_demo_entry_t*, item0, tb_single_list_entry_itor(&list), item0)
    {
        tb_trace_i("%lu", item0->data);
    }

    // trace
    tb_trace_i("");

    // replace entries
    tb_single_list_entry_replace_head(&list, &entries[10].entry);

    // walk it
    tb_trace_i("replace: %lu", tb_single_list_entry_size(&list));
    tb_for_all_if(tb_demo_entry_t*, item1, tb_single_list_entry_itor(&list), item1)
    {
        tb_trace_i("%lu", item1->data);
    }

    // trace
    tb_trace_i("");

    // remove entries
    tb_single_list_entry_remove_head(&list);

    // walk it
    tb_trace_i("remove: %lu", tb_single_list_entry_size(&list));
    tb_for_all_if(tb_demo_entry_t*, item2, tb_single_list_entry_itor(&list), item2)
    {
        tb_trace_i("%lu", item2->data);
    }

    // trace
    tb_trace_i("");

    // clear entries
    tb_single_list_entry_clear(&list);

    // walk it
    tb_trace_i("clear: %lu", tb_single_list_entry_size(&list));
    tb_for_all_if(tb_demo_entry_t*, item5, tb_single_list_entry_itor(&list), item5)
    {
        tb_trace_i("%lu", item5->data);
    }

    // exit the list
    tb_single_list_entry_exit(&list);

    // trace
    tb_trace_i("");
    return 0;
}
