/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "tbox.h"
#import <Foundation/Foundation.h>
#include <asl.h>

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t test_m_main(tb_int_t argc, tb_char_t** argv)
{
	NSLog(@"hello %@", [NSString stringWithCString:"world" encoding:NSUTF8StringEncoding]);

	return 0;
}
