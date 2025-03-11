# cpp-preview
C++新特性前瞻

本repo主要用来学习记录C++20/23/26以及未来新版本特性。
主要参考网站：
* [cppreference](https://en.cppreference.com/)

目前gcc对新版本特性支持最好，参见[compiler_support](https://en.cppreference.com/w/cpp/compiler_support)。所以学习过程中始终使用最新稳定版gcc。

### 编译安装gcc
稳定发行版Linux大概率不支持最新版本gcc，所以需要手动编译安装。为了不影响开发机环境，将gcc安装到个人HOME目录下。参考[InstallingGCC](https://gcc.gnu.org/wiki/InstallingGCC)。
```shell
wget --no-check-certificate https://mirrors.ocf.berkeley.edu/gnu/gcc/gcc-14.2.0/gcc-14.2.0.tar.gz
tar zxf gcc-14.2.0.tar.gz
cd gcc-14.2.0
contrib/download_prerequisites
mkdir build && cd build

../configure -v \
    --build=x86_64-linux-gnu \
    --host=x86_64-linux-gnu \
    --target=x86_64-linux-gnu \
    --prefix=$HOME/gcc-14.2.0 \
    --enable-checking=release \
    --enable-languages=c,c++ \
    --disable-multilib

make -j 4
make install-strip
```
使用时添加PATH和LD_LIBRARY_PATH。
```shell
export PATH=$HOME/gcc-14.2.0/bin:$PATH
export LD_LIBRARY_PATH=$HOME/gcc-14.2.0/lib64:$LD_LIBRARY_PATH
```
