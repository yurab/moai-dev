cmake_minimum_required(VERSION 2.8.5)
project(blackberry-host)


if ( BUILD_BLACKBERRY )

set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSING_GL20")

set ( BLACKBERRY_HOST_INCLUDES "../../src/;../../src/hosts/BlackBerry10/src/" CACHE INTERNAL "doc string" )

include_directories ( 
  ${BLACKBERRY_INCLUDES}
  ${BLACKBERRY_HOST_INCLUDES}
  # ${MOAICORE_INCLUDES}
  # ${LUA_INCLUDES} 
  # ${ZLIB_INCLUDES}
  # ${GLEW_INCLUDES}
   )

set ( CMAKE_CURRENT_SOURCE_DIR "../../src/hosts/BlackBerry10/src" )

add_executable ( blackberry-host 
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/bbutil.c
)

target_link_libraries ( blackberry-host aku moai-core uslscore zlcore third-party bps screen GLESv2 GLESv1_CL EGL freetype png m curl sqlite3 ssl socket crypto cpp-ne )

#
# BAR file creation
#
add_custom_command (
  TARGET blackberry-host
  POST_BUILD
  COMMAND blackberry-nativepackager -package blackberry-host.bar ../${CMAKE_CURRENT_SOURCE_DIR}/../bar-descriptor.xml -e blackberry-host blackberry-host ../${CMAKE_CURRENT_SOURCE_DIR}/../icon.png
)

endif ( BUILD_BLACKBERRY )