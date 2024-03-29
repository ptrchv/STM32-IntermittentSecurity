/* ...... */

/* Memories definition */
MEMORY
{
  RAM	(xrw)	 : ORIGIN = 0x30000000,	LENGTH = 256K    
  SRAM4	(xrw)    : ORIGIN = 0x38000000,	LENGTH = 16K
  FLASH	(rx)	 : ORIGIN = 0x0C000000,	LENGTH = 760K
  FLASH_CKP (rw) : ORIGIN = 0x0C0BE000,	LENGTH = 256K
  FLASH_NSC (rx) : ORIGIN = 0x0C0FE000,	LENGTH = 8K
}

/* Sections */
SECTIONS
{
  /*
  .isr_vector
  .text
  .rodata
  .ARM.extab
  .ARM
  .preinit_array
  .init_array
  .fini_array
  ...
  */

  .data :
  {
    _sdata = .;
    ./Drivers*(.data)
    ./Drivers*(.data*)
    *(.data.driver)
    _sdata_conf = .;
    . = . + (_sdata_conf - _sdata);
    *(.data.confidential)
    _edata_conf = .;
    *(.data)
    *(.data*)
    *(.RamFunc)
    *(.RamFunc*)
    _edata = .;
  } >RAM AT> FLASH

  /* .gnu.sgstubs */

  .bss :
  {
    _sbss = .;
    __bss_start__ = _sbss;
    ./Drivers*(.bss)
    ./Drivers*(.bss*)
    *(.bss.driver)
    _sbss_conf = .;
    . = . + (_sbss_conf - _sbss);
    *(.bss.confidential)
    _ebss_conf = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    _ebss = .;
    __bss_end__ = _ebss;
  } >RAM  
  
  .crypobuf :
  {
  	. = ALIGN(4);
  	_scryptobuf = .;
  	. = . + (_edata_conf - _sdata_conf);
  	_ecryptobuf_data = .;
  	. = . + (_ebss_conf - _sbss_conf);
  	. = ALIGN(4);
  	_ecryptobuf = .;
  } >RAM

  /*
  ._user_heap_stack
  DISCARD
  .ARM.attributes 0
  */
}
