/* ////////////////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 		"flv"

/* ////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "libflv.h"

/* ////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the flv type
typedef struct __tb_flv_t
{
	// the stream
	tb_gstream_t* 					gst;

	// the spank type
	tb_size_t 						spank_type;
	tb_size_t 						spank_type_ok;

	// the string data
	tb_pstring_t 					string;

	// the sdata path
	tb_char_t 						spath[TB_FLV_SDATA_STRING_MAX];
	tb_char_t* 						stail;

	// the sdata bstream
	tb_bstream_t 					sdata_bst;

	// the hdata callback
	tb_flv_hdata_cb_func_t 			hdata_cb_func;
	tb_pointer_t 					hdata_cb_data;

	// the sdata callback
	tb_flv_sdata_cb_func_t 			sdata_cb_func;
	tb_pointer_t 					sdata_cb_data;

	tb_flv_sdata_data_cb_func_t 	sdata_data_cb_func;
	tb_pointer_t 					sdata_data_cb_data;

	// the audio config
	tb_byte_t* 						audio_config_data;
	tb_size_t 						audio_config_size;

	// the video config
	tb_byte_t* 						video_config_data;
	tb_size_t 						video_config_size;

	// the flv audio config callback
	tb_flv_audio_config_cb_func_t 	audio_config_cb_func;
	tb_pointer_t 					audio_config_cb_data;

	// the flv video config callback
	tb_flv_video_config_cb_func_t 	video_config_cb_func;
	tb_pointer_t 					video_config_cb_data;

	// the audio data
	tb_byte_t* 						audio_data;
	tb_size_t 						audio_size;
	tb_size_t 						audio_maxn;

	// the video data
	tb_byte_t* 						video_data;
	tb_size_t 						video_size;
	tb_size_t 						video_maxn;

	// the flv audio data callback
	tb_flv_audio_data_cb_func_t 	audio_data_cb_func;
	tb_pointer_t 					audio_data_cb_data;

	// the flv video data callback
	tb_flv_video_data_cb_func_t 	video_data_cb_func;
	tb_pointer_t 					video_data_cb_data;

}tb_flv_t;


// the spank func type
typedef tb_bool_t (*tb_flv_spank_func_t)(tb_flv_t* );

/* ////////////////////////////////////////////////////////////////////////////////
 * decls
 */

static tb_bool_t tb_flv_sdata_number_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value);
static tb_bool_t tb_flv_sdata_boolean_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value);
static tb_bool_t tb_flv_sdata_string_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value);
static tb_bool_t tb_flv_sdata_movieclip_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value);
static tb_bool_t tb_flv_sdata_null_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value);
static tb_bool_t tb_flv_sdata_undefined_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value);
static tb_bool_t tb_flv_sdata_reference_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value);
static tb_bool_t tb_flv_sdata_ecmaarray_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value);
static tb_bool_t tb_flv_sdata_strictarray_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value);
static tb_bool_t tb_flv_sdata_date_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value);
static tb_bool_t tb_flv_sdata_longstring_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value);
static tb_bool_t tb_flv_sdata_value_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value);
static tb_bool_t tb_flv_sdata_object_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value);
static tb_bool_t tb_flv_sdata_objects_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value);

/* /////////////////////////////////////////////////////////
 * cast
 */
static tb_char_t const* tb_flv_sdata_value_to_string(tb_flv_t* flv, tb_flv_sdata_value_t const* value)
{
	switch (value->type)
	{
	case TB_FLV_SDATA_TYPE_NUMBER:
		tb_pstring_cstrfcpy(&flv->string, "%f", value->u.number);
		break;
	case TB_FLV_SDATA_TYPE_BOOLEAN:
		tb_pstring_cstrfcpy(&flv->string, "%s", value->u.boolean? "true" : "false");
		break;
	case TB_FLV_SDATA_TYPE_STRING:
	case TB_FLV_SDATA_TYPE_LONGSTRING:
		return value->u.string.size? (value->u.string.size < TB_FLV_SDATA_STRING_MAX? value->u.string.data : "too long") : "";
	case TB_FLV_SDATA_TYPE_OBJECT:
		tb_pstring_cstrfcpy(&flv->string, "object");
		break;
	case TB_FLV_SDATA_TYPE_MOVIECLIP:
		tb_pstring_cstrfcpy(&flv->string, "movieclip");
		break;
	case TB_FLV_SDATA_TYPE_NULL:
		tb_pstring_cstrfcpy(&flv->string, "null");
		break;
	case TB_FLV_SDATA_TYPE_UNDEFINED:
		tb_pstring_cstrfcpy(&flv->string, "undefined");
		break;
	case TB_FLV_SDATA_TYPE_REFERENCE:
		tb_pstring_cstrfcpy(&flv->string, "reference");
		break;
	case TB_FLV_SDATA_TYPE_ECMAARRAY:
		tb_pstring_cstrfcpy(&flv->string, "ecmaarray");
		break;
	case TB_FLV_SDATA_TYPE_STRICTARRAY:
		tb_pstring_cstrfcpy(&flv->string, "strictarray");
		break;
	case TB_FLV_SDATA_TYPE_DATE:
		tb_pstring_cstrfcpy(&flv->string, "date");
		break;
	default:
		break;
	}

	return tb_pstring_cstr(&flv->string);
}
static tb_float_t tb_flv_sdata_value_to_number(tb_flv_t* flv, tb_flv_sdata_value_t const* value)
{
	switch (value->type)
	{
	case TB_FLV_SDATA_TYPE_NUMBER:
		return value->u.number;
	case TB_FLV_SDATA_TYPE_BOOLEAN:
		return (value->u.boolean? 1. : 0.);
	case TB_FLV_SDATA_TYPE_STRING:
	case TB_FLV_SDATA_TYPE_LONGSTRING:
	case TB_FLV_SDATA_TYPE_OBJECT:
	case TB_FLV_SDATA_TYPE_MOVIECLIP:
	case TB_FLV_SDATA_TYPE_NULL:
	case TB_FLV_SDATA_TYPE_UNDEFINED:
	case TB_FLV_SDATA_TYPE_REFERENCE:
	case TB_FLV_SDATA_TYPE_ECMAARRAY:
	case TB_FLV_SDATA_TYPE_STRICTARRAY:
	case TB_FLV_SDATA_TYPE_DATE:
	default:
		break;
	}

	return 0.;
}

/* ////////////////////////////////////////////////////////////////////////////////
 * spank
 */

static tb_bool_t tb_flv_sdata_number_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value)
{
	tb_assert_and_check_return_val(tb_bstream_left(&flv->sdata_bst) >= 8, TB_FALSE);

	value->type = TB_FLV_SDATA_TYPE_NUMBER;
	value->u.number = tb_bstream_get_double_bbe(&flv->sdata_bst);

	return TB_TRUE;
}
static tb_bool_t tb_flv_sdata_boolean_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value)
{
	tb_assert_and_check_return_val(tb_bstream_left(&flv->sdata_bst) >= 1, TB_FALSE);

	value->type = TB_FLV_SDATA_TYPE_BOOLEAN;
	value->u.boolean = tb_bstream_get_u8(&flv->sdata_bst)? TB_TRUE : TB_FALSE;
	return TB_TRUE;
}
static tb_bool_t tb_flv_sdata_string_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value)
{	
	tb_assert_and_check_return_val(tb_bstream_left(&flv->sdata_bst) >= 2, TB_FALSE);

	// read size
	tb_uint16_t size = tb_bstream_get_u16_be(&flv->sdata_bst);
	tb_assert_and_check_return_val(tb_bstream_left(&flv->sdata_bst) >= size, TB_FALSE);

	// too larger? skip it
	if (size >= TB_FLV_SDATA_STRING_MAX) tb_bstream_skip(&flv->sdata_bst, size);
	else
	{
		// read data
		if (size != tb_bstream_get_data(&flv->sdata_bst, value->u.string.data, size)) return TB_FALSE;
		if (size)
		{
			value->u.string.data[size] = '\0';
			value->u.string.size = size;
		}
	}
	value->type = TB_FLV_SDATA_TYPE_STRING;
	value->u.string.size = size;
	return TB_TRUE;
}
static tb_bool_t tb_flv_sdata_movieclip_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value)
{
	return TB_FALSE;
}
static tb_bool_t tb_flv_sdata_null_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value)
{
	value->type = TB_FLV_SDATA_TYPE_NULL;
	return TB_TRUE;
}
static tb_bool_t tb_flv_sdata_undefined_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value)
{
	value->type = TB_FLV_SDATA_TYPE_UNDEFINED;
	return TB_TRUE;
}
static tb_bool_t tb_flv_sdata_reference_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value)
{
	return TB_FALSE;
}
static tb_bool_t tb_flv_sdata_ecmaarray_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value)
{
	tb_assert_and_check_return_val(tb_bstream_left(&flv->sdata_bst) >= 4, TB_FALSE);

	// read size
	tb_uint32_t size = tb_bstream_get_u32_be(&flv->sdata_bst);
	tb_trace_impl("[ecmaarray]: size = %d", size);

	// save spath
	tb_char_t* 	stail = flv->stail;

	tb_uint32_t i = 0;
	if (size)
	{
		// read variable
		for (i = 0; i < size && tb_bstream_left(&flv->sdata_bst); i++)
		{
			// is end?
			if (tb_bstream_left(&flv->sdata_bst) >= 3 
				&& tb_bits_get_u24_be(flv->sdata_bst.p) == 0x09) 
			{
				tb_size_t end = tb_bstream_get_u24_be(&flv->sdata_bst);
				tb_trace_impl("ecmaarray end: %x", end);
				break;
			}

			// read name
			tb_flv_sdata_value_t name;
			if (!tb_flv_sdata_string_spank(flv, &name)) return TB_FALSE;

			// update spath
			tb_int_t ret = snprintf(stail, flv->spath + TB_FLV_SDATA_STRING_MAX - stail, ".%s", tb_flv_sdata_value_to_string(flv, &name));
			tb_assert_and_check_return_val(ret > 0, TB_FALSE);
			flv->stail += ret;
			*flv->stail = '\0';
			tb_trace_impl("[spath]: %s", flv->spath);

			// read data
			tb_flv_sdata_value_t data;
			if (tb_flv_sdata_value_spank(flv, &data))
			{
				tb_trace_impl("[ecmaarray: %d]: %s = %s"
					, i
					, tb_flv_sdata_value_to_string(flv, &name)
					, tb_flv_sdata_value_to_string(flv, &data));

				// callback
				if (flv->sdata_cb_func) flv->sdata_cb_func(flv->spath, &data, flv->sdata_cb_data);
			}
			// restore spath
			flv->stail = stail;
			*stail = '\0';
		}
	}

	// is end?
	if (i == size)
	{
		// is end?
		if (tb_bstream_left(&flv->sdata_bst) >= 3 
			&& tb_bits_get_u24_be(flv->sdata_bst.p) == 0x09) 
		{
			tb_size_t end = tb_bstream_get_u24_be(&flv->sdata_bst);
			tb_trace_impl("ecmaarray end: %x", end);
		}
	}

	// ok
	value->type = TB_FLV_SDATA_TYPE_ECMAARRAY;

	return TB_TRUE;
}
static tb_bool_t tb_flv_sdata_strictarray_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value)
{
	tb_assert_and_check_return_val(tb_bstream_left(&flv->sdata_bst) >= 4, TB_FALSE);
	
	// read size
	tb_uint32_t size = tb_bstream_get_u32_be(&flv->sdata_bst);
	tb_trace_impl("[strictarray]: size = %d", size);

	// callback
	tb_flv_sdata_value_t data;
	data.type = TB_FLV_SDATA_TYPE_NUMBER;
	data.u.number = size;
	if (flv->sdata_cb_func) flv->sdata_cb_func(flv->spath, &data, flv->sdata_cb_data);

	// read variable
	tb_uint32_t i = 0;
	for (i = 0; i < size && tb_bstream_left(&flv->sdata_bst); i++)
	{
		if (!tb_flv_sdata_value_spank(flv, &data)) return TB_FALSE;
		tb_trace_impl("[strictarray: %d]: %s", i, tb_flv_sdata_value_to_string(flv, &data));

		// callback
		if (flv->sdata_cb_func) flv->sdata_cb_func(flv->spath, &data, flv->sdata_cb_data);
	}

	// ok
	value->type = TB_FLV_SDATA_TYPE_STRICTARRAY;

	return TB_TRUE;
}
static tb_bool_t tb_flv_sdata_date_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value)
{
	tb_assert_and_check_return_val(tb_bstream_left(&flv->sdata_bst) >= 10, TB_FALSE);
	tb_bstream_skip(&flv->sdata_bst, 10);
	value->type = TB_FLV_SDATA_TYPE_DATE;
	return TB_TRUE;
}
static tb_bool_t tb_flv_sdata_longstring_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value)
{	
	tb_assert_and_check_return_val(tb_bstream_left(&flv->sdata_bst) >= 4, TB_FALSE);

	// read size
	tb_uint32_t size = tb_bstream_get_u32_be(&flv->sdata_bst);
	tb_assert_and_check_return_val(tb_bstream_left(&flv->sdata_bst) >= size, TB_FALSE);

	// too larger? skip it
	if (size >= TB_FLV_SDATA_STRING_MAX) tb_bstream_skip(&flv->sdata_bst, size);
	else
	{
		// read data
		if (size != tb_bstream_get_data(&flv->sdata_bst, value->u.string.data, size)) return TB_FALSE;
		if (size)
		{
			value->u.string.data[size] = '\0';
			value->u.string.size = size;
		}
	}
	value->type = TB_FLV_SDATA_TYPE_LONGSTRING;
	value->u.string.size = size;
	return TB_TRUE;
}
static tb_bool_t tb_flv_sdata_value_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value)
{	
	tb_assert_and_check_return_val(tb_bstream_left(&flv->sdata_bst) >= 1, TB_FALSE);

	value->type = tb_bstream_get_u8(&flv->sdata_bst);
	//tb_trace_impl("value type: %x", value->type);
	switch (value->type)
	{
	case TB_FLV_SDATA_TYPE_NUMBER:
		return tb_flv_sdata_number_spank(flv, value);
	case TB_FLV_SDATA_TYPE_LONGSTRING:
		return tb_flv_sdata_longstring_spank(flv, value);
	case TB_FLV_SDATA_TYPE_STRING:
		return tb_flv_sdata_string_spank(flv, value);
	case TB_FLV_SDATA_TYPE_ECMAARRAY:
		return tb_flv_sdata_ecmaarray_spank(flv, value);
	case TB_FLV_SDATA_TYPE_OBJECT:
		return tb_flv_sdata_objects_spank(flv, value);
	case TB_FLV_SDATA_TYPE_STRICTARRAY:
		return tb_flv_sdata_strictarray_spank(flv, value);
	case TB_FLV_SDATA_TYPE_BOOLEAN:
		return tb_flv_sdata_boolean_spank(flv, value);
	case TB_FLV_SDATA_TYPE_DATE:
		return tb_flv_sdata_date_spank(flv, value);
	case TB_FLV_SDATA_TYPE_NULL:
		return tb_flv_sdata_null_spank(flv, value);
	case TB_FLV_SDATA_TYPE_UNDEFINED:
		return tb_flv_sdata_undefined_spank(flv, value);
	case TB_FLV_SDATA_TYPE_MOVIECLIP:
		//return tb_flv_sdata_movieclip_spank(flv, value);
	case TB_FLV_SDATA_TYPE_REFERENCE:
		//return tb_flv_sdata_reference_spank(flv, value);
	default:
		tb_trace_impl("unknown value type: %x", value->type);
		break;
	}
	return TB_FALSE;
}
static tb_bool_t tb_flv_sdata_object_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value)
{
	// save spath
	tb_char_t* 	stail = flv->stail;

	// get name
	tb_flv_sdata_value_t name;
	if (!tb_flv_sdata_string_spank(flv, &name)) return TB_FALSE;
	tb_trace_impl("[object_name]: %s", tb_flv_sdata_value_to_string(flv, &name));

	// update spath
	tb_int_t ret = snprintf(stail, flv->spath + TB_FLV_SDATA_STRING_MAX - stail, ".%s", tb_flv_sdata_value_to_string(flv, &name));
	tb_assert_and_check_return_val(ret > 0, TB_FALSE);
	flv->stail += ret;
	*flv->stail = '\0';
	tb_trace_impl("[spath]: %s", flv->spath);

	// get data
	tb_flv_sdata_value_t data;
	if (tb_flv_sdata_value_spank(flv, &data) && flv->sdata_cb_func)
	{
		// callback
		flv->sdata_cb_func(flv->spath, &data, flv->sdata_cb_data);
	}

	// restore spath
	flv->stail = stail;
	*stail = '\0';

	// ok
	value->type = TB_FLV_SDATA_TYPE_OBJECT;

	return TB_TRUE;
}

static tb_bool_t tb_flv_sdata_objects_spank(tb_flv_t* flv, tb_flv_sdata_value_t* value)
{
	value->type = TB_FLV_SDATA_TYPE_OBJECT;
	while (tb_bstream_left(&flv->sdata_bst))
	{
		// is end?
		if (tb_bstream_left(&flv->sdata_bst) >= 3 
			&& tb_bits_get_u24_be(flv->sdata_bst.p) == 0x09) 
		{
			tb_size_t end = tb_bstream_get_u24_be(&flv->sdata_bst);
			tb_trace_impl("objects end: %x", end);
			break;
		}

		// get object
		tb_flv_sdata_value_t object;
		if (!tb_flv_sdata_object_spank(flv, &object)) return TB_FALSE;
	}

	// ok
	return TB_TRUE;
}
static tb_size_t tb_flv_video_h264_sps_analyze_get_exp_golomb(tb_bstream_t* bst)
{
	tb_size_t nbits = 0;
	tb_size_t b = 1;

	while (!tb_bstream_get_u1(bst) && tb_bstream_left_bits(bst))
	{
		++nbits;
		b <<= 1;
	}
	return (b - 1 + tb_bstream_get_ubits32(bst, nbits));
}

/* ////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_handle_t tb_flv_init(tb_gstream_t* gst)
{
	// check 
	tb_assert_and_check_return_val(gst, TB_NULL);

	// alloc flv
	tb_flv_t* flv = tb_malloc0(sizeof(tb_flv_t));
	tb_assert_and_check_return_val(flv, TB_NULL);

	// init flv
	flv->gst 	= gst;
	flv->stail 	= flv->spath;

	// init string
	if (!tb_pstring_init(&flv->string)) goto fail;

	// ok
	return (tb_handle_t)flv;

fail:
	if (flv) tb_flv_exit(flv);
	return TB_NULL;
}
tb_void_t tb_flv_exit(tb_handle_t hflv)
{
	tb_flv_t* flv = (tb_flv_t*)hflv;
	if (flv)
	{	
		// free audio data
		if (flv->audio_data) 
			tb_free(flv->audio_data);

		// free video data
		if (flv->video_data) 
			tb_free(flv->video_data);

		// free audio config data
		if (flv->audio_config_data) 
			tb_free(flv->audio_config_data);

		// free video config data
		if (flv->video_config_data) 
			tb_free(flv->video_config_data);

		// free script data
		if (tb_bstream_valid(&flv->sdata_bst) && tb_bstream_beg(&flv->sdata_bst))
			tb_free(tb_bstream_beg(&flv->sdata_bst));

		// free string
		tb_pstring_exit(&flv->string);

		// free it
		tb_free(flv);
	}
}
tb_bool_t tb_flv_spank(tb_handle_t hflv)
{
	tb_flv_t* flv = (tb_flv_t*)hflv;
	tb_assert_and_check_return_val(flv, TB_FALSE);

	// init variables
	tb_bstream_t 	bst;
	tb_byte_t 		tag[16];
	tb_bool_t 		ret = TB_FALSE;

	// get stream
	tb_gstream_t* gst = flv->gst;
	tb_assert_and_check_return_val(gst, TB_FALSE);

	// spank hdata?
	if ((flv->spank_type & TB_FLV_SPANK_TYPE_HDATA)
		&& !(flv->spank_type_ok & TB_FLV_SPANK_TYPE_HDATA)) 
	{
		// read flv header
		if (!tb_gstream_bread(gst, tag, 9)) goto end;

		// check 
		if (tag[0] != 'F' || tag[1] != 'L' || tag[2] != 'V' || tag[3] > 6) goto end;

		// callback
		if (flv->hdata_cb_func) flv->hdata_cb_func(tag, 9, flv->hdata_cb_data);

		// finish hdata
		flv->spank_type_ok |= TB_FLV_SPANK_TYPE_HDATA;

		// spank ok?
		if (flv->spank_type == flv->spank_type_ok)
			goto ok;
	}

	// is end?
	if (tb_gstream_left(gst) < 16) goto end;
	
	// read packets
	while (tb_gstream_left(gst) >= 15)
	{
		// read flv tag
		if (!tb_gstream_bread(gst, tag, 15)) goto end;
		tb_bstream_attach(&bst, tag, 15);
		tb_uint32_t 	ptag_size = tb_bstream_get_u32_be(&bst);
		tb_uint8_t 		tag_type = tb_bstream_get_u8(&bst);
		tb_uint32_t 	data_size = tb_bstream_get_u24_be(&bst);
		tb_uint32_t 	dts = tb_bstream_get_u24_be(&bst);
		dts |= tb_bstream_get_u8(&bst) << 24;
		tb_bstream_skip(&bst, 3); // skip stream id

		tb_trace_impl("tag type: %d", tag_type);
		tb_trace_impl("tag size: %d", data_size);

		// check tag type
		tb_assert_goto( 	tag_type == TB_FLV_TAG_TYPE_AUDIO 
						|| 	tag_type == TB_FLV_TAG_TYPE_VIDEO 
						|| 	tag_type == TB_FLV_TAG_TYPE_SDATA, end);

		// is end?
		if (tb_gstream_left(gst) < data_size) break;

		// read flv data
		switch (tag_type)
		{
		case TB_FLV_TAG_TYPE_SDATA: // script data	
			{
				// spank sdata?
				if ((flv->spank_type & TB_FLV_SPANK_TYPE_SDATA)
					&& !(flv->spank_type_ok & TB_FLV_SPANK_TYPE_SDATA)) 
				{
					// alloc data
					tb_byte_t* data = tb_malloc0(data_size);
					tb_assert_goto(data, end);

					// attach data
					tb_bstream_attach(&flv->sdata_bst, data, data_size);

					// read data
					if (!tb_gstream_bread(gst, data, data_size)) goto end;

					// spank meta
					if (flv->sdata_cb_func)
					{
						// the first value must be string : onMetaData
						tb_size_t type = tb_bstream_get_u8(&flv->sdata_bst);
						tb_assert_goto(type == TB_FLV_SDATA_TYPE_STRING, end);
						
						// get onMetaData object
						tb_flv_sdata_value_t sdata;
						if (!tb_flv_sdata_object_spank(flv, &sdata)) goto end;
					}

					// callback
					if (flv->sdata_data_cb_func)
					{
						tb_byte_t head_data[11];
						tb_memcpy(head_data, tag + 4, 11);

						// callback
						flv->sdata_data_cb_func(head_data, 11, data, data_size, flv->sdata_data_cb_data);
						tb_trace_impl("read sdata ok: %d bytes", data_size);
					}

					// finish sdata
					flv->spank_type_ok |= TB_FLV_SPANK_TYPE_SDATA;

					// spank ok?
					if (flv->spank_type == flv->spank_type_ok)
						goto ok;
				}
				else
				{
					// skip it
					if (!tb_gstream_bskip(gst, data_size)) goto end;
				}
			}
			break; 
		case TB_FLV_TAG_TYPE_AUDIO: // audio data
			{
				// spank audio?
				if (flv->spank_type & (TB_FLV_SPANK_TYPE_AUDIO_CONFIG | TB_FLV_SPANK_TYPE_AUDIO_DATA)) 
				{
					// read audio flag
					tb_byte_t data[2];
					if (!tb_gstream_bread(gst, data, 2)) goto end;

					// aac?
					if ((data[0] & TB_FLV_AUDIO_CODEC_MASK) != TB_FLV_AUDIO_CODEC_AAC) 
					{
						// skip this tag
						tb_trace_impl("skip this audio tag");
						if (data_size > 2)
						{
							if (!tb_gstream_bskip(gst, data_size - 2)) goto end;
						}
						break;
					}

					// aac type: AAC sequence header?
					if (!data[1] && !flv->audio_config_data 
						&& (flv->spank_type & TB_FLV_SPANK_TYPE_AUDIO_CONFIG)
						&& !(flv->spank_type_ok & TB_FLV_SPANK_TYPE_AUDIO_CONFIG))
					{
						// read audio specific config
						flv->audio_config_size = data_size - 2;
						flv->audio_config_data = tb_malloc(flv->audio_config_size);
						tb_assert_goto(flv->audio_config_data, end);
						if (!tb_gstream_bread(gst, flv->audio_config_data, flv->audio_config_size)) goto end;

						// make head data
						tb_byte_t head_data[13];
						tb_memcpy(head_data, tag + 4, 11);
						tb_memcpy(head_data + 11, data, 2);

						// callback
						if (flv->audio_config_cb_func) flv->audio_config_cb_func(head_data, 13, flv->audio_config_data, flv->audio_config_size, flv->audio_config_cb_data);
						tb_trace_impl("read audio config ok: %d bytes", flv->audio_config_size);

						// finish audio config
						flv->spank_type_ok |= TB_FLV_SPANK_TYPE_AUDIO_CONFIG;

						// spank ok?
						if (flv->spank_type == flv->spank_type_ok)
							goto ok;
					}
					// read audio data
					else if (0x01 == data[1] && (flv->spank_type & TB_FLV_SPANK_TYPE_AUDIO_DATA))
					{
						data_size -= 2;
						if (!flv->audio_data)
						{
							tb_assert(!flv->audio_size && !flv->audio_maxn);
							flv->audio_maxn = data_size + 4096;
							flv->audio_size = data_size;
							flv->audio_data = tb_malloc(flv->audio_maxn);
						}
						else if (flv->audio_maxn < data_size)
						{
							tb_assert(flv->audio_data);
							flv->audio_maxn = data_size + 4096;
							flv->audio_size = data_size;
							flv->audio_data = tb_ralloc(flv->audio_data, flv->audio_maxn);
						}
						else flv->audio_size = data_size;

						tb_assert_goto(flv->audio_data, end);
						if (!tb_gstream_bread(gst, flv->audio_data, flv->audio_size)) goto end;

						// make head data
						tb_byte_t head_data[13];
						tb_memcpy(head_data, tag + 4, 11);
						tb_memcpy(head_data + 11, data, 2);

						// callback
						if (flv->audio_data_cb_func)
						{
							if (!flv->audio_data_cb_func(head_data, 13, flv->audio_data, flv->audio_size, dts, flv->audio_data_cb_data)) goto end;
						}
						tb_trace_impl("read audio data ok: %d bytes", flv->audio_size);
						goto ok;
					}
					else
					{
						// skip this tag	
						if (data_size > 2)
						{
							if (!tb_gstream_bskip(gst, data_size - 2)) goto end;
						}
						break;
					}
				}
				else
				{
					// skip it
					if (!tb_gstream_bskip(gst, data_size)) goto end;
				}
			}
			break;
		case TB_FLV_TAG_TYPE_VIDEO: // video data
			{
				// spank video?
				if (flv->spank_type & (TB_FLV_SPANK_TYPE_VIDEO_CONFIG | TB_FLV_SPANK_TYPE_VIDEO_DATA)) 
				{
					// read video flag
					tb_byte_t data[5];
					if (!tb_gstream_bread(gst, data, 5)) goto end;
			
					// h264?
					if ((data[0] & TB_FLV_VIDEO_CODEC_MASK) != TB_FLV_VIDEO_CODEC_H264) 
					{
						// skip this tag
						tb_trace_impl("skip this video tag");
						if (data_size > 5)
						{
							if (!tb_gstream_bskip(gst, data_size - 5)) goto end;
						}
						break;
					}

					// read type & cts
					tb_uint8_t 	type = tb_bits_get_u8(data + 1);
					tb_sint32_t cts = (tb_bits_get_u24_be(data + 2) + 0xff800000) ^ 0xff800000;

					// read video specific config
					if (!type && !flv->video_config_data 
						&& (flv->spank_type & TB_FLV_SPANK_TYPE_VIDEO_CONFIG) 
						&& !(flv->spank_type_ok & TB_FLV_SPANK_TYPE_VIDEO_CONFIG))
					{
						flv->video_config_size = data_size - 5;
						flv->video_config_data = tb_malloc(flv->video_config_size);
						tb_assert_goto(flv->video_config_data, end);
						if (!tb_gstream_bread(gst, flv->video_config_data, flv->video_config_size)) goto end;

						// make head data
						tb_byte_t head_data[16];
						tb_memcpy(head_data, tag + 4, 11);
						tb_memcpy(head_data + 11, data, 5);

						// callback
						if (flv->video_config_cb_func) flv->video_config_cb_func(head_data, 16, flv->video_config_data, flv->video_config_size, flv->video_config_cb_data);
						tb_trace_impl("read video config ok: %d bytes", flv->video_config_size);

						// finish video config
						flv->spank_type_ok |= TB_FLV_SPANK_TYPE_VIDEO_CONFIG;

						// spank ok?
						if (flv->spank_type == flv->spank_type_ok)
							goto ok;
					}
					// read video data
					else if (1 == type && (flv->spank_type & TB_FLV_SPANK_TYPE_VIDEO_DATA))
					{
						data_size -= 5;
						if (!flv->video_data)
						{
							tb_assert(!flv->video_size && !flv->video_maxn);
							flv->video_maxn = data_size + 4096;
							flv->video_size = data_size;
							flv->video_data = tb_malloc(flv->video_maxn);
						}
						else if (flv->video_maxn < data_size)
						{
							tb_assert(flv->video_data);
							flv->video_maxn = data_size + 4096;
							flv->video_size = data_size;
							flv->video_data = tb_ralloc(flv->video_data, flv->video_maxn);
						}
						else flv->video_size = data_size;

						tb_assert_goto(flv->video_data, end);
						if (!tb_gstream_bread(gst, flv->video_data, flv->video_size)) goto end;

						// make head data
						tb_byte_t head_data[16];
						tb_memcpy(head_data, tag + 4, 11);
						tb_memcpy(head_data + 11, data, 5);

						// callback
						if (flv->video_data_cb_func) 
						{
							if (!flv->video_data_cb_func(head_data, 16, flv->video_data, flv->video_size, dts, cts, flv->video_data_cb_data)) goto end;
						}
						tb_trace_impl("read video data ok: %d bytes", flv->video_size);
						goto ok;
					}
					else
					{
						// skip this tag
						if (data_size > 5)
						{
							if (!tb_gstream_bskip(gst, data_size - 5)) goto end;
						}
						break;
					}
				}
				else
				{
					// skip it
					if (!tb_gstream_bskip(gst, data_size)) goto end;
				}
			}
			break;
		default:
			{
				// skip it
				if (!tb_gstream_bskip(gst, data_size)) goto end;
				break;
			}
		}
	}

ok:
	// ok
	ret = TB_TRUE;

end:
	tb_trace_impl("spank %s", ret? "ok" : "fail");
	return ret;
}
tb_bool_t tb_flv_ioctl(tb_handle_t hflv, tb_size_t cmd, ...)
{
	tb_flv_t* flv = (tb_flv_t*)hflv;
	tb_assert_and_check_return_val(flv, TB_FALSE);

	// init
	tb_bool_t 		ret = TB_FALSE;
	tb_va_list_t 	arg; 
	tb_va_start(arg, cmd);

	// handle
	switch (cmd)
	{
	case TB_FLV_IOCTL_CMD_CB_SDATA:
		{
			flv->sdata_cb_func = (tb_flv_sdata_cb_func_t)tb_va_arg(arg, tb_flv_sdata_cb_func_t);
			flv->sdata_cb_data = (tb_pointer_t)tb_va_arg(arg, tb_pointer_t);
			ret = TB_TRUE;
		}
		break;
	case TB_FLV_IOCTL_CMD_CB_SDATA_DATA:
		{
			flv->sdata_data_cb_func = (tb_flv_sdata_data_cb_func_t)tb_va_arg(arg, tb_flv_sdata_cb_func_t);
			flv->sdata_data_cb_data = (tb_pointer_t)tb_va_arg(arg, tb_pointer_t);
			ret = TB_TRUE;
		}
		break;
	case TB_FLV_IOCTL_CMD_CB_HDATA:
		{
			flv->hdata_cb_func = (tb_flv_hdata_cb_func_t)tb_va_arg(arg, tb_flv_hdata_cb_func_t);
			flv->hdata_cb_data = (tb_pointer_t)tb_va_arg(arg, tb_pointer_t);
			ret = TB_TRUE;
		}
		break;
	case TB_FLV_IOCTL_CMD_CB_AUDIO_CONFIG:
		{
			flv->audio_config_cb_func = (tb_flv_audio_config_cb_func_t)tb_va_arg(arg, tb_flv_audio_config_cb_func_t);
			flv->audio_config_cb_data = (tb_pointer_t)tb_va_arg(arg, tb_pointer_t);
			ret = TB_TRUE;
		}
		break;
	case TB_FLV_IOCTL_CMD_CB_VIDEO_CONFIG:	
		{
			flv->video_config_cb_func = (tb_flv_video_config_cb_func_t)tb_va_arg(arg, tb_flv_video_config_cb_func_t);
			flv->video_config_cb_data = (tb_pointer_t)tb_va_arg(arg, tb_pointer_t);
			ret = TB_TRUE;
		}
		break;
	case TB_FLV_IOCTL_CMD_CB_AUDIO_DATA:
		{
			flv->audio_data_cb_func = (tb_flv_audio_data_cb_func_t)tb_va_arg(arg, tb_flv_audio_data_cb_func_t);
			flv->audio_data_cb_data = (tb_pointer_t)tb_va_arg(arg, tb_pointer_t);
			ret = TB_TRUE;
		}
		break;
	case TB_FLV_IOCTL_CMD_CB_VIDEO_DATA:	
		{
			flv->video_data_cb_func = (tb_flv_video_data_cb_func_t)tb_va_arg(arg, tb_flv_video_data_cb_func_t);
			flv->video_data_cb_data = (tb_pointer_t)tb_va_arg(arg, tb_pointer_t);
			ret = TB_TRUE;
		}
		break;
	case TB_FLV_IOCTL_CMD_SET_STREAM:
		{
			flv->gst = (tb_gstream_t*)tb_va_arg(arg, tb_gstream_t*);
			ret = TB_TRUE;
		}
		break;
	case TB_FLV_IOCTL_CMD_GET_STREAM:
		{
			tb_gstream_t** pgst = (tb_gstream_t**)tb_va_arg(arg, tb_gstream_t**);
			tb_assert_and_check_return_val(pgst, TB_FALSE);
			
			*(pgst) = flv->gst;
			ret = TB_TRUE;
		}
		break;
	case TB_FLV_IOCTL_CMD_SPANK_TYPE:
		{
			flv->spank_type = (tb_size_t)tb_va_arg(arg, tb_size_t);
			ret = TB_TRUE;
		}
		break;
	default:
		break;
	}

	// exit
	tb_va_end(arg);

	// ok?
	return ret;
}

/* ////////////////////////////////////////////////////////////////////////////////
 * helper
 */
tb_size_t tb_flv_video_h264_sps_analyze_remove_emulation(tb_byte_t* sps_data, tb_size_t sps_size)
{
	tb_assert_and_check_return_val(sps_size < 4096, sps_size);
	tb_byte_t 	b[4096] = {0};
	tb_byte_t* 	q = b;
	tb_byte_t* 	p = sps_data;
	tb_byte_t* 	e = sps_data + sps_size;
	while (p < e)
	{
		if (p + 2 < e && p[0] == 0 && p[1] == 0 && p[2] == 3) 
		{
			tb_trace_impl("remove emulation bytes 0x03");
			*q++ = *p++;
			*q++ = *p++;
			p++;
		} 
		else *q++ = *p++;
	}
	if (q > b)
	{
		tb_memcpy(sps_data, b, q - b);
		return q - b;
	}
	return 0;
}
tb_float_t tb_flv_video_h264_sps_analyze_framerate(tb_byte_t* data, tb_size_t size)
{
	// attach data
	tb_bstream_t bst;
	tb_bstream_attach(&bst, data, size);

	// skip forbidden_zero_bit & nal_ref_idc & nal_unit_type
	tb_bstream_skip(&bst, 1);

	tb_size_t profile_idc = tb_bstream_get_u8(&bst);
	tb_trace_impl("profile_idc: %x", profile_idc);

	tb_size_t constraint_setn_flag = tb_bstream_get_u8(&bst);
	tb_trace_impl("constraint_setn_flag: %x", constraint_setn_flag);

	tb_size_t level_idc = tb_bstream_get_u8(&bst);
	tb_trace_impl("level_idc: %x", level_idc);

	tb_size_t seq_parameter_set_id = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
	tb_trace_impl("seq_parameter_set_id: %x", seq_parameter_set_id);

	if (profile_idc == 100 || profile_idc == 110 || profile_idc == 122 || profile_idc == 244 || profile_idc == 44 || profile_idc == 83 || profile_idc == 86 || profile_idc == 118 || profile_idc == 128 ) 
	{
		tb_size_t chroma_format_idc = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
		tb_trace_impl("\tchroma_format_idc: %x", chroma_format_idc);

		if (chroma_format_idc == 3)
		{
			tb_size_t separate_colour_plane_flag = tb_bstream_get_u1(&bst);
			tb_trace_impl("\t\tseparate_colour_plane_flag: %x", separate_colour_plane_flag);
		}

		tb_size_t bit_depth_luma_minus8 = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
		tb_trace_impl("\tbit_depth_luma_minus8: %x", bit_depth_luma_minus8);
	
		tb_size_t bit_depth_chroma_minus8 = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
		tb_trace_impl("\tbit_depth_chroma_minus8: %x", bit_depth_chroma_minus8);
	
		tb_size_t qpprime_y_zero_transform_bypass_flag = tb_bstream_get_u1(&bst);
		tb_trace_impl("\tqpprime_y_zero_transform_bypass_flag: %x", qpprime_y_zero_transform_bypass_flag);
	
		tb_size_t seq_scaling_matrix_present_flag = tb_bstream_get_u1(&bst);
		tb_trace_impl("\tseq_scaling_matrix_present_flag: %x", seq_scaling_matrix_present_flag);

		if (seq_scaling_matrix_present_flag)
		{
			tb_size_t i = 0;
			for (i = 0; i < ((chroma_format_idc != 3) ? 8 : 12); ++i) 
			{
				tb_size_t seq_scaling_list_present_flag = tb_bstream_get_u1(&bst);
				tb_trace_impl("\t\tseq_scaling_list_present_flag: %x", seq_scaling_list_present_flag);

				if (seq_scaling_list_present_flag) 
				{
					tb_size_t sizeofscalinglist = i < 6 ? 16 : 64;
					tb_size_t lastscale = 8;
					tb_size_t nextscale = 8;
					tb_size_t j = 0;
					for (j = 0; j < sizeofscalinglist; ++j) 
					{
						if (nextscale)
						{
							tb_size_t delta_scale = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
							tb_trace_impl("\t\t\tdelta_scale: %x", delta_scale);
							nextscale = (lastscale + delta_scale + 256) % 256;
						}
						lastscale = (nextscale == 0) ? lastscale : nextscale;
					}
				}
			}
		}
	}

	tb_size_t log2_max_frame_num_minus4 = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
	tb_trace_impl("log2_max_frame_num_minus4: %x", log2_max_frame_num_minus4);

	tb_size_t pic_order_cnt_type = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
	tb_trace_impl("pic_order_cnt_type: %x", pic_order_cnt_type);

	if (pic_order_cnt_type == 0) 
	{
		tb_size_t log2_max_pic_order_cnt_lsb_minus4 = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
		tb_trace_impl("\tlog2_max_pic_order_cnt_lsb_minus4: %x", log2_max_pic_order_cnt_lsb_minus4);
	}
	else if (pic_order_cnt_type == 1) 
	{
		tb_size_t delta_pic_order_always_zero_flag = tb_bstream_get_u1(&bst);
		tb_trace_impl("\tdelta_pic_order_always_zero_flag: %x", delta_pic_order_always_zero_flag);

		tb_size_t offset_for_non_ref_pic = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
		tb_trace_impl("\toffset_for_non_ref_pic: %x", offset_for_non_ref_pic);

		tb_size_t offset_for_top_to_bottom_field = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
		tb_trace_impl("\toffset_for_top_to_bottom_field: %x", offset_for_top_to_bottom_field);

		tb_size_t num_ref_frames_in_pic_order_cnt_cycle = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
		tb_trace_impl("\tnum_ref_frames_in_pic_order_cnt_cycle: %x", num_ref_frames_in_pic_order_cnt_cycle);

		tb_size_t i = 0;
		for (i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; ++i)
		{
			tb_size_t offset_for_ref_frame = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
			tb_trace_impl("\t\toffset_for_ref_frame[%u]: %x", i, offset_for_ref_frame);
		}
	}
	tb_size_t max_num_ref_frames = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
	tb_trace_impl("max_num_ref_frames: %x", max_num_ref_frames);

	tb_size_t gaps_in_frame_num_value_allowed_flag = tb_bstream_get_u1(&bst);
	tb_trace_impl("gaps_in_frame_num_value_allowed_flag: %x", gaps_in_frame_num_value_allowed_flag);
	
	tb_size_t pic_width_in_mbs_minus1 = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
	tb_trace_impl("pic_width_in_mbs_minus1: %x", pic_width_in_mbs_minus1);

	tb_size_t pic_height_in_map_units_minus1 = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
	tb_trace_impl("pic_height_in_map_units_minus1: %x", pic_height_in_map_units_minus1);

	tb_size_t frame_mbs_only_flag = tb_bstream_get_u1(&bst);
	tb_trace_impl("frame_mbs_only_flag: %x", frame_mbs_only_flag);
	
	if (!frame_mbs_only_flag) 
	{
		tb_size_t mb_adaptive_frame_field_flag = tb_bstream_get_u1(&bst);
		tb_trace_impl("\tmb_adaptive_frame_field_flag: %x", mb_adaptive_frame_field_flag);
	}

	tb_size_t direct_8x8_inference_flag = tb_bstream_get_u1(&bst);
	tb_trace_impl("direct_8x8_inference_flag: %x", direct_8x8_inference_flag);

	tb_size_t frame_cropping_flag = tb_bstream_get_u1(&bst);
	tb_trace_impl("frame_cropping_flag: %x", frame_cropping_flag);

	if (frame_cropping_flag)
	{
		tb_size_t frame_crop_left_offset = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
		tb_trace_impl("\tframe_crop_left_offset: %x", frame_crop_left_offset);

		tb_size_t frame_crop_right_offset = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
		tb_trace_impl("\tframe_crop_right_offset: %x", frame_crop_right_offset);

		tb_size_t frame_crop_top_offset = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
		tb_trace_impl("\tframe_crop_top_offset: %x", frame_crop_top_offset);

		tb_size_t frame_crop_bottom_offset = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
		tb_trace_impl("\tframe_crop_bottom_offset: %x", frame_crop_bottom_offset);
	}

	tb_size_t vui_parameters_present_flag = tb_bstream_get_u1(&bst);
	tb_trace_impl("vui_parameters_present_flag: %x", vui_parameters_present_flag);

	if (vui_parameters_present_flag)
	{
		tb_size_t aspect_ratio_info_present_flag = tb_bstream_get_u1(&bst);
		tb_trace_impl("\taspect_ratio_info_present_flag: %x", aspect_ratio_info_present_flag);

		if (aspect_ratio_info_present_flag)
		{
			tb_size_t aspect_ratio_idc = tb_bstream_get_ubits32(&bst, 8);
			tb_trace_impl("\t\taspect_ratio_idc: %x", aspect_ratio_idc);

			if (aspect_ratio_idc == 255)
			{		
				tb_size_t sar_width = tb_bstream_get_ubits32(&bst, 16);
				tb_trace_impl("\t\t\tsar_width: %x", sar_width);

				tb_size_t sar_height = tb_bstream_get_ubits32(&bst, 16);
				tb_trace_impl("\t\t\tsar_height: %x", sar_height);
			}
		}
		tb_size_t overscan_info_present_flag = tb_bstream_get_u1(&bst);
		tb_trace_impl("\toverscan_info_present_flag: %x", overscan_info_present_flag);

		if (overscan_info_present_flag)
		{	
			tb_size_t overscan_appropriate_flag = tb_bstream_get_u1(&bst);
			tb_trace_impl("\t\toverscan_appropriate_flag: %x", overscan_appropriate_flag);
		}
		
		tb_size_t video_signal_type_present_flag = tb_bstream_get_u1(&bst);
		tb_trace_impl("\tvideo_signal_type_present_flag: %x", video_signal_type_present_flag);

		if (video_signal_type_present_flag)
		{
			// skip video_format & video_full_range_flag
			tb_bstream_skip_bits(&bst, 4);
	
			tb_size_t colour_description_present_flag = tb_bstream_get_u1(&bst);
			tb_trace_impl("\t\tcolour_description_present_flag: %x", colour_description_present_flag);
	
			if (colour_description_present_flag)
			{
				// skip colour_primaries & transfer_characteristics & matrix_coefficients	
				tb_bstream_skip_bits(&bst, 24);
			}
		}

		tb_size_t chroma_loc_info_present_flag = tb_bstream_get_u1(&bst);
		tb_trace_impl("\tchroma_loc_info_present_flag: %x", chroma_loc_info_present_flag);

		if (chroma_loc_info_present_flag)
		{
			tb_size_t chroma_sample_loc_type_top_field = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
			tb_trace_impl("\t\tchroma_sample_loc_type_top_field: %x", chroma_sample_loc_type_top_field);

			tb_size_t chroma_sample_loc_type_bottom_field = tb_flv_video_h264_sps_analyze_get_exp_golomb(&bst);
			tb_trace_impl("\t\tchroma_sample_loc_type_bottom_field: %x", chroma_sample_loc_type_bottom_field);
		}

		tb_size_t timing_info_present_flag = tb_bstream_get_u1(&bst);
		tb_trace_impl("\ttiming_info_present_flag: %x", timing_info_present_flag);

		if (timing_info_present_flag)
		{
			tb_size_t num_units_in_tick = tb_bstream_get_ubits32(&bst, 32);
			tb_trace_impl("\t\tnum_units_in_tick: %x", num_units_in_tick);
	
			tb_size_t time_scale = tb_bstream_get_ubits32(&bst, 32);
			tb_trace_impl("\t\ttime_scale: %x", time_scale);
	
			tb_size_t fixed_frame_rate_flag = tb_bstream_get_u1(&bst);
			tb_trace_impl("\t\tfixed_frame_rate_flag: %x", fixed_frame_rate_flag);

			if (time_scale > 0 && num_units_in_tick > 0)
			{
				tb_float_t framerate = time_scale / (2.0 * num_units_in_tick);
				tb_trace_impl("\t\t\tfixed => framerate: %f", framerate);
				return framerate;
			}
		}

		// analyze ...
	}

	tb_trace_impl("left: %u bits", tb_bstream_left_bits(&bst));

	return 0.;
}
