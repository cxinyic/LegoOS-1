cmd_arch/x86/boot/vmImage.bin := objcopy  -O binary -R .note -R .comment -S arch/x86/boot/compressed/vmImage arch/x86/boot/vmImage.bin
