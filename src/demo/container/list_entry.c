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
    // the data
    tb_size_t           data;

    // the list entry
    tb_list_entry_t     entry;

}tb_demo_entry_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_container_list_entry_main(tb_int_t argc, tb_char_t** argv)
{
    // init the entries
    tb_demo_entry_t entries[12] = 
    {
        {0, {0}}
    ,   {1, {0}}
    ,   {2, {0}}
    ,   {3, {0}}
    ,   {4, {0}}
    ,   {5, {0}}
    ,   {6, {0}}
    ,   {7, {0}}
    ,   {8, {0}}
    ,   {9, {0}}
    ,   {10, {0}}
    ,   {11, {0}}
    };
    

    // init the list
    tb_list_entry_t list;
    tb_list_entry_init(&list);

    // insert entries
    tb_list_entry_insert_tail(&list, &entries[5].entry);
    tb_list_entry_insert_tail(&list, &entries[6].entry);
    tb_list_entry_insert_tail(&list, &entries[7].entry);
    tb_list_entry_insert_tail(&list, &entries[8].entry);
    tb_list_entry_insert_tail(&list, &entries[9].entry);
    tb_list_entry_insert_head(&list, &entries[4].entry);
    tb_list_entry_insert_head(&list, &entries[3].entry);
    tb_list_entry_insert_head(&list, &entries[2].entry);
    tb_list_entry_insert_head(&list, &entries[1].entry);
    tb_list_entry_insert_head(&list, &entries[0].entry);

    // walk it
    tb_trace_i("insert:");
    tb_list_entry_for_all(tb_demo_entry_t, entry, item0, &list)
    {
        tb_trace_i("%lu", item0->data);
    }

    // trace
    tb_trace_i("");

    // replace entries
    tb_list_entry_replace_head(&list, &entries[10].entry);
    tb_list_entry_replace_last(&list, &entries[11].entry);

    // walk it
    tb_trace_i("replace:");
    tb_list_entry_for_all(tb_demo_entry_t, entry, item1, &list)
    {
        tb_trace_i("%lu", item1->data);
    }

    // trace
    tb_trace_i("");

    // remove entries
    tb_list_entry_remove_head(&list);
    tb_list_entry_remove_last(&list);

    // walk it
    tb_trace_i("remove:");
    tb_list_entry_for_all(tb_demo_entry_t, entry, item2, &list)
    {
        tb_trace_i("%lu", item2->data);
    }

    // trace
    tb_trace_i("");

    // clear entries
    tb_list_entry_clear(&list);

    // walk it
    tb_trace_i("clear:");
    tb_list_entry_for_all(tb_demo_entry_t, entry, item3, &list)
    {
        tb_trace_i("%lu", item3->data);
    }

    // exit the list
    tb_list_entry_exit(&list);

    // trace
    tb_trace_i("");
    return 0;
}
