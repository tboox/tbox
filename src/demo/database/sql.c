/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_demo_database_sql_test_done(tb_database_sql_ref_t database, tb_char_t const* sql)
{
    // check
    tb_assert_and_check_return(database && sql);

    // done
    do
    {
        // done sql
        if (!tb_database_sql_done(database, sql))
        {
            // trace
            tb_trace_e("done %s failed, error: %s", sql, tb_state_cstr(tb_database_sql_state(database)));
            break ;
        }

        // load result
//      tb_iterator_ref_t result = tb_database_sql_result_load(database, tb_true);
        tb_iterator_ref_t result = tb_database_sql_result_load(database, tb_false);
        tb_check_break(result);

        // trace
        tb_trace_i("==============================================================================");
        tb_trace_i("row: size: %lu", tb_iterator_size(result));

        // walk result
        tb_for_all_if (tb_iterator_ref_t, row, result, row)
        {
            // trace
            tb_tracef_i("[row: %lu, col: size: %lu]: ", row_itor, tb_iterator_size(row));

            // walk items
            tb_for_all_if (tb_database_sql_value_t*, value, row, value)
            {
                // trace
                tb_tracet_i("[%s:%s] ", tb_database_sql_value_name(value), tb_database_sql_value_text(value));
            }

            // trace
            tb_tracet_i(__tb_newline__);
        }

        // exit result
        tb_database_sql_result_exit(database, result);

    } while (0);
}
static tb_void_t tb_demo_database_sql_test_statement_done(tb_database_sql_ref_t database, tb_char_t const* sql)
{
    // check
    tb_assert_and_check_return(database && sql);

    // done
    tb_database_sql_statement_ref_t statement = tb_null;
    do
    {
        // init statement
        if (!(statement = tb_database_sql_statement_init(database, sql)))
        {
            // trace
            tb_trace_e("statement: init %s failed, error: %s", sql, tb_state_cstr(tb_database_sql_state(database)));
            break ;
        }

        // done statement
        if (!tb_database_sql_statement_done(database, statement))
        {
            // trace
            tb_trace_e("statement: done %s failed, error: %s", sql, tb_state_cstr(tb_database_sql_state(database)));
            break ;
        }

        // load result
//      tb_iterator_ref_t result = tb_database_sql_result_load(database, tb_true);
        tb_iterator_ref_t result = tb_database_sql_result_load(database, tb_false);
        tb_check_break(result);

        // trace
        tb_trace_i("==============================================================================");
        tb_trace_i("row: size: %lu", tb_iterator_size(result));

        // walk result
        tb_for_all_if (tb_iterator_ref_t, row, result, row)
        {
            // trace
            tb_tracef_i("[row: %lu, col: size: %lu]: ", row_itor, tb_iterator_size(row));

            // trace id
            tb_database_sql_value_t const* id = (tb_database_sql_value_t const*)tb_iterator_item(row, 0);
            tb_assert_and_check_break(id);
            tb_tracet_i("[%s:%d] ", tb_database_sql_value_name(id), tb_database_sql_value_int32(id));

#ifdef TB_CONFIG_TYPE_HAVE_FLOAT
            // trace fval
            tb_database_sql_value_t const* fval = (tb_database_sql_value_t const*)tb_iterator_item(row, 1);
            tb_assert_and_check_break(fval);
            tb_tracet_i("[%s:%f] ", tb_database_sql_value_name(fval), tb_database_sql_value_float(fval));
#endif

            // trace name
            tb_database_sql_value_t const* name = (tb_database_sql_value_t const*)tb_iterator_item(row, 2);
            tb_assert_and_check_break(name);
            tb_tracet_i("[%s:%s] ", tb_database_sql_value_name(name), tb_database_sql_value_text(name));

            // trace data
            tb_database_sql_value_t const* data = (tb_database_sql_value_t const*)tb_iterator_item(row, 3);
            tb_assert_and_check_break(data);
            tb_tracet_i("[%s:%s] ", tb_database_sql_value_name(data), tb_database_sql_value_blob(data));

            // trace tdata
            tb_database_sql_value_t const* tdata = (tb_database_sql_value_t const*)tb_iterator_item(row, 4);
            tb_assert_and_check_break(tdata);
            tb_tracet_i("[%s:%s] ", tb_database_sql_value_name(tdata), tb_database_sql_value_blob(tdata));

            // trace ldata
            tb_database_sql_value_t const* ldata = (tb_database_sql_value_t const*)tb_iterator_item(row, 5);
            tb_assert_and_check_break(ldata);
            tb_tracet_i("[%s:%s] ", tb_database_sql_value_name(ldata), tb_database_sql_value_blob(ldata));

            // trace ldata2
            tb_database_sql_value_t const* ldata2 = (tb_database_sql_value_t const*)tb_iterator_item(row, 6);
            tb_assert_and_check_break(ldata2);
            {
                // data?
                tb_stream_ref_t  stream = tb_null;
                if (tb_database_sql_value_blob(ldata2))
                {
                    // trace
                    tb_tracet_i("[%s:adler32(%lx)] ", tb_database_sql_value_name(ldata2), tb_adler32_make(tb_database_sql_value_blob(ldata2), tb_database_sql_value_size(ldata2), 0));
                }
                // stream?
                else if ((stream = tb_database_sql_value_blob_stream(ldata2)))
                {
                    // the stream size
                    tb_hong_t size = tb_stream_size(stream);
                    tb_assert_and_check_break(size >= 0);

                    // make data
                    tb_byte_t* data = tb_malloc0_bytes((tb_size_t)size);
                    tb_assert_and_check_break(data);

                    // read data
                    if (tb_stream_bread(stream, data, (tb_size_t)size))
                    {
                        // trace
                        tb_tracet_i("[%s:adler32(%lx)] ", tb_database_sql_value_name(ldata2), tb_adler32_make(data, (tb_size_t)size, 0));
                    }

                    // exit data
                    tb_free(data);
                }
                // null?
                else
                {
                    // trace
                    tb_tracet_i("[%s:null] ", tb_database_sql_value_name(ldata2));
                }
            }

            // trace number
            tb_database_sql_value_t const* number = (tb_database_sql_value_t const*)tb_iterator_item(row, 7);
            tb_assert_and_check_break(number);
            tb_tracet_i("[%s:%d] ", tb_database_sql_value_name(number), tb_database_sql_value_int32(number));

            // trace snumber
            tb_database_sql_value_t const* snumber = (tb_database_sql_value_t const*)tb_iterator_item(row, 8);
            tb_assert_and_check_break(snumber);
            tb_tracet_i("[%s:%d] ", tb_database_sql_value_name(snumber), tb_database_sql_value_int32(snumber));

            // trace
            tb_tracet_i(__tb_newline__);
        }

        // exit result
        tb_database_sql_result_exit(database, result);

    } while (0);

    // exit statement
    if (statement) tb_database_sql_statement_exit(database, statement);
}
static tb_void_t tb_demo_database_sql_test_statement_done_insert(tb_database_sql_ref_t database, tb_char_t const* sql, tb_char_t const* name, tb_char_t const* data, tb_char_t const* tdata, tb_char_t const* ldata1, tb_char_t const* ldata2, tb_size_t number, tb_uint16_t snumber)
{
    // check
    tb_assert_and_check_return(database && sql);

    // done
    tb_stream_ref_t                 stream = tb_null;
    tb_database_sql_statement_ref_t statement = tb_null;
    do
    {
        // init statement
        if (!(statement = tb_database_sql_statement_init(database, sql)))
        {
            // trace
            tb_trace_e("statement: init %s failed, error: %s", sql, tb_state_cstr(tb_database_sql_state(database)));
            break ;
        }

        // init stream
        if (ldata2)
        {
            // init it
            stream = tb_stream_init_from_url(ldata2);
            tb_assert_and_check_break(stream);

            // open it
            if (!tb_stream_open(stream)) break;
        }

        // bind statement
        tb_database_sql_value_t list[7];
        tb_database_sql_value_set_text(&list[0], name, 0);
        tb_database_sql_value_set_blob16(&list[1], (tb_byte_t const*)data, tb_strlen(data) + 1);
        tb_database_sql_value_set_blob8(&list[2], (tb_byte_t const*)tdata, tb_strlen(tdata) + 1);
        tb_database_sql_value_set_blob32(&list[3], (tb_byte_t const*)ldata1, tb_strlen(ldata1) + 1, tb_null);
        tb_database_sql_value_set_blob32(&list[4], tb_null, 0, stream);
        tb_database_sql_value_set_int32(&list[5], (tb_int32_t)number);
        tb_database_sql_value_set_int16(&list[6], snumber);
        if (!tb_database_sql_statement_bind(database, statement, list, tb_arrayn(list)))
        {
            // trace
            tb_trace_e("statement: bind %s failed, error: %s", sql, tb_state_cstr(tb_database_sql_state(database)));
            break ;
        }

        // done statement
        if (!tb_database_sql_statement_done(database, statement))
        {
            // trace
            tb_trace_e("statement: done %s failed, error: %s", sql, tb_state_cstr(tb_database_sql_state(database)));
            break ;
        }

    } while (0);

    // exit statement
    if (statement) tb_database_sql_statement_exit(database, statement);

    // exit stream
    if (stream) tb_stream_exit(stream);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_database_sql_main(tb_int_t argc, tb_char_t** argv)
{
    // init database
    tb_database_sql_ref_t database = tb_database_sql_init(argv[1]);
    if (database)
    {
        // open database
        if (tb_database_sql_open(database))
        {
            // done tests
            tb_demo_database_sql_test_done(database, "drop table if exists table1");
            tb_demo_database_sql_test_done(database, "create table table1(id int, name text, number int)");
            tb_demo_database_sql_test_done(database, "insert into table1 values(1, 'name1', 52642)");
            tb_demo_database_sql_test_done(database, "insert into table1 values(2, 'name2', 57127)");
            tb_demo_database_sql_test_done(database, "insert into table1 values(3, 'name3', 9000)");
            tb_demo_database_sql_test_done(database, "insert into table1 values(4, 'name4', 29000)");
            tb_demo_database_sql_test_done(database, "insert into table1 values(5, 'name5', 350000)");
            tb_demo_database_sql_test_done(database, "insert into table1 values(6, 'name6', 21000)");
            tb_demo_database_sql_test_done(database, "insert into table1 values(7, 'name7', 21600)");
            tb_demo_database_sql_test_done(database, "select * from table1");

            // remove first
            tb_demo_database_sql_test_statement_done(database, "drop table if exists table2");

            // begin
            if (tb_database_sql_begin(database))
            {
                // done tests
                tb_demo_database_sql_test_statement_done(database, "create table table2(id int, fval float, name text, data blob, tdata tinyblob, ldata1 longblob, ldata2 longblob, number int, snumber smallint)");
                tb_demo_database_sql_test_statement_done_insert(database, "insert into table2 values(1, 3.0, ?, ?, ?, ?, ?, ?, ?)", "name1", "data1", "ldata1", "tdata1", argv[2], 52642, 2642);
                tb_demo_database_sql_test_statement_done_insert(database, "insert into table2 values(2, 3.1, ?, ?, ?, ?, ?, ?, ?)", "name2", "data2", "ldata2", "tdata2", argv[2], 57127, 7127);
                tb_demo_database_sql_test_statement_done_insert(database, "insert into table2 values(3, 3.14, ?, ?, ?, ?, ?, ?, ?)", "name3", "data3", "ldata3", "tdata3", argv[2], 9000, 9000);
                tb_demo_database_sql_test_statement_done_insert(database, "insert into table2 values(4, 3.1415, ?, ?, ?, ?, ?, ?, ?)", "name4", "data4", "ldata4", "tdata4", argv[2], 29000, 9000);
                tb_demo_database_sql_test_statement_done_insert(database, "insert into table2 values(5, -3.1, ?, ?, ?, ?, ?, ?, ?)", "name5", "data5", "ldata5", "tdata5", argv[2], 350000, 5000);
                tb_demo_database_sql_test_statement_done_insert(database, "insert into table2 values(6, 3.454, ?, ?, ?, ?, ?, ?, ?)", "name6", "data6", "ldata6", "tdata6", argv[2], 21000, 1000);
                tb_demo_database_sql_test_statement_done_insert(database, "insert into table2 values(7, 100.098, ?, ?, ?, ?, ?, ?, ?)", "name7", "data7", "ldata7", "tdata7", argv[2], 21600, 1600);

                // commit
                tb_database_sql_commit(database);
//                tb_database_sql_rollback(database);

                // select
                tb_demo_database_sql_test_statement_done(database, "select * from table2");
            }
        }
        else
        {
            // trace
            tb_trace_i("open failed: %s", tb_state_cstr(tb_database_sql_state(database)));
        }

        // exit database
        tb_database_sql_exit(database);
    }
    return 0;
}
