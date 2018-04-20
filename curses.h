
#include <windows.h>
#include <conio.h>


#define RETSIGTYPE void

#define TRUE	1
#define FALSE	0


/* colors */
#define COLOR_BLACK		0
#define COLOR_RED		1
#define COLOR_GREEN		2
#define COLOR_YELLOW	3
#define COLOR_BLUE		4
#define COLOR_MAGENTA	5
#define COLOR_CYAN		6
#define COLOR_WHITE		7


#define napms(DLY) Sleep((DWORD)(DLY))
#define move(Y,X) gotoxy((X)-1,(Y)+1)

int addch(int);
int fgcolor(int);


