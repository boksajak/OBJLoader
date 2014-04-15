cmake_minimum_required (VERSION 2.6)
project (OBJLoader)

set_property(GLOBAL PROPERTY USE_FOLDERS On)

macro(post_build_make_dir_link target source  destination )
    if(WIN32)
        add_custom_command(TARGET ${target} PRE_BUILD  COMMAND rmdir /Q   \"${destination}\" || set errorLevel = 0)
        add_custom_command(TARGET ${target} POST_BUILD COMMAND mklink /D  \"${destination}\" \"${source}\" || set errorLevel = 0)
    else(UNIX)
        add_custom_command(TARGET ${target} POST_BUILD COMMAND rm -f \"${destination}\")
        add_custom_command(TARGET ${target} POST_BUILD COMMAND ln -s \"${source}\" \"${destination}\")
    endif(WIN32)
endmacro(post_build_make_dir_link)


MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        SET(dirlist ${dirlist} ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

file(GLOB_RECURSE INCS "*.h")
file(GLOB_RECURSE CPPS "*.cpp")


IF(MINGW)
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
  endif(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_CXX_STANDARD_LIBRARIES -lstdc++)
  set(CMAKE_CXX_FLAGS "-std=c++11 -m64 -fpermissive")
  set(CMAKE_CXX_FLAGS_RELEASE "-fno-exceptions -fno-rtti -ffast-math -ffunction-sections -fomit-frame-pointer -mavx -ftree-vectorize -O3 -fwhole-program -flto")
  set(CMAKE_CXX_FLAGS_DEBUG "-g")
ENDIF()

add_executable(OBJLoader ${CPPS} ${INCS})


# Set target directory for copying. Can refer to target which hasn't been added yet.
set(COPY_TARGET_DIR $<TARGET_FILE_DIR:OBJLoader>)
post_build_make_dir_link(OBJLoader ${PROJECT_SOURCE_DIR}/../assets  ${COPY_TARGET_DIR}/assets)
if(MSVC)
    post_build_make_dir_link(OBJLoader ${PROJECT_SOURCE_DIR}/../assets  ${PROJECT_BINARY_DIR}/assets)
endif(MSVC)
