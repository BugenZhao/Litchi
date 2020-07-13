// For embed an user elf into the kernel binary
// See also: CMakeLists.txt, srcipts/kernel.ld
// https://stackoverflow.com/questions/327609/include-binary-file-with-gnu-ld-linker-script

.section .embuser
.incbin EMBUSER_ELF // macro provided by cmake