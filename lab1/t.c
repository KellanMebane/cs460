/************** t.c file **************************/

#include "ext.h"

#define BLK 1024

int gets(char *s)
{
    while ((*s = getc()) != '\r') // add characters except enter key
        putc(*s++);
    *s = 0; // null terminate
}

int prints(char *s)
{
    while (*s)      // while character isn't null
        putc(*s++); // print character and move to the next
}

int getblk(u16 blk, char *buf)
{
    readfd(blk / 18, (blk / 9) % 2, (blk * 2) % 18, buf); // track, cylinder, block calculations. get's a block of memory 1024 bytes.
}

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

INODE *search(char *s, INODE *ip, u16 iblock)
{
    u8 i;               // small iterator
    DIR *dp;            // dir pointer
    char *cp, buf[BLK]; // char block and pointer to iterate a block

    for (i = 0; i < 12; i++) // search each direct inode
    {
        if (ip->i_block[i] == 0) // break when you run out of allocated inodes
            break;

        getblk((u16)ip->i_block[i], buf); // get a dircct block
        dp = (DIR *)buf;
        cp = buf;

        while (cp < buf + BLK) // leapfrog iterate through the dir block
        {
            if (!mystrcmp(s, dp->name, dp->name_len)) // is name equal to child dir?
            {
                // yes grab it and return it's inode;
                i = dp->inode - 1;
                getblk((i / 8) + iblock, buf);
                return (INODE *)buf + (i % 8);
            }

            // no, jump to next record
            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
    }

    return -1; // error: -1 isn't a pointable location
}

main()
{
    GD *gp;                  // group descripter pointer
    INODE *ip;               // inode pointer
    DIR *dp;                 // directory pointer
    u16 i, iblk;             // ints that hold block number
    u32 *indirect;           // indirect block pointer (indirects are 2 blocks in size, so 4B pointer)
    u8 buf1[BLK], buf2[BLK]; // 1024 byte blocks
    char bootfile[64];       // filename of disk being booted

    getblk(2, buf1);           // get group descriptor block
    gp = (GD *)buf1;           // point at it
    iblk = gp->bg_inode_table; // grab the inode table
    getblk(iblk, buf1);        // get inode block start
    ip = (INODE *)buf1 + 1;    // get root (inode 2)

    prints("KMBoot: "); // ask for the file to boot
    gets(bootfile);   // get the input

    ip = search("boot", ip, iblk); // grab boot dir first
    if (ip == -1)
        error();

    if (bootfile[0] != 0)
    {
        ip = search(bootfile, ip, iblk); // now grab input file from under boot
        if (ip == -1)
            error();
    }
    else
    {
        ip = search("mtx", ip, iblk); // now grab input file from under boot
        if (ip == -1)
            error();
    }

    i = ip->i_block[12]; // grab indirect block locations
    getblk(i, buf2);
    indirect = buf2; // indirect blocks point to two blocks each, indirect pointer should be twice the size

    setes(0x1000); // jump extra segment to 4KB
    // start loading inode blocks into memory
    // grab each direct block and increment extra segment
    for (i = 0; i < 12; i++)
    {
        getblk(ip->i_block[i], 0);
        inces();
    }
    // grab each indirect block and increment extra segment
    while (*indirect != 0)
    {
        getblk(*indirect++, 0);
        inces();
    }
}