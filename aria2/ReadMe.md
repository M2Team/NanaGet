# The aria2 binaries for NanaGet

## Notes

- I only compile libssh2 dependency manually because msys2's libssh2 depends
  on OpenSSL and I want only to use Secure Channel.
- If you want to compile ARM64 version of aria2 binary, you need an ARM64 
  Windows PC first because I have no idea for cross-compile ARM64 target 
  successfully with msys2 toolchain on x64 Windows.
- If you want to compile x86 version of aria2 binary, you need to comment out 
  the something like `EXTRACPPFLAGS="$EXTRACPPFLAGS -D_USE_32BIT_TIME_T"` in
  configure.ac from aria2 source code because msys2 clang32 toolchain have 
  64-bit time_t only.

## Commands

```
https://www.msys2.org

pacman -S mingw-w64-clang-i686-autotools mingw-w64-clang-i686-cc mingw-w64-clang-i686-pkg-config mingw-w64-clang-i686-gcc-libs mingw-w64-clang-i686-zlib mingw-w64-clang-i686-sqlite3 mingw-w64-clang-i686-c-ares mingw-w64-clang-i686-expat mingw-w64-clang-i686-gmp

pacman -S mingw-w64-clang-x86_64-autotools mingw-w64-clang-x86_64-cc mingw-w64-clang-x86_64-pkg-config mingw-w64-clang-x86_64-gcc-libs mingw-w64-clang-x86_64-zlib mingw-w64-clang-x86_64-sqlite3 mingw-w64-clang-x86_64-c-ares mingw-w64-clang-x86_64-expat mingw-w64-clang-x86_64-gmp

pacman -S mingw-w64-clang-aarch64-autotools mingw-w64-clang-aarch64-cc mingw-w64-clang-aarch64-pkg-config mingw-w64-clang-aarch64-gcc-libs mingw-w64-clang-aarch64-zlib mingw-w64-clang-aarch64-sqlite3 mingw-w64-clang-aarch64-c-ares mingw-w64-clang-aarch64-expat mingw-w64-clang-aarch64-gmp


https://libssh2.org/download/libssh2-1.10.0.tar.gz

cd /c/msys2/libssh2-1.10.0

./configure --disable-shared --enable-static --with-crypto=wincng LIBS="-lws2_32" --prefix=/clang32 --host=i686-w64-mingw32 CC=i686-w64-mingw32-clang CXX=i686-w64-mingw32-clang++ LD=i686-w64-mingw32-lld
make install -j8
make clean

./configure --disable-shared --enable-static --with-crypto=wincng LIBS="-lws2_32" --prefix=/clang64 --host=x86_64-w64-mingw32 CC=x86_64-w64-mingw32-clang CXX=x86_64-w64-mingw32-clang++ LD=x86_64-w64-mingw32-lld
make install -j8
make clean

./configure --disable-shared --enable-static --with-crypto=wincng LIBS="-lws2_32" --prefix=/clangarm64 --host=aarch64-w64-mingw32 CC=aarch64-w64-mingw32-clang CXX=aarch64-w64-mingw32-clang++ LD=aarch64-w64-mingw32-lld
make install -j8
make clean


https://github.com/aria2/aria2/releases/download/release-1.36.0/aria2-1.36.0.tar.xz

cd /c/msys2/aria2-1.36.0

autoreconf -ifv

./configure --disable-shared --enable-static --disable-websocket --without-included-gettext --disable-nls --with-libcares --without-gnutls --without-openssl --with-sqlite3 --without-libxml2 --with-libexpat --with-libz --with-libgmp --with-libssh2 --without-libgcrypt --without-libnettle ARIA2_STATIC=yes --prefix=/c/msys2/rootfs/x86 LDFLAGS="-Wl,--gc-sections"
make install -j16
strip -s /c/msys2/rootfs/x86/bin/aria2c.exe
make clean

./configure --disable-shared --enable-static --disable-websocket --without-included-gettext --disable-nls --with-libcares --without-gnutls --without-openssl --with-sqlite3 --without-libxml2 --with-libexpat --with-libz --with-libgmp --with-libssh2 --without-libgcrypt --without-libnettle ARIA2_STATIC=yes --prefix=/c/msys2/rootfs/x64 LDFLAGS="-Wl,--gc-sections"
make install -j16
strip -s /c/msys2/rootfs/x64/bin/aria2c.exe
make clean

./configure --disable-shared --enable-static --disable-websocket --without-included-gettext --disable-nls --with-libcares --without-gnutls --without-openssl --with-sqlite3 --without-libxml2 --with-libexpat --with-libz --with-libgmp --with-libssh2 --without-libgcrypt --without-libnettle ARIA2_STATIC=yes --prefix=/c/msys2/rootfs/arm64 LDFLAGS="-Wl,--gc-sections"
make install -j16
strip -s /c/msys2/rootfs/arm64/bin/aria2c.exe
make clean
```
