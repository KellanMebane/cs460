/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
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
********************************************************************/

#define DR 0x00
#define FR 0x18

#define RXFE 0x10
#define TXFF 0x20

#define BASE 10
#define HEX 16
char *table = "0123456789ABCDEF";

typedef struct uart
{
    char *base;
    int n;
} UART;

UART uart[4];

int uart_init()
{
    int i;
    UART *up;

    for (i = 0; i < 4; i++)
    {
        up = &uart[i];
        up->base = (char *)(0x10009000 + i * 0x1000);
        up->n = i;
    }
    //uart[3].base = (char *)(0x10009000); // uart3 at 0x10009000
}

int ugetc(UART *up)
{
    while (*(up->base + FR) & RXFE)
        ;
    return *(up->base + DR);
}

int uputc(UART *up, char c)
{
    while (*(up->base + FR) & TXFF)
        ;
    *(up->base + DR) = c;
}

int ugets(UART *up, char *s)
{
    while ((*s = (char)ugetc(up)) != '\r')
    {
        uputc(up, *s);
        s++;
    }
    *s = 0;
}

int uprints(UART *up, char *s)
{
    while (*s)
        uputc(up, *s++);
}

int urpu(UART *up, unsigned long x)
{
    char c;
    if (x)
    {
        c = table[x % BASE];
        urpu(up, x / BASE);
        uputc(up, c);
    }
}

int urpd(UART *up, int x)
{
    char c;
    if (x)
    {
        c = table[x % BASE];
        urpu(up, x / BASE);
        uputc(up, c);
    }
}

int uprintd(UART *up, int x)
{
    if (x == 0)
        uputc(up, '0');
    else if (x < 0)
    {
        uputc(up, '-');
        urpd(up, -x);
    }
    else
        urpd(up, x);
    //uputc(up, ' ');
}

int urpx(UART *up, unsigned long x)
{
    char c;
    if (x)
    {
        c = table[x % HEX];
        urpx(up, x / HEX);
        uputc(up, c);
    }
}

int uprintx(UART *up, unsigned long x)
{
    uputc(up, '0');
    uputc(up, 'x');
    if (x == 0)
        uputc(up, '0');
    else
        urpx(up, x);
    uputc(up, ' ');
}

int uprintf(UART *up, char *fmt, ...)
{
    char *cp = fmt;
    int *ip = (int *)&fmt + 1;
    while (*cp)
    {
        if (*cp != '%')
        {
            uputc(up, *cp);
            if (*cp == '\n')
            {
                uputc(up, '\n');
                uputc(up, '\r');
            }
            cp++;
            continue;
        }
        cp++;
        switch (*cp)
        {
        case 'c':
            uputc(up, *ip);
            break;
        case 's':
            uprints(up, *ip);
            break;
        case 'u':
            //printu(*ip);
            break;
        case 'd':
            uprintd(up, *ip);
            break;
        case 'x':
            uprintx(up, *ip);
            break;
        }
        cp++;
        ip++;
    }
}