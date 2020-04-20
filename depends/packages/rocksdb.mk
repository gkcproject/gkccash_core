package=rocksdb
$(package)_version=5.18.3
$(package)_download_path=https://github.com/facebook/$(package)/archive
$(package)_file_name=v$($(package)_version).tar.gz
$(package)_sha256_hash=7fb6738263d3f2b360d7468cf2ebe333f3109f3ba1ff80115abd145d75287254
$(package)_patches=$(package)-libs.patch

define $(package)_set_vars
$(package)_config_opts=-DCMAKE_INSTALL_PREFIX=$($(package)_staging_prefix_dir) -DWITH_TOOLS=0 -DWITH_TESTS=0 -DROCKSDB_INSTALL_ON_WINDOWS=1
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
  patch -p1 < $($(package)_patch_dir)/$(package)-libs.patch && \
  cmake $($(package)_config_opts) .
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) install && \
  rm -fr $($(package)_staging_prefix_dir)/lib/cmake/ $($(package)_staging_prefix_dir)/lib/lib$(package).so*
endef

define $(package)_postprocess_cmds
endef
