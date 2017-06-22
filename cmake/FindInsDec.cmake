
# Find libradare
if (InsDec_LIBRARIES AND InsDec_INCLUDE_DIRS)
  set (InsDec_FIND_QUIETLY TRUE)
endif()

find_path(InsDec_INCLUDE_DIRS NAMES ins_dec.h HINTS ENV C_INCLUDE_PATH )
find_library(InsDec_LIBRARIES NAMES ins_dec HINTS ENV LIBRARY_PATH ENV LD_LIBRARY_PATH)

include (FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(InsDec DEFAULT_MSG
  InsDec_LIBRARIES
  InsDec_INCLUDE_DIRS)

mark_as_advanced(InsDec_INCLUDE_DIRS InsDec_LIBRARIES)
