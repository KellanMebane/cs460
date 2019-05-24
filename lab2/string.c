typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

int mystrcmp(char *s1, char *s2, u8 size) // a string compare that doesn't need null termination or direction
{
    u8 i = 0;

    for (i = 0; i < size; i++)
        if (*s1 != *s2) // if not equal return non zero
            return 1;

    if (s1[size] != 0) // if s1 is bigger than string two return non zero
        return 1;

    return 0; // return equal
}

int mystrlen(char *s)
{
    int len = 0;

    while (*s)
    {
        s++;
        len++;
    }

    return len;
}