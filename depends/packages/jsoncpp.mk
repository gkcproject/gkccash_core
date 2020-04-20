package=jsoncpp
$(package)_version=0.10.7
$(package)_download_path=https://github.com/open-source-parsers/$(package)/archive
$(package)_file_name=$($(package)_version).tar.gz
$(package)_sha256_hash=73e235c230708a8ac78ec11b886434a018f89691bd9e7fcf9c3128c8e677b435
$(package)_patches=$(package)-path.patch

define $(package)_set_vars
$(package)_config_opts=-DCMAKE_INSTALL_PREFIX=$($(package)_staging_prefix_dir) -DJSONCPP_WITH_TESTS=0 -DJSONCPP_WITH_POST_BUILD_UNITTEST=0
$(package)_config_opts_i686_linux=-DCMAKE_SYSTEM_NAME=Linux -DCMAKE_SYSTEM_PROCESSOR=i686
$(package)_config_opts_x86_64_linux=-DCMAKE_SYSTEM_NAME=Linux -DCMAKE_SYSTEM_PROCESSOR=x86_64
$(package)_config_opts_darwin=-DCMAKE_SYSTEM_NAME=Darwin -DCMAKE_SYSTEM_PROCESSOR=arm
$(package)_config_opts_i686_mingw32=-DCMAKE_SYSTEM_NAME=Windows -DCMAKE_SYSTEM_PROCESSOR=i686
$(package)_config_opts_i686_mingw32+=-DCMAKE_C_COMPILER=i686-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++
$(package)_config_opts_x86_64_mingw32=-DCMAKE_SYSTEM_NAME=Windows -DCMAKE_SYSTEM_PROCESSOR=x86_64
$(package)_config_opts_x86_64_mingw32+=-DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++
#$(package)_config_opts_x86_64_mingw32+=-DCMAKE_AR=x86_64-w64-mingw32-ar -DCMAKE_RANLIB=x86_64-w64-mingw32-ranlib
$(package)_config_opts_arm_linux=-DCMAKE_SYSTEM_NAME=Linux -DCMAKE_SYSTEM_PROCESSOR=arm
endef

define $(package)_config_cmds
  mkdir -p $($(package)_staging_prefix_dir) && \
  patch -p1 < $($(package)_patch_dir)/$(package)-path.patch && \
  cmake $($(package)_config_opts) . 
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  cmake -P cmake_install.cmake
endef

define $(package)_postprocess_cmds
endef
