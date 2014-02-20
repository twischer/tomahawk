# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
SET(CMAKE_C_COMPILER   /usr/bin/arm-linux-gnueabihf-gcc-4.7)
SET(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++-4.7)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH  /usr/arm-linux-gnueabihf/)


SET(TAGLIB_FOUND  TRUE)
SET(TAGLIB_LIBRARIES  "-L/usr/arm-linux-gnueabihf/lib -ltag")
SET(TAGLIB_CFLAGS  "-I/usr/arm-linux-gnueabihf/include/taglib")
SET(TAGLIB_INCLUDES  ";/usr/arm-linux-gnueabihf/include/taglib")


# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


SET( THREADS_PTHREAD_ARG "2")
