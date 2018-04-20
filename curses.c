
#include "curses.h"
#include <stdio.h>

int addch(int ct)
{
	int r;
	r=_putch((int)ct);
	_putch(8);
	return r;
}

int fgcolor(int a)
{
	_textcolor(a);
	return 0;
}




