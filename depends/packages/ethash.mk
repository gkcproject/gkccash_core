package=ethash
$(package)_version=0.4.1
$(package)_download_path=https://github.com/chfast/$(package)/archive
$(package)_file_name=v$($(package)_version).tar.gz
$(package)_sha256_hash=db70eb8549bbb57096f6e0e3ffcf7276683eb2c2eed66766149011deadaaa150

define $(package)_set_vars
$(package)_config_opts=-DCMAKE_INSTALL_PREFIX=$($(package)_staging_prefix_dir) -DETHASH_BUILD_TESTS=0
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
  cmake $($(package)_config_opts) .
endef

define $(package)_build_cmds
  $(MAKE) -C lib/$(package)/ 
endef

define $(package)_stage_cmds
  $(MAKE) -C lib/$(package)/ install && \
  rm -fr $($(package)_staging_prefix_dir)/lib/cmake
endef

define $(package)_postprocess_cmds
endef
