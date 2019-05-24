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

int body(), goUmode();

char *disk = (char *)67108864;

int get_block(u32 blk, char *buf) // read from a 1KB block
{
  char *cp = disk + blk * BLKSIZE;
  memcpy(buf, cp, BLKSIZE);
}

int put_block(u32 blk, char *buf) // write to a 1KB block
{
  char *cp = disk + blk * BLKSIZE;
  memcpy(cp, buf, BLKSIZE);
}

u32 search(char *s, INODE *ip)
{
  u8 i;                   // small iterator
  DIR *dp;                // dir pointer
  char *cp, buf[BLKSIZE]; // char block and pointer to iterate a block
  char temp[64];

  for (i = 0; i < 12; i++) // search each direct inode
  {
    if (ip->i_block[i] == 0) // break when you run out of allocated inodes
      break;

    get_block((u16)ip->i_block[i], buf); // get a dircct block
    dp = (DIR *)buf;
    cp = buf;

    while (cp < buf + BLKSIZE) // leapfrog iterate through the dir block
    {
      memcpy(temp, dp->name, dp->name_len);
      temp[dp->name_len] = 0;

      if (strcmp(temp, s) == 0) // is name equal to child dir?
      {
        // yes grab it and return it's inode number
        return dp->inode;
      }

      // no, jump to next record
      cp += dp->rec_len;
      dp = (DIR *)cp;
    }
  }

  return 0;
}

PROC *kfork(char *filename)
{
  int i, j = 0;
  char buffer[BLKSIZE];
  char buffer2[BLKSIZE];
  SUPER *host = 0;
  INODE *inoTemp = 0;
  GD *gdTemp = 0;
  int bin = 0;
  DIR *dp = 0;
  int looker = 0;
  int inodeStart = 0;
  char *cp, *cq;
  char *addr;
  char line[8];
  int usize1, usize;
  int usize2, usize3, usize4;
  int *ustacktop, *usp;
  u32 BA, Btop, Busp;

  PROC *p = dequeue(&freeList);
  if (p == 0)
  {
    kprintf("kfork failed\n");
    return (PROC *)0;
  }
  p->ppid = running->pid;
  p->parent = running;
  p->parent = running;
  p->status = READY;
  p->priority = 1;

  // set kstack to resume to body
  for (i = 1; i < 29; i++) // all 28 cells = 0
    p->kstack[SSIZE - i] = 0;

  p->kstack[SSIZE - 15] = (int)goUmode; // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE - 28]);


  // must load filename to Umode image area at 7MB+(pid-1)*1MB
  addr = (char *)(0x800000 + (p->pid - 1) * 0x100000);

  // loadelf(filename, p); // p->PROC containing pid, pgdir, etc
  /*if (strcmp(filename, "u1") == 0)
  {
    usize1 = (int)&_binary_u1_size;
    cp = &_binary_u1_start;
    usize = &_binary_u1_end - &_binary_u1_start;
    printf("loading u1 %x to %x\n", cp, addr);
  }

  if (strcmp(filename, "u2") == 0)
  {
    usize2 = (int)&_binary_u2_size;
    cp = &_binary_u2_start;
    usize = &_binary_u2_end - &_binary_u2_start;
    printf("loading u2 %x to %x\n", cp, addr);
  }
  
  if (strcmp(filename, "u3") == 0)
  {
    usize3 = (int)&_binary_u3_size;
    cp = &_binary_u3_start;
    usize = &_binary_u3_end - &_binary_u3_start;
    printf("loading u3 %x to %x\n", cp, addr);
  }

  if (strcmp(filename, "u4") == 0)
  {
    usize4 = (int)&_binary_u4_size;
    cp = &_binary_u4_start;
    usize = &_binary_u4_end - &_binary_u4_start;
    printf("loading u4 %x to %x\n", cp, addr);
  }*/

  //find the inode location of
  get_block(1, buffer);   //get super block
  host = (SUPER *)buffer; //typecast as super
  get_block(2, buffer);   //get group descriptor
  gdTemp = (GD *)buffer;
  get_block(gdTemp->bg_inode_table, buffer); //grab inode table
  inoTemp = (INODE *)buffer;
  inoTemp++; // this is root
  //printf("inode shit: %x\n", inoTemp->i_mode);
  bin = search("bin", inoTemp); //search bin
  //printf("bin is in inode: %d\n", bin);
  //(bin - 1) / 8 + 8
  get_block(((bin - 1) / 8) + 8, buffer); //get bin block
  inoTemp = (INODE *)buffer;
  inoTemp += (bin - 1) % 8; //pointer to bin

  bin = search(filename, inoTemp);        //user mode file (assume it's in bin for now) inode number

  printf("file found in: %d\n", bin);
  get_block(((bin - 1) / 8) + 8, buffer); //get user file block
  inoTemp = (INODE *)buffer;
  inoTemp += (bin - 1) % 8; //pointer to user file inode

  usize = inoTemp->i_size;

  printf("FILE SIZE IS: %d\n", usize);

  cq = addr;

  i = 0;

  while (usize > 0)
  {
    get_block(inoTemp->i_block[i++], buffer2);
    printf("I block number: %d\n", inoTemp->i_block[i - 1]);
    cp = (char *)buffer2;
    j =0;

    while (j < BLKSIZE && j < usize)
    { 
      *cq++ = *cp++;
      j++;      
    }

    //printf("")
    usize -= BLKSIZE;
  }

  //p->usp = (int *)(0x80100000);
  p->usp = (int *)VA(0x100000);

  //  p->kstack[SSIZE-1] = (int)0x80000000;
  p->kstack[SSIZE - 1] = VA(0);
  // -|-----goUmode-------------------------------------------------
  //  r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 ufp uip upc|string       |
  //----------------------------------------------------------------
  //  14 13 12 11 10 9  8  7  6  5  4   3    2   1 |             |

  enqueue(&readyQueue, p);

  kprintf("proc %d kforked a child %d: ", running->pid, p->pid);
  printQ(readyQueue);

  return p;
}
