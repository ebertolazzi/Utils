############################################################################
#                                                                          #
#  file: CMakeLists.txt                                                    #
#  Copyright (C) 2016                                                      #
#      Enrico Bertolazzi                                                   #
#      Dipartimento di Ingegneria Industriale                              #
#      Universita` degli Studi di Trento                                   #
#      email: enrico.bertolazzi@ing.unitn.it                               #
#                                                                          #
############################################################################

CMAKE_MINIMUM_REQUIRED( VERSION 2.8.12 )
SET( CMAKE_VERBOSE_MAKEFILE OFF )
SET( CMAKE_INSTALL_MESSAGE NEVER )

PROJECT( Utils CXX C )

IF ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  SET( DEBUG_SUFFIX "_debug" )
ELSE()
  SET( DEBUG_SUFFIX )
ENDIF()

IF(APPLE)
  SET( TARGETS Utils_osx_static${DEBUG_SUFFIX} )
ELSEIF( UNIX )
  SET( TARGETS Utils_linux_static${DEBUG_SUFFIX} )
ELSE()
  SET( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON )
  SET( TARGETS Utils_win_${BITS}_static${DEBUG_SUFFIX} )
ENDIF()

IF ( BUILD_SHARED )
  IF(APPLE)
    SET( TARGET Utils_osx${DEBUG_SUFFIX} )
  ELSEIF( UNIX )
    SET( TARGET Utils_linux${DEBUG_SUFFIX} )
  ELSE()
    SET( TARGET Utils_win_${BITS}${DEBUG_SUFFIX} )
  ENDIF()
ENDIF()

SET( CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/lib" )

INCLUDE( ./CMakeLists-cflags.txt )

MESSAGE( STATUS "Compiler used: ${CMAKE_CXX_COMPILER_ID}")

SET( CMAKE_C_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE} )
SET( CMAKE_C_FLAGS_DEBUG   ${CMAKE_CXX_FLAGS_DEBUG} )

SET( SOURCES )
FILE( GLOB S ./src/*.cc )
FOREACH (F ${S})
  FILE( RELATIVE_PATH RF ${CMAKE_CURRENT_SOURCE_DIR} "${F}" )
  LIST( APPEND SOURCES ${RF} )
ENDFOREACH (F ${S})

SET( HEADERS )
FILE( GLOB S ./src/*.h* )
FOREACH (F ${S})
  FILE( RELATIVE_PATH RF ${CMAKE_CURRENT_SOURCE_DIR} "${F}" )
  LIST( APPEND HEADERS ${RF} )
ENDFOREACH (F ${S})
FILE( GLOB S ./src/*/*.h* )
FOREACH (F ${S})
  FILE( RELATIVE_PATH RF ${CMAKE_CURRENT_SOURCE_DIR} "${F}" )
  LIST( APPEND HEADERS ${RF} )
ENDFOREACH (F ${S})
FILE( GLOB S ./src/*/*/*.h* )
FOREACH (F ${S})
  FILE( RELATIVE_PATH RF ${CMAKE_CURRENT_SOURCE_DIR} "${F}" )
  LIST( APPEND HEADERS ${RF} )
ENDFOREACH (F ${S})

ADD_LIBRARY( ${TARGETS} STATIC ${SOURCES} ${HEADERS} )

IF ( BUILD_SHARED )
  ADD_LIBRARY( ${TARGET} SHARED ${SOURCES} ${HEADERS} )
ENDIF()

MESSAGE( STATUS "YEAR = ${YEAR}" )
MESSAGE( STATUS "BITS = ${BITS}" )

INCLUDE_DIRECTORIES( src src/Utils lib3rd/include )

IF( BUILD_EXECUTABLE )

  IF(APPLE OR UNIX)
    FIND_PACKAGE( ZLIB REQUIRED )
    INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIRS} )
  ELSE()
    IF ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
      FIND_LIBRARY( ZLIB_LIBRARIES zlibstaticd HINTS ./ThirdParties/zlib/lib )
    ELSE()
      FIND_LIBRARY( ZLIB_LIBRARIES zlibstatic HINTS ./ThirdParties/zlib/lib )
    ENDIF()
    INCLUDE_DIRECTORIES( ./ThirdParties/zlib/include )
  ENDIF()

  SET( EXELISTCPP test_Malloc test_Quaternion test_rang test_table test_Thread test_trace test_zstream )

  SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
  FILE(MAKE_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/bin )
  FOREACH( S ${EXELISTCPP} )
    ADD_EXECUTABLE( ${S} ${CMAKE_CURRENT_SOURCE_DIR}/src_tests/${S}.cc ${HEADERS} )
    IF ( UNIX AND NOT APPLE )
      TARGET_LINK_LIBRARIES( ${S} ${TARGETS} ${lapackblas_libraries} ${ZLIB_LIBRARIES} -ldl )
    ELSE()
      TARGET_LINK_LIBRARIES( ${S} ${TARGETS} ${lapackblas_libraries} ${ZLIB_LIBRARIES} )
    ENDIF()
    ADD_TEST(
      ${S} COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bin/${S}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
  ENDFOREACH()

ENDIF()

FILE( GLOB HDR       ./src/*.h* )
FILE( GLOB HDR1      ./src/Utils/*.h* )
FILE( GLOB HDR_FMT   ./src/Utils/fmt/*.h* )
FILE( GLOB HDR_ZS    ./src/Utils/zstream/*.h* )
FILE( GLOB HDR_MINGW ./src/Utils/mingw-std-threads/*.h* ./src/Utils/mingw-std-threads/README.* ./src/Utils/mingw-std-threads/LICENSE )
FILE( GLOB HDR_ZLIB  ./lib3rd/include/*.h* )
FILE( GLOB LIB_ZLIB  ./lib3rd/lib/*.* )
FILE( GLOB DLL_ZLIB  ./lib3rd/dll/*.* )

IF ( BUILD_SHARED )
  INSTALL(
    TARGETS ${TARGETS} ${TARGET}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/dll
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  )
ELSE()
  INSTALL(
    TARGETS ${TARGETS}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/dll
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  )
ENDIF()

INSTALL(FILES ${HDR}       DESTINATION ${CMAKE_INSTALL_PREFIX}/include )
INSTALL(FILES ${HDR1}      DESTINATION ${CMAKE_INSTALL_PREFIX}/include/Utils )
INSTALL(FILES ${HDR_FMT}   DESTINATION ${CMAKE_INSTALL_PREFIX}/include/Utils/fmt )
INSTALL(FILES ${HDR_ZS}    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/Utils/zstream )
INSTALL(FILES ${HDR_MINGW} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/Utils/mingw-std-threads )
INSTALL(FILES ${HDR_ZLIB}  DESTINATION ${CMAKE_INSTALL_PREFIX}/include )
INSTALL(FILES ${LIB_ZLIB}  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )
INSTALL(FILES ${DLL_ZLIB}  DESTINATION ${CMAKE_INSTALL_PREFIX}/dll )
INSTALL( DIRECTORY ./src/Eigen DESTINATION ${CMAKE_INSTALL_PREFIX}/include )


MESSAGE( STATUS "Using ${SSE_FLAGS} extensions")
MESSAGE( STATUS "C compiler                  = ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER}" )
MESSAGE( STATUS "C++ compiler                = ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER}" )
MESSAGE( STATUS "CMAKE_CXX_FLAGS_RELEASE     = ${CMAKE_CXX_FLAGS_RELEASE}" )
MESSAGE( STATUS "CMAKE_CXX_FLAGS             = ${CMAKE_CXX_FLAGS}" )
MESSAGE( STATUS "CMAKE_C_FLAGS_RELEASE       = ${CMAKE_C_FLAGS_RELEASE}" )
MESSAGE( STATUS "CMAKE_C_FLAGS               = ${CMAKE_C_FLAGS}" )
MESSAGE( STATUS "CMAKE_SYSTEM_NAME           = ${CMAKE_SYSTEM_NAME}" )
MESSAGE( STATUS "CMAKE_SYSTEM_PROCESSOR      = ${CMAKE_SYSTEM_PROCESSOR}" )
MESSAGE( STATUS "EXTRA_LIBS                  = ${EXTRA_LIBS}" )
IF(APPLE)
  MESSAGE( STATUS "CMAKE_OSX_SYSROOT           = ${CMAKE_OSX_SYSROOT}" )
  MESSAGE( STATUS "CMAKE_OSX_ARCHITECTURES     = ${CMAKE_OSX_ARCHITECTURES}" )
  MESSAGE( STATUS "CMAKE_OSX_DEPLOYMENT_TARGET = ${CMAKE_OSX_DEPLOYMENT_TARGET}" )
ENDIF()
