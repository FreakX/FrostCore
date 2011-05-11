
file(GLOB_RECURSE src_authserver_files ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

set(src_authserver
  ${src_authserver_files}
)

source_group(AuthServer FILES ${src_authserver})

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/source/global
  ${CMAKE_SOURCE_DIR}/source/global/Packets
  ${CMAKE_SOURCE_DIR}/source/global/Authentication
  ${CMAKE_SOURCE_DIR}/source/global/Common
  ${CMAKE_SOURCE_DIR}/source/global/Sockets
  ${CMAKE_SOURCE_DIR}/source/global/AuthServer
  ${CMAKE_SOURCE_DIR}/source/global/Cryptography
  ${CMAKE_SOURCE_DIR}/source/global/Database
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/libs
)
add_executable(AuthServer ${src_authserver})

find_package(OpenSSL REQUIRED)
find_package(MySQL REQUIRED)

target_link_libraries (AuthServer
  boost_lib_thread
  global
  ${OPENSSL_LIBRARIES}
  ${OPENSSL_EAY_LIBRARIES}
  ${MYSQL_LIBRARY}
)
