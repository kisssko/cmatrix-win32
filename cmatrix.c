
/**************************************************************************
 *   cmatrix.c                                                            *
 *                                                                        *
 *   Copyright (C) 1999 Chris Allegretta                                  *
 *   This program is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation; either version 1, or (at your option)  *
 *   any later version.                                                   *
 *                                                                        *
 *   This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 *   GNU General Public License for more details.                         *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program; if not, write to the Free Software          *
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.            *
 *                                                                        *
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "curses.h"

#define MEGABOLD 2

int COLS	= 100;
int LINES	= 30;

/* Matrix typedef */
typedef struct cmatrix {
    int val;
    int bold;
} cmatrix;

/* Global variables, unfortunately */
cmatrix **matrix = (cmatrix **) NULL;   /* The matrix has you */
int *length = NULL;			/* Length of cols in each line */
int *spaces = NULL;			/* spaces left to fill */
int *updates = NULL;			/* What does this do again? :) */

void *nmalloc(size_t howmuch)
{
    void *r;
    if (!(r = malloc(howmuch)))
	{
		puts("CMatrix: malloc: out of memory!");
		exit(1);
	}
    return r;
}

/* Initialize the global variables */
RETSIGTYPE var_init(void)
{
    int i, j;

    if (matrix != NULL)
	free(matrix);

    matrix = nmalloc(sizeof(cmatrix) * (LINES + 1));
    for (i = 0; i <= LINES; i++)
	matrix[i] = nmalloc(sizeof(cmatrix) * COLS);

    if (length != NULL)
	free(length);
    length = nmalloc(COLS * sizeof(int));

    if (spaces != NULL)
	free(spaces);
    spaces = nmalloc(COLS* sizeof(int));

    if (updates != NULL)
	free(updates);
    updates = nmalloc(COLS * sizeof(int));

    /* Make the matrix */
    for (i = 0; i <= LINES; i++)
	for (j = 0; j <= COLS - 1; j += 2)
	    matrix[i][j].val = -1;

    for (j = 0; j <= COLS - 1; j += 2) {
	/* Set up spaces[] array of how many spaces to skip */
	spaces[j] = (int) rand() % LINES + 1;

	/* And length of the stream */
	length[j] = (int) rand() % (LINES - 3) + 3;

	/* Sentinel value for creation of new objects */
	matrix[1][j].val = ' ';

	/* And set updates[] array for update speed. */
	updates[j] = (int) rand() % 3 + 1;
    }

}


int main(int argc, char **argv)

{
	int i, j = 0, count = 0, asynch = 0, bold = 1, y, z,
		firstcoldone = 0, random = 0, update = 4, highnum = 0,
		mcolor = COLOR_GREEN, randnum = 0, randmin = 0;

	volatile int keypress = 0;
	volatile int oldstyle = 0;

	srand(time(NULL));

	/* Set up values for random number generation */
	randnum = 93;
	randmin = 33;
	highnum = 123;

	var_init();

	while (!keypress)
	{
		count++;
		if (count > 4)
			count = 1;

		for (j = 0; j <= COLS - 1; j += 2)
		{
			if (count > updates[j] || asynch == 0)
			{

				/* I dont like old-style scrolling, yuck */
				if (oldstyle)
				{
					for (i = LINES - 1; i >= 1; i--)
						matrix[i][j].val = matrix[i - 1][j].val;

					random = (int)rand() % (randnum + 8) + randmin;

					if (matrix[1][j].val == 0)
						matrix[0][j].val = 1;
					else if (matrix[1][j].val == ' ' || matrix[1][j].val == -1)
					{
						if (spaces[j] > 0)
						{
							matrix[0][j].val = ' ';
							spaces[j] --;
						}
						else
						{

							/* Random number to determine whether head of next collumn
							   of chars has a white 'head' on it. */

							if (((int)rand() % 3) == 1)
								matrix[0][j].val = 0;
							else
								matrix[0][j].val = (int)rand() % randnum + randmin;

							spaces[j] = (int)rand() % LINES + 1;
						}
					}
					else if (random > highnum && matrix[1][j].val != 1)
						matrix[0][j].val = ' ';
					else
						matrix[0][j].val = (int)rand() % randnum + randmin;

				}
				else
				{	/* New style scrolling (default) */
					if (matrix[0][j].val == -1 && matrix[1][j].val == ' ' && spaces[j] > 0)
					{
						matrix[0][j].val = -1;
						spaces[j] --;
					}
					else if (matrix[0][j].val == -1 && matrix[1][j].val == ' ')
					{
						length[j] = (int)rand() % (LINES - 3) + 3;
						matrix[0][j].val = (int)rand() % randnum + randmin;

						if ((int)rand() % 2 == 1)
							matrix[0][j].bold = MEGABOLD;

						spaces[j] = (int)rand() % LINES + 1;
					}
					i = 0;
					y = 0;
					firstcoldone = 0;
					while (i <= LINES)
					{

						/* Skip over spaces */
						while (i <= LINES && (matrix[i][j].val == ' ' || matrix[i][j].val == -1))
							i++;

						if (i > LINES)
							break;

						/* Go to the head of this collumn */
						z = i;
						y = 0;
						while (i <= LINES && (matrix[i][j].val != ' ' && matrix[i][j].val != -1))
						{
							i++;
							y++;
						}

						if (i > LINES)
						{
							matrix[z][j].val = ' ';
							matrix[LINES][j].bold = 1;
							continue;
						}

						matrix[i][j].val = (int)rand() % randnum + randmin;

						if (matrix[i - 1][j].bold == 2)
						{
							matrix[i - 1][j].bold = 1;
							matrix[i][j].bold = MEGABOLD;
						}

						/* If we're at the top of the collumn and it's reached its
						 * full length (about to start moving down), we do this
						 * to get it moving.  This is also how we keep segments not
						 * already growing from growing accidentally =>
						 */
						if (y > length[j] || firstcoldone)
						{
							matrix[z][j].val = ' ';
							matrix[0][j].val = -1;
						}
						firstcoldone = 1;
						i++;
					}
				}
			}
			/* Hack =P */
			if (!oldstyle)
			{
				y = 1;
				z = LINES;
			}
			else
			{
				y = 0;
				z = LINES - 1;
			}
			for (i = y; i <= z; i++)
			{
				move(i - y, j);

				if (matrix[i][j].val == 0 || matrix[i][j].bold == 2)
				{
					fgcolor(COLOR_WHITE | (bold ? 8 : 0));
					if (matrix[i][j].val == 0)
					{
						addch('&');
					}
					else
						addch(matrix[i][j].val);
					fgcolor(mcolor);
				}
				else
				{
					fgcolor(mcolor | (bold ? 8 : 0));
					if (matrix[i][j].val == 1)
					{
						addch('|');
						fgcolor(mcolor | bold ? 8 : 0);
					}
					else
					{
						if (bold == 2 || (bold == 1 && matrix[i][j].val % 2 == 0))
							fgcolor(mcolor | 8);
						if (matrix[i][j].val == -1)
							addch(' ');
						else
							addch(matrix[i][j].val);
						if (bold == 2 || (bold == 1 && matrix[i][j].val % 2 == 0))
							fgcolor(mcolor);
					}
					fgcolor(mcolor);
				}
			}
		}
		napms(update * 2);
	}
}

