file(GLOB src_boost_thread thread/*.cpp thread/*.hpp)
file(GLOB_RECURSE src_boost_thread_win thread/win32/*.cpp thread/win32/*.hpp)
file(GLOB_RECURSE src_boost_thread_unix thread/pthread/*.cpp thread/pthread/*.hpp)

if(WIN32)
set(boost_thread_static_src
  ${src_boost_thread}
  ${src_boost_thread_win}
)
message("Use Win32 Thread library")
else(UNIX)
set(boost_thread_static_src
  ${src_boost_thread}
  ${src_boost_thread_unix}
)
message("Use Unix Thread library")
endif(WIN32)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/system
  ${CMAKE_CURRENT_SOURCE_DIR}/thread
  ${CMAKE_CURRENT_SOURCE_DIR}/thread/pthread
  ${CMAKE_CURRENT_SOURCE_DIR}/thread/win32
  ${CMAKE_SOURCE_DIR}/include
)
add_library(boost_lib_thread STATIC ${boost_thread_static_src})

if(WIN32)
target_link_libraries (boost_lib_thread
)
else(UNIX)
target_link_libraries (boost_lib_thread
  pthread
)
endif(WIN32)
