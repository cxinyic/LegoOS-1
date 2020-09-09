cmd_arch/x86/boot/compressed/vmImage.lds := gcc -E -Wp,-MD,arch/x86/boot/compressed/.vmImage.lds.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/4.8/include -I./arch/x86/include -I./arch/x86/include/generated -I./arch/x86/include/generated/uapi  -I./include -I./managers -I./managers -include ./include/lego/kconfig.h -D__KERNEL__    -P -C -Ux86 -D__ASSEMBLY__ -DLINKER_SCRIPT -o arch/x86/boot/compressed/vmImage.lds arch/x86/boot/compressed/vmImage.lds.S

source_arch/x86/boot/compressed/vmImage.lds := arch/x86/boot/compressed/vmImage.lds.S

deps_arch/x86/boot/compressed/vmImage.lds := \
    $(wildcard include/config/output/format.h) \
    $(wildcard include/config/x86/64.h) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/physical/start.h) \
  include/lego/const.h \
  include/lego/types.h \
    $(wildcard include/config/64bit.h) \
  arch/x86/include/asm/types.h \
  include/lego/compiler.h \
    $(wildcard include/config/smp.h) \
  arch/x86/include/asm/cache.h \
  include/lego/linkage.h \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/x86/32.h) \
    $(wildcard include/config/x86/alignment/16.h) \
  include/lego/stringify.h \
  arch/x86/include/asm/sparsemem.h \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/x86/pae.h) \

arch/x86/boot/compressed/vmImage.lds: $(deps_arch/x86/boot/compressed/vmImage.lds)

$(deps_arch/x86/boot/compressed/vmImage.lds):
