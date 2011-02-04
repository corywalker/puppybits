
// sleep approximately v miliseconds
// NB: this doesn't work if you have caching on ...
void ksleep(int v) {
	int i;

	// this is approximately 1ms, at least on beagleboard c3 w/ no cache on
	while (v--) {
		for (i=0;i<1000;i++) {
			asm volatile("mov r0, r0");
			asm volatile("mov r0, r0");
			asm volatile("mov r0, r0");
		}
	}
}

