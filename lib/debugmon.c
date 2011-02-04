/*  debugmon.c: Very simple debug/crash monitor.

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

/*
  Very simple 'debug' monitor.

  It lets you examine memory after a crash.
*/

#include "pbits.h"

static unsigned int parsenum(char **p) {
	char *s = *p;
	unsigned int base = 10;

	while (*s == ' ')
		s++;

	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
		s += 2;
		base = 16;
	}

	*p = s;

	return dstrtoul(s, p, base);
}

void debugmon(void) {
	dprintf("Entering crappy crash monitor.\n ? for help.\n");
	while (1) {
		char cmd[128];
		char *p = cmd+1;
		int len;

		dprintf("#> ");
		len = dreadline(cmd, 128);
		switch (cmd[0]) {
		case '?':
			dprintf("?\t\thelp\n"
				"m addr len\tShow memory as words from addr for len words\n"
				);
			break;
		case 'm': {
			unsigned int *addr;
			int mlen;
			int i;

			addr = (unsigned int *)parsenum(&p);
			mlen = parsenum(&p);

			for (i=0;i<mlen;i++) {
				if ((i & 7) == 0)
					dprintf("\n0x%08x:", addr);
				dprintf(" 0x%08x", *addr);
				addr++;
			}
			dprintf("\n");
			break; }
		default:
			dprintf("%s: Unknown command or arguments\n", cmd);
			break;
		}
	}
}
