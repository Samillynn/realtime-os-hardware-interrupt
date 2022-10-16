ENTRY(_start)           /* ELF entry symbol */

SECTIONS {
  . = 0x80000;          /* load address */
	.text.boot : {        /* this section goes first - not strictly needed */
		KEEP(*(.text.boot))
	}
}
