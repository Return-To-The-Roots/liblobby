# - Try to find MySQL.
# Once done this will define:
# MYSQL_FOUND			- If false, do not try to use MySQL.
# MYSQL_INCLUDE_DIRS	- Where to find mysql.h, etc.
# MYSQL_LIBRARIES		- The libraries to link against.
# MYSQL_VERSION_STRING	- Version in a string of MySQL.
#
# Use as find_package(MYSQL REQUIRED)
# include_directories(${MYSQL_INCLUDE_DIR})

set(_PROGRAMFILESx86 "PROGRAMFILES(x86)")
find_path(MYSQL_INCLUDE_DIR
	NAMES "mysql/mysql.h"
	PATHS "$ENV{PROGRAMFILES}/MySQL/*/include"
		  "$ENV{${_PROGRAMFILESx86}}/MySQL/*/include"
		  "$ENV{SYSTEMDRIVE}/MySQL/*/include"
	PATH_SUFFIXES "include")
find_library(MYSQL_LIBRARY
	NAMES "mysqlclient" "mysqlclient_r"
	PATHS "$ENV{PROGRAMFILES}/MySQL/*/lib"
		  "$ENV{${_PROGRAMFILESx86}}/MySQL/*/lib"
		  "$ENV{SYSTEMDRIVE}/MySQL/*/lib"
	PATH_SUFFIXES "lib")

# handle the QUIETLY and REQUIRED arguments and set MYSQL_FOUND to TRUE if found
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MYSQL DEFAULT_MSG MYSQL_LIBRARY MYSQL_INCLUDE_DIRS)

set(MYSQL_INCLUDE_DIRS ${MYSQL_INCLUDE_DIR})
set(MYSQL_LIBRARIES ${MYSQL_LIBRARY})
mark_as_advanced(MYSQL_INCLUDE_DIR MYSQL_LIBRARY)
