使用（编译）流程
======

依赖工具集合库
------

+ 支持c++0x或c++11的编译器(为了代码尽量简洁,特别是少做无意义的平台兼容，依赖部分 C11和C++11的功能，所以不支持过低版本的编译器)
> + GCC: 4.4 及以上（建议gcc 4.8.1及以上）
> + Clang: 3.0 及以上 （建议 clang 3.4及以上）
> + VC: 10 及以上 （建议VC 12及以上）

+ [cmake](https://cmake.org/download/) 3.1.0 以上(建议 3.4 以上)
+ [msgpack](https://github.com/msgpack/msgpack-c)
> 用于协议打解包,仅使用头文件

+ [libuv](http://libuv.org/)（用于网络通道）
> 用于协议打解包,仅使用头文件
> 
> 手动编译libuv则需要额外的软件集，基本有: gcc/clang/msvc, autoconf, automake, make, pthread, m4

+ [atframe_utils](https://github.com/atframework/atframe_utils)（基础公共代码，检测到不存在会自动下载）
+ (可选)[python](http://python.org/) （周边工具集）
+ (可选)tar, curl, wget: 如果使用内置的脚本自动构建依赖的库，则这些是必要的工具


环境准备(开发环境最小依赖)
------
### Windows + MSVC
1. [cmake](https://cmake.org/download/)
2. [visual studio](https://www.visualstudio.com)
3. [libuv](http://dist.libuv.org/dist)
4. 执行 mkdir build && cd build && cmake .. -G "Visual Studio 14 2015 Win64" -DLIBUV_ROOT=[libuv安装目录] -DMSGPACK_ROOT=[msgpack的include目录所在位置]

详情可参考 [Appceyor CI](../appveyor.yml) 脚本

**生成目标(-G 选项的值)请按自己所在环境手动修改，否则会使用默认的配置，MSVC的默认会生成x86版本的工程文件**

上面最后一条命令可以根据实际环境修改参数，这里只提供一个示例

### Windows + MinGW(msys2)
1. 安装[Msys2](http://msys2.github.io/)
2. Msys2里安装依赖组件
> ```
> for pkg_name in m4 curl wget tar autoconf automake mingw-w64-i686-toolchain mingw-w64-x86_64-toolchain mingw-w64-x86_64-libtool mingw-w64-i686-libtool python; do pacman -S $pkg_name; done
> ```
> 具体可以根据 编译目标裁剪，这里把32位和64位依赖库都安装上了

3. MinGW shell 下执行cmake构建命令
> 注意一定是要MingGW的shell（libuv不支持msys shell），msys2默认开启的是msys shell
> 
> 通常情况下可以通过以下方式打开MingGW shell
> + C:\msys64\msys2_shell.cmd -mingw32    -- 32位
> + C:\msys64\msys2_shell.cmd -mingw64    -- 64位
> 
> 请指定生成 MSYS Makefiles 工程文件(-G "MSYS Makefiles")

其他MinGW环境请自行安装依赖库

详情可参考 [Appceyor CI](../appveyor.yml) 脚本

### Linux
1. gcc 4.4及以上
2. autoconf
3. gdb
4. valgrind
5. curl
6. wget
7. tar
8. m4
9. cmake
10. automake
11. make

以上请用Linux发行版的包管理器安装，然后正常使用cmake即可

比如CentOS:
```
yum install gcc autoconf automake gdb valgrind curl wget tar m4 automake make;
wget -c "https://github.com/Kitware/CMake/archive/v3.4.3.tar.gz" -O "CMake-3.4.3.tar.gz";
tar -xvf "CMake-3.4.3.tar.gz";
cd CMake-3.4.3;
./bootstrap --prefix=/usr/local/cmake-3.4.3
make -j4 install;
ln -sf /usr/local/cmake-3.4.3/bin/cmake /usr/bin/cmake;
```

详情可参考 [Travis CI](../.travis.yml) 脚本

### OSX
1. 安装 [brew](http://brew.sh/)
2. 安装 xcode
3. sudo brew install gcc gdb autoconf automake make curl wget tar m4 cmake

详情可参考 [Travis CI](../.travis.yml) 脚本

编译和构建
------
### 编译选项
除了cmake标准编译选项外，libatbus还提供一些额外选项

+ CMAKE_BUILD_TYPE (默认: Debug): 构建类型，目前**默认是Debug**方式构建。生产环境建议使用**-DCMAKE_BUILD_TYPE=RelWithDebInfo**(相当于gcc -O2 -g -ggdb -DNDEBUG)
+ LIBUV_ROOT: 手动指定libuv的安装目录
+ MSGPACK_ROOT: 手动指定msgpack的安装目录，也可以不安装直接指向msgpack的源码目录
+ ============= 以上选项根据实际环境配置，以下选项不建议修改 =============
+ ATBUS_MACRO_BUSID_TYPE (默认: uint64_t): busid的类型，建议不要设置成大于64位，否则需要修改protocol目录内的busid类型，并且重新生成协议文件
+ ATBUS_MACRO_DATA_NODE_SIZE (默认: 128): atbus的内存通道node大小（必须是2的倍数）
+ ATBUS_MACRO_DATA_ALIGN_TYPE (默认: uint64_t): atbus的内存内存块对齐类型（用于优化memcpy和校验）
+ ATBUS_MACRO_DATA_SMALL_SIZE (默认: 3072): 流通道小数据块大小（用于优化减少内存拷贝）
+ ATBUS_MACRO_HUGETLB_SIZE (默认: 4194304): 大页表分页大小（用于优化共享内存分页,此功能暂时关闭，所以并不生效）
+ ATBUS_MACRO_MSG_LIMIT (默认: 65536): 默认消息体大小限制
+ ATBUS_MACRO_CONNECTION_CONFIRM_TIMEOUT (默认: 30): 默认连接确认时限
+ ATBUS_MACRO_CONNECTION_BACKLOG (默认: 128): 默认握手队列的最大连接数
+ GTEST_ROOT: 使用GTest单元测试框架
+ BOOST_ROOT: 设置Boost库根目录
+ PROJECT_TEST_ENABLE_BOOST_UNIT_TEST: 使用Boost.Test单元测试框架(如果GTEST_ROOT和此项都不设置，则使用内置单元测试框架)
+ PROJECT_ENABLE_SAMPLE (默认: NO): 是否编译Sample代码
+ PROJECT_ENABLE_UNITTEST (默认: NO): 是否编译单元测试代码
+ PROJECT_ENABLE_TOOLS (默认: NO): 是否编译工具集（主要是压力测试工具）


### 示例构建脚本
```
git clone "https://github.com/owt5008137/libatbus.git"
mkdir libatbus/build;
cd libatbus/build;
# cmake .. -G "MSYS Makefiles" -DLIBUV_ROOT=c:/workspace/projs/github/libatbus/3rd_party/libuv/prebuilt/mingw64 -DMSGPACK_ROOT=C:/workspace/lib/protocol/msgpack/msgpack-1.4.0
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/libatbus;
make;
make install;
```

### 运行和测试

单元测试会生成在[构建目录]/test下，sample会生成在[构建目录]/sample下，工具程序（比如压力测试程序）会生成在[构建目录]/tools下。并且不会被install

make install 仅会安装include目录和编译好的libs

**注意:** Windows下MSVC直接安装的libuv会链接dll库，但是构建脚本不会把dll拷贝到执行目录，所以直接运行可能会出错。这时候把libuv.dll手动拷贝到执行目录即可 

附加说明
------
### 目录结构说明

+ 3rd_party: 外部组件（不一定是依赖项）
+ doc: 文档目录
+ include: 导出lib的包含文件（注意不导出的内部接口后文件会直接在src目录里）
+ project: 工程工具和配置文件集
+ protocol: 协议描述文件目录
+ sample: 使用示例目录，每一个cpp文件都是一个完全独立的例子
+ src: 源文件和内部接口申明目录
+ test: 测试框架及测试用例目录