cmd_arch/x86/boot/compressed/vmImage.bin := objcopy  -O binary -R .comment -R .note -S vmImage arch/x86/boot/compressed/vmImage.bin
