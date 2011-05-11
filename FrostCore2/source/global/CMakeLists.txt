
file(GLOB_RECURSE src_authentication Authentication/*.cpp Authentication/*.h)
file(GLOB_RECURSE src_packets Packets/*.cpp Packets/*.h)
file(GLOB_RECURSE src_spells Spells/*.cpp Spells/*.h)
file(GLOB_RECURSE src_common Common/*.cpp Common/*.h)
file(GLOB_RECURSE src_database Database/*.cpp Database/*.h)
file(GLOB_RECURSE src_crypt Cryptography/*.cpp Cryptography/*.h)
file(GLOB_RECURSE src_authserver AuthServer/*.cpp AuthServer/*.h)
file(GLOB_RECURSE src_worldserver WorldServer/*.cpp WorldServer/*.h)

source_group(Authentication FILES ${src_authentication})
source_group(Packets FILES ${src_packets})
source_group(Spells FILES ${src_spells})
source_group(Common FILES ${src_common})
source_group(Database FILES ${src_database})
source_group(Cryptography FILES ${src_crypt})
source_group(AuthServer FILES ${src_authserver})
source_group(WorldServer FILES ${src_worldserver})

set(global_static_src
  ${src_authentication}
  ${src_packets}
  ${src_spells}
  ${src_common}
  ${src_database}
  ${src_crypt}
  ${src_authserver}
  ${src_worldserver}
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Authentication
  ${CMAKE_CURRENT_SOURCE_DIR}/Packets
  ${CMAKE_CURRENT_SOURCE_DIR}/Spells
  ${CMAKE_CURRENT_SOURCE_DIR}/Common
  ${CMAKE_CURRENT_SOURCE_DIR}/Database
  ${CMAKE_CURRENT_SOURCE_DIR}/Cryptography
  ${CMAKE_CURRENT_SOURCE_DIR}/AuthServer
  ${CMAKE_CURRENT_SOURCE_DIR}/WorldServer
  ${CMAKE_SOURCE_DIR}/libs
  ${CMAKE_SOURCE_DIR}/include
)

add_library(global STATIC ${global_static_src})

find_package(OpenSSL REQUIRED)
find_package(MySQL REQUIRED)

target_link_libraries(global
  ${OPENSSL_LIBRARIES}
  ${OPENSSL_EAY_LIBRARIES}
  ${MYSQL_LIBRARY}
)

