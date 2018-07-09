# Initiates
AC_INIT([testblock.exe], [1.0])

# Check if file exists
AC_CONFIG_SRCDIR([src/Block.cpp])
AC_CONFIG_SRCDIR([src/test_Block.cpp])

# Puts aux files in that folder
AC_CONFIG_AUX_DIR([./aux_files])

# Make config header
AC_CONFIG_HEADERS([config.h])

# Initiates automake
AM_INIT_AUTOMAKE([1.11 -Wall -Werror])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])

# Find C++ Compiler
AC_PROG_CXX

# Output
AC_OUTPUT
