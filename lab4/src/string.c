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
// write YOUR OWN
//strlen(), strcmp(), strcpy(), memcpy(), geti()

int strlen(char *s);
int strcmp(char *s1, char *s2);
char *strcpy(char *dest, char *src);
void *memcpy(void *dest, void *source, int size);

char *strcpy(char *dest, char *src)
{
	while (*src)
		*dest++ = *src++;

	return dest;
}

int strlen(char *s)
{
	int i = 0;
	while (*s++)
		i++;

	return i;
}

void *memcpy(void *dest, void *source, int size)
{
    char *cdest = (char *)dest;
    char *csource = (char *)source;
    int i = 0;
    for (i = 0; i < size; i++)
        cdest[i] = csource[i];

    return (void*)cdest;
}

int strcmp(char *s1, char *s2)
{
    while ((*s1 == *s2) && *s1)
    {
        s1++;
        s2++;
    }

    if (*s1 < *s2) //other way since numbers and symbols come first
        return 1;
    else if (*s1 == *s2)
        return 0;
    else
        return -1;
}

int strncmp(char *s1, char *s2, int len)
{
    int i = 0;

    while ((*s1 == *s2) && *s1)
    {
        s1++;
        s2++;
        i++;
        if (i >= len)
            break;
    }

    if (*s1 < *s2) //other way since numbers and symbols come first
        return 1;
    else if (*s1 == *s2)
        return 0;
    else
        return -1;
}