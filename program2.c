#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

typedef unsigned char byte;

//Have a pagesize
#define PAGESIZE 4096 //or 2048, or 4096 …

typedef struct chunkhead
{
unsigned int size;
unsigned int info;
unsigned char *next,*prev;
} chunkhead;

void *startofheap = NULL;

chunkhead* get_last_chunk() //you can change it when you aim for performance
{
    if(!startofheap) //I have a global void *startofheap = NULL;
    return NULL;
    chunkhead* ch = (chunkhead*)startofheap;
    for (; ch->next; ch = (chunkhead*)ch->next);
    return ch;
}

unsigned char* mymalloc(unsigned int size) // function's return is void
{
    unsigned int chunks_num = ( (size + sizeof(chunkhead)) / PAGESIZE) 
        + ( ( (size + sizeof(chunkhead))  % PAGESIZE) != 0);
    unsigned int mem_to_alloc = chunks_num * PAGESIZE;
    
    //printf("C_head size: %i\n", sizeof(chunkhead));
    //printf("M to alloc: %i\n", mem_to_alloc);

    if(startofheap == NULL) 
    {
        printf("\n");
        chunkhead* ch = (chunkhead *)sbrk(sizeof(chunkhead));

        //printf("ch: %x", (int)ch);

        startofheap = ch;

        ch->info = 1;
        ch->prev = ch->next = NULL;
        ch->size = mem_to_alloc - sizeof(chunkhead);
        void* data_p = sbrk(ch->size);

        startofheap = ch;
       
        return (unsigned char*)data_p; 
    }
    
    
    for (chunkhead* ch = startofheap; ch != NULL; ch = (chunkhead*)ch->next)
    {
        //implement "BEST FIT" MECHANISM
        if (ch->info == 0 && ch->size >= mem_to_alloc - sizeof(chunkhead))
        {

            ch->info = 1;
            if ( ch->size - (mem_to_alloc - sizeof(chunkhead)) >= PAGESIZE )
            {
                chunkhead* new_ch = (chunkhead*)((unsigned char*)ch + mem_to_alloc);
                new_ch->info = 0;

                new_ch->size = ch->size - (mem_to_alloc - sizeof(chunkhead));
                ch->size = ch->size - new_ch->size - sizeof(chunkhead);

                ch->next = (unsigned char*)new_ch;
                new_ch->prev = (unsigned char*)ch;
                new_ch->next = (unsigned char*)ch->next;               
            }
            return (unsigned char*)ch + sizeof(chunkhead);
        }  
    }

    //IF NO CHUNKS WITH ENOUGH FREE SPACE EXISTS, CREATE A NEW ONE
    chunkhead* ch = (chunkhead *)sbrk(sizeof(chunkhead));
    
    ch->info = 1;
    ch->size = mem_to_alloc - sizeof(chunkhead);

    void* data_p = sbrk(ch->size);

    chunkhead* old_last = get_last_chunk();

    old_last->next = (unsigned char*)ch;
    ch->prev = (unsigned char*)old_last;
    ch->next = NULL;

    return (unsigned char*)data_p;
}

void myfree(unsigned char *address)
{

    unsigned char* ptr = (unsigned char*)address - sizeof(chunkhead);
    chunkhead* ch = (chunkhead*)ptr;
    
    ch->info = 0;

    chunkhead* next = (chunkhead*) ch->next;
    chunkhead* prev = (chunkhead*) ch->prev;
    
    if (ch->prev == NULL && ch->next == NULL)
    {
        startofheap = NULL;
        void *pagebreak = sbrk(0);

/*         printf("chunk location %x\n", (int)ch);
        printf("should be same location as above %x\n", ((int) (pagebreak - (ch->size + sizeof(chunkhead)))) ) ; */

        
        brk(pagebreak - (ch->size + sizeof(chunkhead) ));
        return;
    }
    
    if (ch->prev == NULL)
    {
        if (next->info == 1)
        {
            return;
        }
        else
        {
            ch->next = next->next;
            ch->size +=  next->size + sizeof(chunkhead); 
            return;
        }
    }

    if (ch->next == NULL)
    {
        if (prev->info == 1)
        {
            //deallocate current
            prev->next = NULL;
            void *pagebreak = sbrk(0);
            brk(pagebreak - (ch->size + sizeof(chunkhead) ));
            return;
        }
        else
        {
            if (prev->prev == NULL)
            {
                void *pagebreak = sbrk(0);
                brk(pagebreak - (ch->size + prev->size + sizeof(chunkhead)*2 ));
                startofheap = NULL;
                return;
            }
            else
            {
                chunkhead* new_last = (chunkhead*)prev->prev;
                new_last->next = NULL;
                void *pagebreak = sbrk(0);
                brk(pagebreak - (ch->size + sizeof(chunkhead) ));
                return;
            }
        }
    }
    

    if (prev->info == 1 && next->info == 0)
    {
        if(next->next = NULL)
        {
            prev->next = NULL;
            void *pagebreak = sbrk(0);
            brk(pagebreak - (ch->size + sizeof(chunkhead) ));
            return;
        }
        ch->next = next->next;
        ch->size += next->size + sizeof(chunkhead);
        return;
    }
    else if (prev->info == 0 && next->info == 1)
    {
        prev->next = ch->next;
        next->prev = ch->prev;
        prev->size += ch->size + sizeof(chunkhead);
        return;
    }
    else if (prev->info == 0 && next->info == 0)
    {
        chunkhead* first = (chunkhead*)prev->prev;
        chunkhead* last = (chunkhead*)next->next; 

        if (first == NULL && last == NULL)
        {
            void *pagebreak = sbrk(0);
            brk(pagebreak - (ch->size + prev->size + next->size + sizeof(chunkhead)*3 ));
            startofheap = NULL;
            return;
        }
        prev->next = next->next;
        prev->size += ch->size + next->size + 2*sizeof(chunkhead);
        return;
    }
    else
    {
        return;
    }  

}


void analyze()
{
    printf("\n--------------------------------------------------------------\n");
    if(!startofheap)
    {
        printf("no heap\n");
        return;
    }
    chunkhead* ch = (chunkhead*)startofheap;
    for (int no=0; ch; ch = (chunkhead*)ch->next,no++)
    {
        printf("%d | current addr: %x |", no, (int)ch);
        printf("size: %d | ", ch->size);
        printf("info: %d | ", ch->info);
        printf("next: %x | ", (int)ch->next);
        printf("prev: %x", (int)ch->prev);
        printf(" \n");
    }
}



void main()
{
/*     void *programbreak_1 = sbrk(0);
    brk(programbreak_1 + 1024);                //funfact, void* plus offset works here
    void *programbreak_2 = sbrk(0);           //programbreak 2 is now exactly 1024 bytes away from the first. I debugged it and confirmed
    brk(programbreak_1);
    void *programbreak_3 = sbrk(0);           //programbreak 3 is now back to address of the first! */

/*     unsigned char *a, *b, *c;
    a = mymalloc(1000);
    b = mymalloc(1000);

    analyze();
    myfree(a);
    analyze();
    myfree(b);
    analyze(); */
    printf("\n");
    
    byte*a[100];
    analyze();//50% points
    for(int i=0;i<100;i++)
    a[i]= mymalloc(1000);
    for(int i=0;i<90;i++)
    myfree(a[i]);
    analyze(); //50% of points if this is correct
/*     myfree(a[95]);
     a[95] = mymalloc(1000);
    analyze();//25% points, this new chunk should fill the smaller free one
    //(best fit)
    for(int i=90;i<100;i++)
    myfree(a[i]);
    analyze();// 25% should be an empty heap now with the start address from the program start
 */

}