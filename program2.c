#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
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

unsigned char* mymalloc(unsigned int size) // function's return is void
{
    
    void* p;

    unsigned int chunks_num = ( (size + sizeof(chunkhead)) / PAGESIZE) 
        + ( ( (size + sizeof(chunkhead))  % PAGESIZE) != 0);
    unsigned int mem_to_alloc = chunks_num * PAGESIZE;

    if(!startofheap) 
    {
        //printf("\n\n\n\n%i\n\n\n\n", mem_to_alloc);
        p = sbrk(mem_to_alloc);
        chunkhead* ch = (chunkhead*)p;
        ch->info = 1;
        ch->prev = ch->next = 0;
        ch->size = mem_to_alloc - sizeof(chunkhead);
        startofheap = ch;

        /* printf("%i\n", (int)sbrk(0));
        printf("%i\n", (int)sbrk(-1024));
        printf("%i\n", (int)sbrk(0));  */
        return (unsigned char*)ch + sizeof(chunkhead); 
    }
    
/*     for (; ch != NULL; ch = (chunkhead*)ch->next)
    {
        if (ch->info == 0 && ch->size >= mem_to_alloc)
        {
            unsigned int og_size = ch->size;

            ch->info = 1;
            if ( ch->size - mem_to_alloc < PAGESIZE + sizeof(chunkhead) )
            {
                
            }
            else
            {
                unsigned char* ptr = (unsigned char*)ch + sizeof(chunkhead) + mem_to_alloc;
                chunkhead* new_ch = (chunkhead*)ptr;
                new_ch->info = 0;

                new_ch->size = og_size - mem_to_alloc - sizeof(chunkhead);
                ch->size = og_size - new_ch->size - sizeof(chunkhead);

                new_ch->prev = (unsigned char*)ch;
                new_ch->next = (unsigned char*)ch->next;
                ch->next = (unsigned char*)new_ch;
                
            }
            return (unsigned char*)ch + sizeof(chunkhead);
        }   
    } */

    //printf("3\n");
    return 0;
}

void myfree(unsigned char *address)
{

    unsigned char* ptr = (unsigned char*)address - sizeof(chunkhead);
    chunkhead* ch = (chunkhead*)ptr;
    
    ch->info = 0;

    chunkhead* next = (chunkhead*) ch->next;
    chunkhead* prev = (chunkhead*) ch->prev;
    
    if (ch->prev == 0 && ch->next == 0)
    {
        return;
    }
    
    if (ch->prev == 0)
    {
        if (next->info == 1)
        {
            return;
        }
        else
        {
            ch->next = (unsigned char*)next->next;
            ch->size +=  next->size + sizeof(chunkhead); 
            return;
        }
    }

    if (ch->next == 0)
    {
        if (prev->info == 1)
        {
            return;
        }
        else
        {
            prev->next = (unsigned char*) ch->next;
            prev->size += ch->size + sizeof(chunkhead); 
            return;
        }
    }
    

    if (prev->info == 1 && next->info == 0)
    {
        ch->next = (unsigned char*)next->next;
        ch->size += next->size + sizeof(chunkhead);
        return;
    }
    else if (prev->info == 0 && next->info == 1)
    {
        prev->next = (unsigned char*) ch->next;
        prev->size += ch->size + sizeof(chunkhead);
        return;
    }
    else if (prev->info == 0 && next->info == 0)
    {
        prev->next = (unsigned char*) next->next;
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

chunkhead* get_last_chunk() //you can change it when you aim for performance
{
    if(!startofheap) //I have a global void *startofheap = NULL;
    return NULL;
    chunkhead* ch = (chunkhead*)startofheap;
    for (; ch->next; ch = (chunkhead*)ch->next);
    return ch;
}

void main()
{

    byte* a[10];
    a[0] = mymalloc(1000);
    analyze();

/*     byte* a[10];
    for(int i=0;i<10;i++)
    a[i]= mymalloc(1000);
    for(int i=0;i<9;i++)
    myfree(a[i]);
    analyze();//50% points
    for(int i=0;i<9;i++)
    a[i]= mymalloc(5000);
    analyze();
    myfree(a[9]);
    a[9] = mymalloc(5000);
    analyze();//25% points
    myfree((byte*)get_last_chunk() + sizeof(chunkhead));
    myfree((byte*)get_last_chunk() + sizeof(chunkhead));
    myfree((byte*)get_last_chunk() + sizeof(chunkhead));
    myfree((byte*)get_last_chunk() + sizeof(chunkhead));
    myfree((byte*)get_last_chunk() + sizeof(chunkhead));
    myfree((byte*)get_last_chunk() + sizeof(chunkhead));
    analyze(); //25% points
 */
}