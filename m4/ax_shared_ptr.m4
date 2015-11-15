AC_DEFUN([AX_SHARED_PTR], [

AC_LANG(C++)

AC_MSG_CHECKING([for std::shared_ptr])

AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
    [[#include <memory>]]
    [[std::shared_ptr<int> have_shared_ptr;]])
], [
    AC_MSG_RESULT([yes])
    AC_DEFINE_UNQUOTED([HAVE_STD_SHARED_PTR], 1, [Define to 1 if you have the `std::shared_ptr' class.])
], [
    AC_MSG_RESULT([no])
])

AC_MSG_CHECKING([for std::tr1::shared_ptr])

AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
    [[#include <tr1/memory>]]
    [[std::tr1::shared_ptr<int> have_std_tr1_shared_ptr;]])
], [
    AC_MSG_RESULT([yes])
    AC_DEFINE_UNQUOTED([HAVE_STD_TR1_SHARED_PTR], 1, [Define to 1 if you have the `std::tr1::shared_ptr' class.])
], [
    AC_MSG_RESULT([no])
])

AC_MSG_CHECKING([for boost::shared_ptr])

AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
    [[#include <boost/shared_ptr.hpp>]]
    [[boost::shared_ptr<int> have_boost_shared_ptr;]])
], [
    AC_MSG_RESULT([yes])
    AC_DEFINE_UNQUOTED([HAVE_BOOST_SHARED_PTR], 1, [Define to 1 if you have the `boost::shared_ptr' class.])
], [
    AC_MSG_RESULT([no])
])

AC_MSG_CHECKING([for boost::tr1::shared_ptr])

AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
    [[#include <boost/tr1/shared_ptr.hpp>]]
    [[boost::tr1::shared_ptr<int> have_boost_tr1_shared_ptr;]])
], [
    AC_MSG_RESULT([yes])
    AC_DEFINE_UNQUOTED([HAVE_BOOST_TR1_SHARED_PTR], 1, [Define to 1 if you have the `boost::tr1::shared_ptr' class.])
], [
    AC_MSG_RESULT([no])
])

])
