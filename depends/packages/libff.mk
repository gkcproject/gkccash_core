package=libff
$(package)_version=f2067162520f91438b44e71a2cab2362f1c3cab4
$(package)_download_path=https://github.com/scipr-lab/$(package)/archive
$(package)_file_name=$($(package)_version).tar.gz
$(package)_sha256_hash=1ff3f8f3b9b4e8ce9e58364aeb15ded88c3514562286a162478fbb9b2553243d
$(package)_dependencies=openssl gmp

define $(package)_set_vars
$(package)_config_opts=-DCMAKE_INSTALL_PREFIX=$($(package)_staging_prefix_dir) -DOPENSSL_ROOT_DIR=$(host_prefix) 
$(package)_config_opts+=-DGMP_INCLUDE_DIR=$(host_prefix)/include -DGMP_LIBRARY=$(host_prefix)/lib 
$(package)_config_opts+=-DCURVE=ALT_BN128 -DWITH_PROCPS=0 -DUSE_ASM=0 -DIS_LIBFF_PARENT=0
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
  cmake $($(package)_config_opts)  .
endef

define $(package)_build_cmds
  $(MAKE) 
endef

define $(package)_stage_cmds
  $(MAKE) install && \
  rm -fr $($(package)_staging_prefix_dir)/include/$(package)/CMakeFiles
endef

define $(package)_postprocess_cmds
endef
