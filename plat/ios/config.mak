# demo
TB_CONFIG_DEMO_LIBS				= stdc++ z m dl pthread
TB_CONFIG_DEMO_CMD1				= @ldid -S $(1)$(BIN_SUFFIX)

# plat
TB_CONFIG_PLAT 					= ios

# api
TB_CONFIG_API_DLL 				= unix
TB_CONFIG_API_FILE 				= unix
TB_CONFIG_API_SOCK 				= unix
TB_CONFIG_API_UTIL 				= unix
TB_CONFIG_API_MUTX 				= unix
TB_CONFIG_API_THRD 				= unix
TB_CONFIG_API_ASIO 				= unix
TB_CONFIG_API_TIME 				= unix
TB_CONFIG_API_EVENT 			= unix
TB_CONFIG_API_SEMAPHORE 		= unix
TB_CONFIG_API_PRINT 			= unix
TB_CONFIG_API_PROCESS 			= unix
TB_CONFIG_API_MEMORY 			= libc

# third party
TB_CONFIG_THIRD_ZLIB 			= y

# memory
TB_CONFIG_MEMORY_POOL 			= $(DEBUG)

# float
TB_CONFIG_TYPE_FLOAT 			= y

