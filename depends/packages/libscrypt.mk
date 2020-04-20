package=libscrypt
$(package)_version=1.20
$(package)_download_path=https://github.com/technion/$(package)/archive
$(package)_file_name=v$($(package)_version).tar.gz
$(package)_sha256_hash=6074add2170b7d00e080fe3a58d3dec76850a4f272d488f5e8cc3c4acb6d8e21
$(package)_patches=$(package)-make.patch

define $(package)_set_vars
endef

define $(package)_config_cmds
  mkdir -p $($(package)_staging_prefix_dir) && \
  patch -p1 < $($(package)_patch_dir)/$(package)-make.patch
endef

define $(package)_build_cmds
  $(MAKE) CC="$($(package)_cc)" 
endef

define $(package)_stage_cmds
  $(MAKE) PREFIX=$($(package)_staging_prefix_dir) install install-static && \
  rm -fr $($(package)_staging_prefix_dir)/lib/$(package).so*
endef

define $(package)_postprocess_cmds
endef
