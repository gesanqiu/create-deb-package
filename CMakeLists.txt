# created by Ricardo Lu in 05-25-2021

cmake_minimum_required(VERSION 3.10)

PROJECT(netUsageMonitor)


#准备的版本设置
SET(MAJOR_VERSION 1)
SET(MINOR_VERSION 0)
SET(PATCH_VERSION 1)

#说明要生成的是deb包
SET(CPACK_GENERATOR "DEB")

##############设置debian/control文件中的内容###############

#设置版本信息
SET(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")

#设置安装包的包名，打好的包将会是packagename-version-linux.deb，如果不设置，默认是工程名
set(CPACK_PACKAGE_NAME "network_bandwidth_cal")

#设置程序名，就是程序安装后的名字
set(CPACK_DEBIAN_PACKAGE_NAME "network-bw-dev")

#设置架构
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "arm64")

#设置依赖
# SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.1-6)")

#设置section
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")

#设置priority
set(CPACK_DEBIAN_PACKAGE_PRIORITY "Optional")

#设置description
SET(CPACK_PACKAGE_DESCRIPTION "A simple network bandwidth calculator.")

#设置联系方式
SET(CPACK_PACKAGE_CONTACT "shenglu1202@163.com")

#设置维护人
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Ricardo Lu")

set(SOURCE ${PROJECT_SOURCE_DIR}/src)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

include_directories(${CMAKE_SOURCE_DIR}/inc)

add_library(${PROJECT_NAME}
    SHARED
    ${SOURCE}/netUsageMonitor.c
)

install(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION /usr/lib
)

install(
    FILES inc/netUsageMonitor.h
    DESTINATION /usr/include
)

include(CPack)