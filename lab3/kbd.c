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

#include "keymap"
#define KCNTL 0x00
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK 0x0C
#define KISTA 0x10

typedef volatile struct kbd
{
  char *base;
  char buf[128];
  int head, tail, data, room;
  u8 is_shift; //new code
} KBD;

volatile KBD kbd;

int kbd_init()
{
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x14;
  *(kp->base + KCLK) = 8;
  kp->head = kp->tail = 0;
  kp->data = 0;
  kp->room = 128;
  kp->is_shift = 0;
}

void kbd_handler()
{
  u8 scode, c;
  KBD *kp = &kbd;
  color = YELLOW;
  scode = *(kp->base + KDATA);
  //if (scode & 0x80)
    //return;

  //new code
  u8 key_code = scode & ~0x80;

  //check for L-SHIFT and R-SHIFT
  if (key_code == 0x2A || 0x36 == key_code)
  {
    printf("shift pressed\n");
    kp->is_shift = (scode & 0x80) ? 0 : 1;
  }

  if (!(scode & 0x80)) //if a key got released exit interrupt
    return;

  if (kp->is_shift) //if shift is currently on
  {
    c = sh[key_code]; //print shifted symbols
  }
  else
  {
    c = unsh[key_code]; //else print unshifted
  }

  if (c >= ' ' && c <= '~') //allow symbols an nunmbers toos
    printf("kbd interrupt: c=%s %c\n", c, c);
  //end new code


  kp->buf[kp->head++] = c;
  kp->head %= 128;
  kp->data++;
  kp->room--;
}

int kgetc()
{
  char c;
  KBD *kp = &kbd;

  unlock();
  while (kp->data == 0)
    ;

  lock();
  c = kp->buf[kp->tail++];
  kp->tail %= 128;
  kp->data--;
  kp->room++;
  unlock();
  return c;
}

int kgets(char s[])
{
  char c;
  while ((c = kgetc()) != '\r')
  {
    *s++ = c;
  }
  *s = 0;
  return strlen(s);
}
