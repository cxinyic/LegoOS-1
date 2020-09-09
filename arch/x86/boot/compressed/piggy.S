.section ".compressed_kernel","a",@progbits
.globl input_data, input_data_end
input_data:
.incbin "arch/x86/boot/compressed/vmImage.bin"
input_data_end:
