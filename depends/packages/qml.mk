PACKAGE=qml
qml_package:=qml
$(qml_package)_version=5.10.1
$(qml_package)_download_path=http://download.qt.io/archive/qt/5.10/5.10.1/single/
$(qml_package)_suffix=everywhere-src-$($(qml_package)_version).tar.xz
$(qml_package)_file_name=qt-$($(qml_package)_suffix)
$(qml_package)_sha256_hash=05ffba7b811b854ed558abf2be2ddbd3bb6ddd0b60ea4b5da75d277ac15e740a
$(package)_dependencies=openssl zlib
$(package)_linux_dependencies=freetype fontconfig libxcb libX11 xproto libXext
$(package)_patches=xcb.patch

define $(qml_package)_set_vars
$(qml_package)_config_opts_release = -release
$(qml_package)_config_opts_debug = -debug
#$(qml_package)_config_opts += -hostprefix $(build_prefix)
$(qml_package)_config_opts += -opensource
$(qml_package)_config_opts += -c++std c++14
$(qml_package)_config_opts += -confirm-license

#$(qml_package)_config_opts += -no-opengl
$(qml_package)_config_opts += -nomake examples
$(qml_package)_config_opts += -nomake tests
$(qml_package)_config_opts += -dbus-runtime
$(qml_package)_config_opts += -skip wayland
$(qml_package)_config_opts += -skip purchasing
$(qml_package)_config_opts += -skip serialbus
$(qml_package)_config_opts += -skip qtserialport
$(qml_package)_config_opts += -skip qtgamepad
$(qml_package)_config_opts += -skip qtscript
$(qml_package)_config_opts += -skip qtdoc
$(qml_package)_config_opts += -skip location
$(qml_package)_config_opts += -skip webchannel
$(qml_package)_config_opts += -skip webengine
$(qml_package)_config_opts += -skip websockets
$(qml_package)_config_opts += -skip webview
$(qml_package)_config_opts += -skip remoteobjects
$(qml_package)_config_opts += -qt-zlib
$(qml_package)_config_opts += -qt-libpng
$(qml_package)_config_opts += -qt-libjpeg
$(qml_package)_config_opts += -qt-pcre
$(qml_package)_config_opts += -openssl-linked OPENSSL_PREFIX=$(host_prefix)

$(qml_package)_config_opts += -no-icu
$(qml_package)_config_opts += -no-iconv
$(qml_package)_config_opts += -no-alsa
$(qml_package)_config_opts += -no-egl
$(qml_package)_config_opts += -no-eglfs
$(qml_package)_config_opts += -no-freetype
$(qml_package)_config_opts += -no-gif
$(qml_package)_config_opts += -no-glib
$(qml_package)_config_opts += -no-gstreamer
$(qml_package)_config_opts += -no-kms
$(qml_package)_config_opts += -no-linuxfb
$(qml_package)_config_opts += -no-libudev
$(qml_package)_config_opts += -no-mtdev
$(qml_package)_config_opts += -no-pulseaudio
$(qml_package)_config_opts += -no-openvg
$(qml_package)_config_opts += -no-reduce-relocations
$(qml_package)_config_opts += -no-sql-db2
$(qml_package)_config_opts += -no-sql-ibase
$(qml_package)_config_opts += -no-sql-oci
$(qml_package)_config_opts += -no-sql-tds
$(qml_package)_config_opts += -no-sql-mysql
$(qml_package)_config_opts += -no-sql-odbc
$(qml_package)_config_opts += -no-sql-psql
$(qml_package)_config_opts += -no-sql-sqlite
$(qml_package)_config_opts += -no-sql-sqlite2
$(qml_package)_config_opts += -no-use-gold-linker
$(qml_package)_config_opts += -no-xinput2


$(qml_package)_config_opts += -optimized-qmake
$(qml_package)_config_opts += -pch
$(qml_package)_config_opts += -pkg-config
$(qml_package)_config_opts += -prefix $(host_prefix)/qml


$(qml_package)_config_opts += -static
$(qml_package)_config_opts += -silent
$(qml_package)_config_opts += -v
#$(qml_package)_config_opts += -no-feature-printer
#$(qml_package)_config_opts += -no-feature-printdialog


ifneq ($(build_os),darwin)
$(qml_package)_config_opts_darwin = -xplatform macx-clang-linux
$(qml_package)_config_opts_darwin += -device-option MAC_SDK_PATH=$(OSX_SDK)
$(qml_package)_config_opts_darwin += -device-option MAC_SDK_VERSION=$(OSX_SDK_VERSION)
$(qml_package)_config_opts_darwin += -device-option CROSS_COMPILE="$(host)-"
$(qml_package)_config_opts_darwin += -device-option MAC_MIN_VERSION=$(OSX_MIN_VERSION)
$(qml_package)_config_opts_darwin += -device-option MAC_TARGET=$(host)
$(qml_package)_config_opts_darwin += -device-option MAC_LD64_VERSION=$(LD64_VERSION)
endif


$(qml_package)_config_opts_linux  = -qt-xkbcommon
$(qml_package)_config_opts_linux += -qt-xcb
$(qml_package)_config_opts_linux += -system-freetype
$(qml_package)_config_opts_linux += -no-sm
$(qml_package)_config_opts_linux += -fontconfig
$(qml_package)_config_opts_linux += -opengl desktop
$(qml_package)_config_opts_linux += -reduce-exports
$(qml_package)_config_opts_linux += OPENSSL_LIBS="-lssl -lcrypto"


$(qml_package)_config_opts_mingw32  += -xplatform win32-g++ -device-option CROSS_COMPILE="$(host)-"
$(qml_package)_config_opts_mingw32 += OPENSSL_LIBS="-lssl -lcrypto -lgdi32"

$(qml_package)_config_opts_mingw32 += -skip qtactiveqt
$(qml_package)_config_opts_mingw32 += -debug-and-release
$(qml_package)_config_opts_mingw32 += -opengl desktop
#$(qml_package)_config_opts_mingw32 += -angle



endef

define $(qml_package)_fetch_cmds
  $(call fetch_file,$(qml_package),$($(qml_package)_download_path),$($(qml_package)_download_file),$($(qml_package)_file_name),$($(qml_package)_sha256_hash))

endef

define $(package)_preprocess_cmds
  cd $($(qml_package)_version) && \
  patch -p1 < $($(package)_patch_dir)/xcb.patch  && \
  cd ../
endef


define $(qml_package)_extract_cmds
  mkdir -p $($(package)_extract_dir) && \
  echo "$($(qml_package)_sha256_hash)  $($(qml_package)_source)" > $($(qml_package)_extract_dir)/.$($(qml_package)_file_name).hash && \
  $(build_SHA256SUM) -c $($(package)_extract_dir)/.$($(qml_package)_file_name).hash && \
  mkdir $($(qml_package)_version) && \
  tar --strip-components=1 -xf $($(qml_package)_source) -C $($(qml_package)_version)
endef


define $(qml_package)_config_cmds
  export PKG_CONFIG_SYSROOT_DIR=/ && \
  export PKG_CONFIG_LIBDIR=$(host_prefix)/lib/pkgconfig && \
  export PKG_CONFIG_PATH=$(host_prefix)/share/pkgconfig  && \
  cd $($(qml_package)_version) && \
  ./configure $($(qml_package)_config_opts)
endef

define $(qml_package)_build_cmds
  cd $($(qml_package)_version) && \
  $(MAKE)
endef

define $(package)_stage_cmds
  cd $($(qml_package)_version) && \
  $(MAKE) INSTALL_ROOT=$($(package)_staging_dir) install
#  cp qtbase/bin/qmake $($(package)_staging_dir)/$(host_prefix)/bin && \
#  cp qtbase/bin/moc $($(package)_staging_dir)/$(host_prefix)/bin && \
#  cp qtbase/bin/rcc $($(package)_staging_dir)/$(host_prefix)/bin && \
#  cp qtbase/bin/qt.conf $($(package)_staging_dir)/$(host_prefix)/bin && \
#  cp -r qtbase/mkspecs $($(package)_staging_dir)/$(host_prefix) && \
#  exit 0
endef
