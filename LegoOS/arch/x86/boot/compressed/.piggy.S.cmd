cmd_arch/x86/boot/compressed/piggy.S := arch/x86/boot/compressed/mkpiggy arch/x86/boot/compressed/vmImage.bin > arch/x86/boot/compressed/piggy.S || ( rm -f arch/x86/boot/compressed/piggy.S ; false )
