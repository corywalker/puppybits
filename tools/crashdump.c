/*  crashdump.c: Crash analyser for objdump files

    Copyright (C) 2010 Michael Zucchi

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*
  take a stack dump and look it up in a disassembly from objdump

  how to use:

  Take the final elf image of your system, and run objdump -d on it.

  objdump -d kernel.elf > kernel.disasm

  Then use crashdump to look up the code by address(es):

  crashdump -3 kernel.disasm 0xabadf00d 0xdeadbeef

  Where the addresses likely come from a stack dump on the target
  host.

 */

int showlines;

char *assembly;
char *assembly_end;
char **lines;
int line_count;

void loadassembly(const char *name) {
	struct stat st;
	FILE *fp;

	if (stat(name, &st) != 0) {
		perror("opening assembly file");
		exit(1);
	}

	assembly = malloc(st.st_size+1);
	fp = fopen(name, "r");
	fread(assembly, 1, st.st_size, fp);
	fclose(fp);
	assembly_end = assembly + st.st_size;
	// set sentinal
	assembly_end[0] = '\n';
}

/*

80008010 <_start_common>:
80008010:       e5cf404c        strb    r4, [pc, #76]   ; 80008064 <gUBootOS>
80008014:       e58f8040        str     r8, [pc, #64]   ; 8000805c <gUBootGlobalData>
*/

// build index of lines based on starting address.
// lines of the form `x{8}:' are the starting address
void scanassembly(void) {
	char *p = assembly;
	int nlines;

	// count lines that start with an address
	while (p<assembly_end) {
		char *s = p;

		while (*p++ != '\n');

		if (p-s > 8 && s[8] == ':')
			nlines += 1;
	}

	lines = malloc(nlines * sizeof(*lines));
	line_count = nlines;

	// assign lines
	p = assembly;
	nlines = 0;
	while (p<assembly_end) {
		char *s = p;

		while (*p++ != '\n');

		if (p-s > 8 && s[8] == ':')
			lines[nlines++] = s;
	}

	// no need to sort - already sorted
}

int cmp_addr(const void *keyp, const void *ap) {
	const char *key = keyp;
	char * const *a = ap;

	return strncmp(key, *a, 8);
}

void probeaddress(const char *addr) {
	char **res = bsearch(addr, lines, line_count, sizeof(*lines), cmp_addr);

	if (res == NULL)
		return;

	char *at = *res;
	char *p = *res;

	// scan backard looking for \n\n (or start)
	while (p > assembly+2) {
		if (p[-1] == '\n' && p[-2] == '\n')
			break;
		p--;
	}

	char *e = p;
	while (*e++ != 10);

	printf("%.*s", e-p, p);

	if (showlines > 0) {
		// go back 'n' lines from 'at'
		char **lp = res-showlines < &lines[0] ? &lines[0] : res-showlines;
		char *detail;

		if (lp[0] > e)
			printf("                ...\n");
		detail = lp[0] < e ? e : lp[0];

		printf("%.*s", at-detail, detail);
		printf("                ...\n");
	}
}

int main(int argc, char **argv)
{
	int i = 1;

	if (argc<2) {
		printf("usage: %s [ -linecount ] disassembly [ addresses ... ]\n");
		return 1;
	}

	if (argv[i][0] == '-') {
		showlines = atoi(argv[i]+1);
		i++;
	}

	loadassembly(argv[i++]);
	scanassembly();

	for (;i<argc;i++) {
		char *arg = argv[i];

		if (arg[0] == '0' && arg[1] == 'x')
			arg+=2;

		probeaddress(arg);
	}
}
