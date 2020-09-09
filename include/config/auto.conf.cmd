deps_config := \
	net/Kconfig \
	managers/memory/Kconfig \
	managers/processor/checkpoint/Kconfig \
	managers/processor/fs/Kconfig \
	managers/processor/pcache/Kconfig \
	managers/processor/Kconfig \
	managers/Kconfig \
	drivers/eth/Kconfig \
	drivers/infiniband/ulp/ipoib/Kconfig \
	drivers/infiniband/hw/mlx4/Kconfig \
	drivers/infiniband/Kconfig \
	drivers/tty/Kconfig \
	drivers/Kconfig \
	mm/Kconfig \
	kernel/Kconfig.preempt \
	kernel/Kconfig.hz \
	kernel/locking/Kconfig \
	init/Kconfig \
	arch/x86/Kconfig \
	Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(KERNELVERSION)" "4.0.0"
include/config/auto.conf: FORCE
endif
ifneq "$(ARCH)" "x86"
include/config/auto.conf: FORCE
endif
ifneq "$(SRCARCH)" "x86"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
