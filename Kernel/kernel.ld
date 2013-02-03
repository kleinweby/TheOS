OUTPUT_FORMAT(elf32-i386)
/**/

ENTRY(BootstrapASMEntry);

SECTIONS
{
   . = 0x100000;

   .mboot : ALIGN(4) {
      /* Multiboot header */
      LONG(0x1BADB002); /* Magic */
      LONG(1 << 1 | 1 << 0); /* Flags */
      LONG(-((1 << 1 | 1 << 0) + 0x1BADB002)); /* Checksum */
   }
      
   PROVIDE_HIDDEN(_KernelBootstrapOffset = .);
   .bootstrap ALIGN(4096) : {
      *(.bootstrap)
   }
   PROVIDE_HIDDEN(_KernelBootstrapLength = . - _KernelBootstrapOffset);

   . = ALIGN(0x1000);
   PROVIDE_HIDDEN(_KernelOffset = .);
   . += 0xC0000000;

   PROVIDE_HIDDEN(_KernelTextOffset = .);
   .text : AT(ADDR(.text) - 0xC0000000) {
      *(.text)
   }
   PROVIDE_HIDDEN(_KernelTextLength = (. - 0xC0000000) - _KernelTextOffset);
   
   PROVIDE_HIDDEN(_KernelDataOffset = .);
   .data ALIGN(4096) : AT(ADDR(.data) - 0xC0000000) {
      *(.data)
   }
   .rodata ALIGN(4096) : AT(ADDR(.rodata) - 0xC0000000) {
      *(.rodata)
   }
   .bss ALIGN(4096) :  AT(ADDR(.bss) - 0xC0000000) {
     *(.bss)
   }
   
   /* Support for staticly declaring logging drivers */
   .LoggingDrivers : {
      KEEP(*(.LoggingDrivers.*))
      KEEP(*(.LoggingDrivers))
   }
   PROVIDE_HIDDEN(_LoggingDrivers = ADDR(.LoggingDrivers));
   PROVIDE_HIDDEN(_LoggingDriversLength = SIZEOF(.LoggingDrivers));
   
   /* Support for staticly declaring panic drivers */
   .PanicDrivers : {
      KEEP(*(.PanicDrivers.*))
      KEEP(*(.PanicDrivers))
   }
   PROVIDE_HIDDEN(_PanicDrivers = ADDR(.PanicDrivers));
   PROVIDE_HIDDEN(_PanicDriversLength = SIZEOF(.PanicDrivers));
   
   PROVIDE_HIDDEN(_KernelDataLength = (. - 0xC0000000) - _KernelDataOffset);
   PROVIDE_HIDDEN(_KernelLength = (. - 0xC0000000) - _KernelOffset);
   
   /* for debug */
   .debug_abbrev 0 : { *(.debug_abbrev) }
   .debug_info 0 : { *(.debug_info) }
   .debug_line 0 : { *(.debug_line) }
   .debug_loc 0 : { *(.debug_loc) }
   .debug_pubnames 0 : { *(.debug_pubnames) }
   .debug_aranges 0 : { *(.debug_aranges) }
   .debug_ranges 0 : { *(.debug_ranges) }
   .debug_str 0 : { *(.debug_str) }
   .debug_frame 0 : { *(.debug_frame) }
}
