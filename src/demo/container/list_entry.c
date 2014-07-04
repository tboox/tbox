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
    tb_demo_entry_t entries[10] = 
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

    tb_list_entry_for_all(tb_demo_entry_t, entry, item, &list)
    {
        tb_trace_i("%lu", item->data);
    }

    return 0;
}
