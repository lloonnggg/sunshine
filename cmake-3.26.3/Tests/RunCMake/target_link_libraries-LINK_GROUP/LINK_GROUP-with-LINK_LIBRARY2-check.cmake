
if (NOT actual_stdout MATCHES "(/|-)-START_GROUP\"? +\"?(/|-)-PREFIX_LIBRARY\"? +\"?(/|-)-LIBFLAG.*${LINK_SHARED_LIBRARY_PREFIX}base1${LINK_SHARED_LIBRARY_SUFFIX}\"? +\"?(/|-)-LIBFLAG.*${LINK_SHARED_LIBRARY_PREFIX}base2${LINK_SHARED_LIBRARY_SUFFIX}\"? +\"?(/|-)-SUFFIX_LIBRARY\"? +\"?(/|-)-END_GROUP")
  set (RunCMake_TEST_FAILED "Not found expected '--START_GROUP --PREFIX_LIBRARY --LIBFLAG<base1> --LIBFLAG<base2> --END_GROUP'.")
endif()
