SECTIONS
{
	. = 0xf0000000;
	__executable_start = .;
	.init : {
		KEEP (*(.init))
	} = 0
	.text : {
	 	*(.text)
	}
	.rodata : {
		*(.rodata)
		*(.rodata.*)
		_etext = .;
	}
	/*. = 0x80000000; */
	.data ALIGN(4096) : {
		__data_start__ = .;
		*(.data)
	}
	__bss_start__ = .;
	.bss : {
		*(.dynbss)
	       	*(.bss .bss.* .gnu.linkonce.b.*)
		*(COMMON)
	}
	__bss_end__ = ALIGN(4096);
/*	/DISCARD/ : {*/
/*		*(.data)*/
/* 	*(.init)		*/
/*	}*/
}
