OUTPUT_FORMAT(elf32-i386)
/**/

ENTRY(startup);

SECTIONS
{
   . = 0x100000;

   .mboot : ALIGN(4) {
      /* Multiboot header */
      LONG(0x1BADB002); /* Magic */
      LONG(1 << 1 | 1 << 0); /* Flags */
      LONG(-((1 << 1 | 1 << 0) + 0x1BADB002)); /* Checksum */
   }
   
   kernel_start_symbol = .;
   
   _kernel_startup_begin = . - 0xC0000000;
   .startup ALIGN(4096) : {
      *(.startup)
   }
   _kernel_startup_end = .;

   . += 0xC0000000;
   _kernel_text_start = .;
   .text : AT(ADDR(.text) - 0xC0000000) {
      *(.text)
   }
   _kernel_text_end = .;

   _kernel_data_start = .;
   .data ALIGN(4096) : AT(ADDR(.data) - 0xC0000000) {
      *(.data)
   }
   .rodata ALIGN(4096) : AT(ADDR(.rodata) - 0xC0000000) {
      *(.rodata)
   }
   .bss ALIGN(4096) :  AT(ADDR(.bss) - 0xC0000000) {
     *(.bss)
   }

   /* Support for staticly declaring logging providers */
   .LoggingProviders : {
      KEEP(*(.LoggingProviders.*))
      KEEP(*(.LoggingProviders))
   }
   PROVIDE_HIDDEN(LoggingProvidersArrayStart = ADDR(.LoggingProviders));
   PROVIDE_HIDDEN(LoggingProvidersArraySize = SIZEOF(.LoggingProviders));


   _kernel_data_end = .;
   
   kernel_end_symbol = ADDR(.bss) + SIZEOF(.bss) - 0xC0000000;
   
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
