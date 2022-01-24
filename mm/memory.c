#include <proto.h>
#include <linux/sched.h>
#include <errno.h>
#define LOW_MEM 0x100000
#define PAGING_MEMORY (15*1024*1024)
#define PAGING_PAGES (PAGING_MEMORY>>12)
#define MAP_NR(addr) (((addr)-LOW_MEM)>>12)
#define USED    100

#define invalidate()    \
    __asm__("movl %%cr3,%%eax;movl %%eax,%%cr3":::"eax")


static unsigned char mem_map[PAGING_PAGES]={0,};
static unsigned long HIGH_MEMORY=0;

#define copy_page(from,to)    \
    __asm__("cld;rep;movsl"::"S"(from),"D"(to),"c"(1024))

unsigned long get_free_page(void){
    register unsigned long __res asm("ax");
    __asm__("std;repne;scasb\n\t"
            "jne 1f\n\t"
            "movb $1,1(%%edi)\n\t"
            "sall   $12,%%ecx\n\t"
            "addl   %2,%%ecx\n\t"
            "movl   %%ecx,%%edx\n\t"
            "leal   4092(%%edx),%%edi\n\t"
            "movl   $1024,%%ecx\n\t"
            "rep;stosl\n\t"
            "movl   %%edx,%%eax\n\t"
            "1:"
            :"=a"(__res)
            :"0"(0),"i"(LOW_MEM),"c"(PAGING_PAGES),"D"(mem_map+PAGING_PAGES-1)
    );
    return __res;
}

void free_page(unsigned long addr)
{
    if(addr<LOW_MEM || addr>=HIGH_MEMORY)
        return;
    addr-=LOW_MEM;
    addr>>=12;
    if(mem_map[addr]--) return;
    mem_map[addr]=0;
}

int free_page_tables(unsigned long dir)
{
    int i,j;
    unsigned long *pDir=(unsigned long*)dir;
    unsigned long *page_table=NULL;
    if(!pDir) return 0;
    for(i=0;i<768;i++,pDir++){
        if(!(*pDir & 1))
            continue;
        page_table=(unsigned long *)(*pDir & 0xfffff000);
        if(page_table>=LOW_MEM){
            for(j=0;j<1024;j++,page_table++){
                if(!(*page_table & 1))
                    continue;
                free_page(*page_table & 0xfffff000);
                *page_table=0;
            }
        }
        free_page(*pDir & 0xfffff000);
        //*pDir=0;
    }
    invalidate();
    return 0;
}

int copy_page_tables(struct task_struct* tsk)
{
    unsigned long *from_dir,*to_dir;
    unsigned long *from_page_table,*to_page_table;
    unsigned long this_page;

    from_dir=(unsigned long*)(current->cr3);
    to_dir=(unsigned long*)(tsk->cr3);
    for(int i=0;i<1024;from_dir++,to_dir++,i++)
    {
        if(!(*from_dir & 1))
            continue;
        from_page_table=(unsigned long*)(*from_dir & 0xfffff000);
        if((unsigned long)from_page_table<LOW_MEM){
            *to_dir=*from_dir;
            continue;
        }
        if(!(to_page_table=(unsigned long*)get_free_page()))
            {
                return -1;

            }
        *to_dir=(unsigned long)to_page_table|7;
//nr_page_tables=((unsigned long)from_page_table<LOW_MEM)?0xA0:1024;
        for(int j=0;j<1024;from_page_table++,to_page_table++,j++)
        {
            this_page=*from_page_table;
            if(!(this_page & 1))
                continue;
            //this_page &=~2;
            *to_page_table=this_page;
            if(this_page>LOW_MEM)
            {
                *from_page_table=this_page;
                this_page-=LOW_MEM;
                this_page>>=12;
                mem_map[this_page]++;
            }
        }
    }
    invalidate();
	return 0;
}

unsigned long put_page(unsigned long page,unsigned long address)
{
    unsigned long *page_table,tmp;
    if((page<LOW_MEM) || (page>HIGH_MEMORY))
        return 0;
    //page doesn't exist
    if(mem_map[(page-LOW_MEM)>>12]!=1)
        return 0;
    page_table=(unsigned long*)(current->cr3+((address>>20)&0xffc));
    if(*page_table&1)
        page_table=(unsigned long*)((*page_table)&0xfffff000);
    else
    {
        if(!(tmp=get_free_page()))
            return 0;
        *page_table=tmp|7;
        page_table=(unsigned long*)tmp;
    }
    page_table[(address>>12)&0x3ff]=page|7;
    return page;

}
void un_wp_page(unsigned long *table_entry)
{
    unsigned long old_page,new_page;
    old_page=*table_entry&0xfffff000;
    if(old_page>=LOW_MEM && mem_map[MAP_NR(old_page)]==1)
    {
        *table_entry |=2;
        invalidate();
        return;
    }

    if(!(new_page=get_free_page()))
    {
        return;
    }
    if(old_page>=LOW_MEM)
        mem_map[MAP_NR(old_page)]--;
     *table_entry=new_page|=7;
    invalidate();
    copy_page(old_page,new_page);
}
void do_wp_page(unsigned long error_code,unsigned long address)
{
    unsigned long old_page,new_page;

}

void do_no_page(unsigned long error_code,unsigned long address)
{
    unsigned long page;
    if(!(page=get_free_page()))
    {
        return;
    }
    if(put_page(page,address))
        return;
    free_page(page);
}

void mem_init(long start_mem,long end_mem)
{
    HIGH_MEMORY=end_mem;
    int main_mem_start_NO=MAP_NR(start_mem);
    for(int i=0;i<main_mem_start_NO;i++)
        mem_map[i]=USED;
    for(int i=main_mem_start_NO;i<PAGING_PAGES;i++)
        mem_map[i]=0;
}
