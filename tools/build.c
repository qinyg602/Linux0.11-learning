#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<elf.h>
#include<unistd.h>
#include<malloc.h>
#include<string.h>
#include<fcntl.h>
#include<error.h>

#define SETUPLEN	4
#define	PAGE_OFFSET	0xC0000000

void die(char* str)
{
	fprintf(stderr,"%s\n",str);
	exit(1);
}

int main(int argc,char* argv[])
{
	int c,fd;
	extern int errno;
	size_t i;
	FILE* fp;
	Elf32_Ehdr elf32_header;
	Elf32_Phdr* pheader;
	int pHdoff=0;
	int pHdnum=0;
	int pSize=0;
	char buf[1024]={0};
	
	if(argc!=4) die("build boot.bin setup.bin system>image");
	if((fd=open(argv[1],O_RDONLY))<0) die("Open boot.bin failed");
	i=read(fd,buf,sizeof(buf));
	if(i!=512) die("wrong boot...");
	i=write(1,buf,512);
	if(i!=512) die("write call failed on boot.bin...");
	close(fd);

	if ((fd=open(argv[2],O_RDONLY,0))<0) die("open setup.bin failed!");
	for(c=0;(i=read(fd,buf,sizeof(buf)))>0;c+=i){
		if(write(1,buf,i)!=i) die("write setup.bin failed!");	
	}
	close(fd);
	if(c>SETUPLEN*512)die("wrong setup.bin");
	for(i=0;i<sizeof(buf);i++) buf[i]='\0';
	while(c<SETUPLEN*512){
		i=SETUPLEN*512-c;
		if(i>sizeof(buf)) i=sizeof(buf);
		if((write(1,buf,i))!=i) die("write call failed on setup.bin");	
		c+=i;
	}
	
//read system
	if((fp=fopen(argv[3],"rb"))==NULL) die("open system failed!");
	fseek(fp,0,SEEK_SET);
	fread(&elf32_header,sizeof(elf32_header),1,fp);
	if((elf32_header.e_ident[0]!=ELFMAG0) || (elf32_header.e_ident[1]!=ELFMAG1) ||(elf32_header.e_ident[2]!=ELFMAG2) ||(elf32_header.e_ident[3]!=ELFMAG3))
		die("not elf format!");
	pHdoff=elf32_header.e_phoff;
	pHdnum=elf32_header.e_phnum;
	Elf32_Phdr* pPHeaders=(Elf32_Phdr*)malloc(sizeof(Elf32_Phdr)*pHdnum);
	if(pPHeaders==NULL) die("malloc call failed!");
	if(fread(pPHeaders,1,sizeof(Elf32_Phdr)*pHdnum,fp)!=(sizeof(Elf32_Phdr)*pHdnum)){
		perror("fread:");
		free(pPHeaders);
		die("fread call failed!");		
	}
	Elf32_Phdr* ptemp=pPHeaders;
	for(int i=0;i<pHdnum;i++,ptemp++){
		if(ptemp->p_type==PT_LOAD){
			char* pbuf=(char*)malloc(ptemp->p_memsz);
			if(pbuf==NULL){
				free(pPHeaders);
				fclose(fp);
				die("fread call failed!");	
			}
			fseek(fp,ptemp->p_offset,SEEK_SET);
			if(fread(pbuf,1,ptemp->p_filesz,fp)!=ptemp->p_filesz){
				free(pPHeaders);
				free(pbuf);
				fclose(fp);
				die("fread call failed!");
			}
			
			int delta=ptemp->p_memsz-ptemp->p_filesz;			
			if(delta>0){
				for(int j=0;j<delta;j++)
					pbuf[j+ptemp->p_filesz]='\0';
			}
			fseek(stdout,((ptemp->p_vaddr-PAGE_OFFSET)+512*5),SEEK_SET);
			if(fwrite(pbuf,1,ptemp->p_memsz,stdout)!=ptemp->p_memsz){
				free(pPHeaders);
				free(pbuf);
				fclose(fp);
				die("fread call failed!");
			}
			free(pbuf);
		}
				
	}
	fclose(fp);
	return 0;	
}
