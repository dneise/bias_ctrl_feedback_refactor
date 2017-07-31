dnl @synopsis MYSQL_DEVEL
dnl 
dnl This macro tries to find MySQL C API header locations.
dnl
dnl Based on MYSQL_C_API_LOCATION from
dnl
dnl    @version 1.4, 2009/05/28
dnl    @author Warren Young <mysqlpp@etr-usa.com>
dnl
AC_DEFUN([MYSQL_DEVEL],
[
	MYSQL_inc_check="/usr/include/mysql /usr/local/include/mysql /usr/local/mysql/include /usr/local/mysql/include/mysql /usr/mysql/include/mysql /opt/mysql/include/mysql /sw/include/mysql"
	AC_ARG_WITH(mysql-include,
		[  --with-mysql-include=<path> directory path of MySQL header installation],
		[MYSQL_inc_check="$with_mysql_include $with_mysql_include/include $with_mysql_include/include/mysql"])

	#
	# Look for MySQL C API headers
	#
	AC_MSG_CHECKING([for MySQL include directory])
	MYSQL_C_INC_DIR=
	for m in $MYSQL_inc_check
	do
		if test -d "$m" && test -f "$m/mysql.h"
		then
			MYSQL_C_INC_DIR=$m
			break
		fi
	done

	if test -z "$MYSQL_C_INC_DIR"
	then
		AC_MSG_ERROR([Didn't find the MySQL include dir in '$MYSQL_inc_check'])
	fi

	case "$MYSQL_C_INC_DIR" in
		/* ) ;;
		* )  AC_MSG_ERROR([The MySQL include directory ($MYSQL_C_INC_DIR) must be an absolute path.]) ;;
	esac

	AC_MSG_RESULT([$MYSQL_C_INC_DIR])

	CPPFLAGS="$CPPFLAGS -I${MYSQL_C_INC_DIR}"

	AC_SUBST(MYSQL_C_INC_DIR)
]) dnl MYSQL_DEVEL

