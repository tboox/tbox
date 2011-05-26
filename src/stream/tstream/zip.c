/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		zstream.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "tstream.h"
#include "../bstream.h"
#include "../../zip/zip.h"

/* /////////////////////////////////////////////////////////
 * types
 */

// the encoding stream type
typedef struct __tb_zstream_t
{
	// the stream base
	tb_tstream_t 			base;

	// the zip package
	tb_zip_package_t 		package;

	// the zip algorithm
	tb_size_t 				algo;

	// the zip action
	tb_size_t 				action;

	// the zip 
	tb_zip_t* 				zip;

}tb_zstream_t;

/* /////////////////////////////////////////////////////////
 * implements
 */

static __tb_inline__ tb_zstream_t* tb_zstream_cast(tb_gstream_t* gst)
{
	tb_tstream_t* tst = tb_tstream_cast(gst);
	TB_ASSERT_RETURN_VAL(tst && tst->type == TB_TSTREAM_TYPE_ZIP, TB_NULL);
	return (tb_zstream_t*)tst;
}
static tb_bool_t tb_zstream_open(tb_gstream_t* gst)
{
	tb_zstream_t* zst = tb_zstream_cast(gst);
	TB_ASSERT_RETURN_VAL(zst, TB_FALSE);

	// open zip
	zst->zip = tb_zip_open(&zst->package, zst->algo, zst->action);
	TB_ASSERT_RETURN_VAL(zst->zip, TB_FALSE);

	// open tstream
	return tb_tstream_open(gst);
}
static void tb_zstream_close(tb_gstream_t* gst)
{
	tb_zstream_t* zst = tb_zstream_cast(gst);
	TB_ASSERT_RETURN(zst);

	// close zip
	if (zst->zip) tb_zip_close(zst->zip);

	// close tstream
	tb_tstream_close(gst);
}
static tb_bool_t tb_zstream_ioctl1(tb_gstream_t* gst, tb_size_t cmd, void* arg1)
{
	tb_zstream_t* zst = tb_zstream_cast(gst);
	TB_ASSERT_RETURN_VAL(zst, TB_FALSE);

	// handle it
	switch (cmd)
	{
	case TB_ZSTREAM_CMD_GET_ALGO:
		{
			tb_zip_algo_t* pa = (tb_zip_algo_t*)arg1;
			TB_ASSERT_RETURN_VAL(pa, TB_FALSE);
			*pa = zst->algo;
			return TB_TRUE;
		}
	case TB_ZSTREAM_CMD_GET_ACTION:
		{
			tb_zip_action_t* pa = (tb_zip_action_t*)arg1;
			TB_ASSERT_RETURN_VAL(pa, TB_FALSE);
			*pa = zst->action;
			return TB_TRUE;
		}
	case TB_ZSTREAM_CMD_SET_ALGO:
		{
			zst->algo = (tb_size_t)arg1;
			return TB_TRUE;
		}
	case TB_ZSTREAM_CMD_SET_ACTION:
		{
			zst->action = (tb_size_t)arg1;
			return TB_TRUE;
		}
	default:
		break;
	}

	// routine to tstream 
	return tb_tstream_ioctl1(gst, cmd, arg1);
}
static tb_bool_t tb_zstream_spank(tb_gstream_t* gst)
{
	tb_zstream_t* zst = tb_zstream_cast(gst);
	tb_tstream_t* tst = tb_tstream_cast(gst);
	TB_ASSERT_RETURN_VAL(zst && tst, TB_FALSE);

	// get zip
	tb_zip_t* zip = zst->zip;
	TB_ASSERT_RETURN_VAL(zip && zip->spank, TB_FALSE);

	// get input
	TB_ASSERT_RETURN_VAL(tst->ip && tst->in, TB_FALSE);
	tb_byte_t const* ip = tst->ip;
	tb_byte_t const* ie = ip + tst->in;

	// get output
	TB_ASSERT_RETURN_VAL(tst->op, TB_FALSE);
	tb_byte_t* op = tst->op;
	tb_byte_t* oe = tst->ob + TB_GSTREAM_BLOCK_SIZE;

	// attach bstream
	tb_bstream_t ist, ost;
	tb_bstream_attach(&ist, ip, ie - ip);
	tb_bstream_attach(&ost, op, oe - op);

	// spank it
	tb_zip_status_t ret = tb_zip_spank(zip, &ist, &ost);
	if (ret == TB_ZIP_STATUS_FAIL) return TB_FALSE;
	TB_ASSERT_RETURN_VAL(ret == TB_ZIP_STATUS_OK || ret == TB_ZIP_STATUS_END, TB_FALSE);
	//while (ip < ie && op < oe) *op++ = *ip++;

	// update pointer
	ip = ist.p;
	op = ost.p;

	// check
	TB_ASSERT_RETURN_VAL(ip >= tst->ip && ip <= ie, TB_FALSE);
	TB_ASSERT_RETURN_VAL(op >= tst->op && op <= oe, TB_FALSE);

	// update input
	tst->in -= ip - tst->ip;
	tst->ip = ip;

	// update output
	tst->on += op - tst->op;

	// update status
	if (ret == TB_ZIP_STATUS_END) tst->status = TB_TSTREAM_STATUS_END;

	return TB_TRUE;
}
/* /////////////////////////////////////////////////////////
 * interfaces
 */
tb_gstream_t* tb_gstream_create_zip()
{
	// create stream
	tb_gstream_t* gst = (tb_gstream_t*)tb_calloc(1, sizeof(tb_zstream_t));
	tb_tstream_t* tst = (tb_tstream_t*)gst;
	TB_ASSERT_RETURN_VAL(gst, TB_NULL);

	// init gstream
	gst->type 	= TB_GSTREAM_TYPE_TRAN;
	gst->open 	= tb_zstream_open;
	gst->close 	= tb_zstream_close;
	gst->read 	= tb_tstream_read;
	gst->ioctl1 = tb_zstream_ioctl1;

	// init tstream
	tst->type 	= TB_TSTREAM_TYPE_ZIP;
	tst->spank 	= tb_zstream_spank;

	return gst;
}
tb_gstream_t* tb_gstream_create_from_zip(tb_gstream_t* gst, tb_size_t algo, tb_size_t action)
{
	TB_ASSERT_RETURN_VAL(gst, TB_NULL);

	// create encoding stream
	tb_gstream_t* zst = tb_gstream_create_zip();
	TB_ASSERT_RETURN_VAL(zst, TB_NULL);

	// set gstream
	if (TB_FALSE == tb_gstream_ioctl1(zst, TB_TSTREAM_CMD_SET_GSTREAM, (void*)gst)) goto fail;
		
	// set zip algorithm
	if (TB_FALSE == tb_gstream_ioctl1(zst, TB_ZSTREAM_CMD_SET_ALGO, (void*)algo)) goto fail;
		
	// set zip action
	if (TB_FALSE == tb_gstream_ioctl1(zst, TB_ZSTREAM_CMD_SET_ACTION, (void*)action)) goto fail;
	
	return zst;

fail:
	if (zst) tb_gstream_destroy(zst);
	return TB_NULL;
}

