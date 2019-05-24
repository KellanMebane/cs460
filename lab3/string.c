// YOUR string.c file for
//strcmp(), strlen(), strncmp(), etc
//#include <stdio.h>
#ifndef STRING_C_
#define STRING_C_

void *memcpy(void *dest, void *source, int size)
{
    char *cdest = (char *)dest;
    char *csource = (char *)source;
    int i = 0;
    for (i = 0; i < size; i++)
        cdest[i] = csource[i];

    return (void*)cdest;
}

int strcpy(char *dest, char *source)
{
    int i = 0;
    while (*dest && *source)
    {
        *dest++ = *source++;
        i++;
    }
    return i;
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

int strlen(char *s)
{
    int i = 0;
    while (*s++)
        i++;
    return i;
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

#endif