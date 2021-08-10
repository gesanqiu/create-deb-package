`deb`是`Unix`系统(其实主要是`Linux`)下的安装包，基于 tar 包，因此本身会记录文件的权限(读/写/可执行)以及所有者/用户组。
由于 Unix 类系统对权限、所有者、组的严格要求，而 deb 格式安装包又经常会涉及到系统比较底层的操作，所以权限等的设置尤其重要。

本文讲解了如何使用CMake的CPack工具，项目repo：[create-deb-package](https://github.com/gesanqiu/create-deb-package)。

## 使用dpkg-deb生成deb安装包

### deb包的组成

```shell
.
└── package	# dpkg-deb -b ./ network_bandwidth_cal-1.0.1-Linux.deb
	└── DEBIAN
       ├── control
       ├── postinst		# postinstallation
       ├── postrm		# postremove
       ├── preinst		# preinstallation
       ├── prerm		# preremove
       ├── copyright	# 版权
       ├── changlog		# 修订记录
       └── conffiles
   └── usr
        ├── include
        │   └── netUsageMonitor.h
        └── lib
            └── libnetUsageMonitor.so
```

deb包本身有三部分组成：

- 数据包，包含实际安装的程序数据，文件名为`data.tar.XXX`；
- 安装信息及控制脚本包，包含deb的安装说明，标识，脚本等，文件名为`control.tar.gz`；
- 最后一个是deb文件的一些二进制数据，包括文件头等信息，一般看不到，在某些软件中打开可以看到。

> deb本身可以使用不同的压缩方式. tar格式并不是一种压缩格式，而是直接把分散的文件和目录集合在一起，并记录其权限等数据信息。之前提到过的 data.tar.XXX，这里 XXX 就是经过压缩后的后缀名. deb默认使用的压缩格式为gzip格式，所以最常见的就是 data.tar.gz。常有的压缩格式还有 bzip2 和 lzma，其中 lzma 压缩率最高，但压缩需要的 CPU 资源和时间都比较长

- `data.tar.gz`包含的是实际安装的程序数据，而在安装过程中，该包里的数据会被直接解压到根目录(即 / )，因此在打包之前需要根据文件所在位置设置好相应的文件/目录树。

- `control.tar.gz`则包含了一个 deb 安装的时候所需要的控制信息.

`DEBIAN`目录下为控制脚本：

- `control`：用了记录软件标识，版本号，平台，依赖信息等数据；
- `preinst`：在解包`data.tar.gz`前运行的脚本，停止作用于待升级软件包的服务，直到软件包安装或升级完成；
- `postinst`：在解包数据后运行的脚本，一般用来配置软件执行环境，必须以`#!/bin/sh`为首行；
- `prerm`：卸载时，在删除文件之前运行的脚本，该脚本负责停止与软件包相关联的`daemon`服务；
- `postrm`：在删除文件之后运行的脚本，负责修改软件包链接或文件关联，或删除由它创建的文件。

### deb-control

`dpkg-deb`打包的核心是 `DEBIAN`目录下的`control`脚本，关于这个脚本的详细说明可以参考官方文档：[deb-control man page](http://manpages.ubuntu.com/manpages/bionic/man5/deb-control.5.html)，以下是一个样例：

```ini
Package: network-bw-dev
Version: 1.0.1
Section: utils
Priority: optional
Architecture: arm64
Depends:
Description: A simple network bandwidth calculator.
Maintainer: Ricardo Lu <shenglu1202@163.com>

```

**注：**`control`文件必须是UTF-8编码格式，最后必须保留一行空行。

```shell
# 可能会报关于权限的错误
sudo chmod 755 * -R
dpkg-deb -b ./ network_bandwidth_cal-1.0.1-Linux.deb
```

## 使用CPacke生成deb包

```cmake
# created by Ricardo Lu in 05-25-2021

cmake_minimum_required(VERSION 3.10)

PROJECT(netUsageMonitor)


# 准备的版本设置
SET(MAJOR_VERSION 1)
SET(MINOR_VERSION 0)
SET(PATCH_VERSION 1)

# 说明要生成的是deb包
SET(CPACK_GENERATOR "DEB")
# 设置可重定目录的选择为 ON
set(CPACK_RPM_PACKAGE_RELOCATABLE ON)
# 设置默认重定的目录
set(CPACK_PACKAGING_INSTALL_PREFIX "./deb")

##############设置debian/control文件中的内容###############
# 设置版本信息
SET(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
# 设置安装包的包名，打好的包将会是packagename-version-linux.deb，如果不设置，默认是工程名
set(CPACK_PACKAGE_NAME "network_bandwidth_cal")
# 设置程序名，就是程序安装后的名字
set(CPACK_DEBIAN_PACKAGE_NAME "network-bw-dev")
# 设置架构
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "arm64")
# 设置依赖
## SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.1-6)")
# 设置section
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
# 设置priority
set(CPACK_DEBIAN_PACKAGE_PRIORITY "Optional")
# 设置description
SET(CPACK_PACKAGE_DESCRIPTION "A simple network bandwidth calculator.")
# 设置联系方式
SET(CPACK_PACKAGE_CONTACT "shenglu1202@163.com")
# 设置维护人
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Ricardo Lu")

include(CPack)

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
```

[CPack](https://cmake.org/cmake/help/latest/module/CPack.html)是 CMake 2.4.2 之后的一个内置工具，用于创建软件的二进制包和源代码包，因为CPack生成`DEB`包所设置的参数基本与`DEBIAN/control`几乎一致，猜测其在实现上依赖于`dpkg-deb`，简单的理解是：CPack将生成一个用于构建deb包的`control`控制脚本，并且将`install`命令指定的安装树空间作为该deb包的安装信息进行打包。

**注：**在这个样例中只展示了如何将当前构建项目打包成一个deb包，实际CPack是支持将一个项目分装成多个包：使用**cpack_add_component** 和 **cpack_add_component_group**，并且在**install**时将安装的文件捆绑到不同的**component**上即可。

