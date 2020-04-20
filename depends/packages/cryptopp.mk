package=cryptopp
$(package)_version=5_6_5
$(package)_download_path=https://github.com/weidai11/$(package)/archive
$(package)_file_name=CRYPTOPP_$($(package)_version).tar.gz
$(package)_sha256_hash=79fd5514b3b191a1c6d934cd989d5e058f4726a72a3dad2444bd1274a6aae686

define $(package)_set_vars
$(package)_config_opts=-DCMAKE_INSTALL_PREFIX=$($(package)_staging_prefix_dir) -DBUILD_STATIC=1 -DBUILD_SHARED=0 -DBUILD_TESTING=0
$(package)_config_opts_i686_linux=-DCMAKE_SYSTEM_NAME=Linux -DCMAKE_SYSTEM_PROCESSOR=i686
$(package)_config_opts_x86_64_linux=-DCMAKE_SYSTEM_NAME=Linux -DCMAKE_SYSTEM_PROCESSOR=x86_64
$(package)_config_opts_darwin=-DCMAKE_SYSTEM_NAME=Darwin -DCMAKE_SYSTEM_PROCESSOR=arm
$(package)_config_opts_i686_mingw32=-DCMAKE_SYSTEM_NAME=Windows -DCMAKE_SYSTEM_PROCESSOR=i686
$(package)_config_opts_x86_64_mingw32=-DCMAKE_SYSTEM_NAME=Windows -DCMAKE_SYSTEM_PROCESSOR=x86_64
$(package)_config_opts_x86_64_mingw32+=-DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++
#$(package)_config_opts_x86_64_mingw32+=-DCMAKE_AR=x86_64-w64-mingw32-ar -DCMAKE_RANLIB=x86_64-w64-mingw32-ranlib
$(package)_config_opts_arm_linux=-DCMAKE_SYSTEM_NAME=Linux -DCMAKE_SYSTEM_PROCESSOR=arm
endef

define $(package)_config_cmds
  mkdir -p $($(package)_staging_prefix_dir) && \
  cmake $($(package)_config_opts)  .
endef

define $(package)_build_cmds
  $(MAKE) -f Makefile
endef

define $(package)_stage_cmds
  $(MAKE) -f Makefile install && \
  rm -fr $($(package)_staging_prefix_dir)/lib/cmake/
endef

define $(package)_postprocess_cmds
endef
