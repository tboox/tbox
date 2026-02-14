/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_file_signature_main(tb_int_t argc, tb_char_t** argv)
{
    tb_file_signature_info_t info = {0};
    tb_char_t const* path = argv[1];

    // default to notepad.exe on windows if no path
    if (!path) 
    {
#ifdef TB_CONFIG_OS_WINDOWS
        path = "C:\\Windows\\System32\\notepad.exe";
#else
        path = argv[0];
#endif
    }

    // check signature
    if (tb_file_get_signature_info(path, &info))
    {
        tb_trace_i("File: %s", path);
        tb_trace_i("Is Signed: %s", info.is_signed ? "yes" : "no");
        tb_trace_i("Is Trusted: %s", info.is_trusted ? "yes" : "no");
        if (info.is_signed)
            tb_trace_i("Signer Name: %s", info.signer_name);
    }
    else
    {
        tb_trace_e("Failed to get signature info for %s", path);
    }

    return 0;
}
