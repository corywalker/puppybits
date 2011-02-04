
SECTIONS
{
	. = 0x80008000;
	.text : {
	 	_stext = .;
	 	*(.text)
	 }
	.rodata : {
		*(.rodata)
		*(.rodata.*)
		_etext = .;
	}
	.data ALIGN(4096) : {
		_sdata = .;
		*(.data)
	}
	.bss : {
	       __bss_start__ = .;
		*(.bss)
		_edata = ALIGN(4096);
		__bss_end__ = _edata;
	}
/*	/DISCARD/ : {
	*(.comment)
	}*/
}

