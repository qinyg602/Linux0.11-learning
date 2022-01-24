#include <asm/io.h>
#include<linux/head.h>
#include<linux/hdreg.h>
#include<asm/system.h>
#include<proto.h>

#define MAJOR_NR 3
#include<linux/blk.h>

#define CMOS_READ(addr) ({ \
	outb_p(addr,0x70); \
	inb_p(0x71); \
})

#define port_read(port,buf,nr) \
	__asm__("cld;rep;insw"::"d"(port),"D"(buf),"c"(nr))
#define port_write(port,buf,nr) \
	__asm__("cld;rep;outsw"::"d"(port),"S"(buf),"c"(nr))

#define MAX_HD	2

struct hd_i_struct {
	int head,sect,cyl,wpcom,lzone,ctl;
	};
#ifdef HD_TYPE
struct hd_i_struct hd_info[] = { HD_TYPE };
#define NR_HD ((sizeof (hd_info))/(sizeof (struct hd_i_struct)))
#else
struct hd_i_struct hd_info[] = { {0,0,0,0,0,0},{0,0,0,0,0,0} };
static int NR_HD = 0;
#endif
extern void hd_interrupt();
char buf1[512]={0};
char buf2[512]={97};

static struct hd_struct {
	long start_sect;
	long nr_sects;
} hd[5*MAX_HD]={{0,0},};

static void reset_hd(int nr);
static void read_intr(void);
static void write_intr(void);

int sys_setup(void *BIOS){
	char cmos_disks=0;
	if(!BIOS)
		return -1;
	for(int drive=0;drive<2;drive++){
		hd_info[drive].cyl = *(unsigned short *) BIOS;
		hd_info[drive].head = *(unsigned char *) (2+BIOS);
		hd_info[drive].wpcom = *(unsigned short *) (5+BIOS);
		hd_info[drive].ctl = *(unsigned char *) (8+BIOS);
		hd_info[drive].lzone = *(unsigned short *) (12+BIOS);
		hd_info[drive].sect = *(unsigned char *) (14+BIOS);
		BIOS += 16;
	}
	if ((cmos_disks = CMOS_READ(0x12)) & 0xff)
		if (cmos_disks & 0x0f)
			NR_HD = 2;
		else
			NR_HD = 1;
	else
		NR_HD = 0;
	for (int i = NR_HD ; i < 2 ; i++) {
		hd[i*5].start_sect = 0;
		hd[i*5].nr_sects = 0;
	}
	//reset_hd(0);
	for(int i=0;i<512;i++)
        buf2[i]=97;
    buf2[511]=0;
	hd_out(0,1,2,0,0,WIN_WRITE,write_intr);
	port_write(HD_DATA,buf2,256);
	delay(10);
	hd_out(0,1,1,0,0,WIN_READ,read_intr);
	disp_str("\nread sector 0,head 0,cylinder 0 from hd0.img\n");
	disp_str(buf1);
	while(1);
	return 0;
}


static int controller_ready(void)
{
	int retries=10000;
    int i=inb_p(HD_STATUS);
	while (--retries && (i & 0xc0)!=0x40);
	return (retries);
}
static int win_result(void)
{
	int i=inb_p(HD_STATUS);

	if ((i & (BUSY_STAT | READY_STAT | WRERR_STAT | SEEK_STAT | ERR_STAT))
		== (READY_STAT | SEEK_STAT))
		return(0); /* ok */
	if (i&1) i=inb(HD_ERROR);
	return (1);
}

void hd_out(unsigned int drive,unsigned int nsect,unsigned int sect,
            unsigned int head,unsigned int cyl,unsigned int cmd,
            void (*intr_addr)(void))
{
    register int port asm("dx");
    if(drive > 1){
        disp_str("wrong disk parameters!");
        while(1);
    }
    if(!controller_ready()){
        disp_str("HD controller not ready!");
        while(1);
    }
    do_hd=intr_addr;
    outb_p(hd_info[drive].ctl,HD_CMD);
    port=HD_DATA;
    outb_p(hd_info[drive].wpcom>>2,++port);
    outb_p(nsect,++port);
    outb_p(sect,++port);
    outb_p(cyl&0xff,++port);
    outb_p(cyl>>8 & 0xff,++port);
    outb_p(0xA0|drive<<4|head,++port);
    outb_p(cmd,++port);
}
static void read_intr(void)
{
    port_read(HD_DATA,buf1,256);
}
static void write_intr(void)
{
    disp_str("\nwrite finished!");
}

static void init_intr(void)
{
    disp_str("\ninit the hard disk!");
}
static int drive_busy(void)
{
	unsigned int i;

	for (i = 0; i < 10000; i++)
		if (READY_STAT == (inb_p(HD_STATUS) & (BUSY_STAT|READY_STAT)))
			break;
	i = inb(HD_STATUS);
	i &= BUSY_STAT | READY_STAT | SEEK_STAT;
	if (i == (READY_STAT | SEEK_STAT))
		return(0);
	disp_str("HD controller times out\n");
	return(1);
}

static void reset_controller(int nr)
{
	int	i;

	outb(hd_info[nr].ctl,HD_CMD);
	for(i = 0; i < 100; i++) nop();
	outb(hd_info[0].ctl & 0x0f ,HD_CMD);
	if (drive_busy())
		disp_str("HD-controller still busy\n");
	if ((i = inb(HD_ERROR)) != 1)
		disp_str("HD-controller reset failed\n");
}

static void reset_hd(int nr)
{
	reset_controller(0);
	hd_out(nr,hd_info[nr].sect,hd_info[nr].sect,hd_info[nr].head-1,
		hd_info[nr].cyl,WIN_SPECIFY,init_intr);
}

void unexpected_hd_interrupt(void)
{
	disp_str("Unexpected HD interrupt\n");
}

void hd_init(void)
{
	set_intr_gate(0x2E,hd_interrupt);
	outb_p(inb_p(0x21)&0xfb,0x21);                      // 复位接联的主8259A int2的屏蔽位
	outb(inb_p(0xA1)&0xbf,0xA1);                        // 复位硬盘中断请求屏蔽位(在从片上)
}

