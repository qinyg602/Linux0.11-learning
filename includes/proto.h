
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            proto.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* klib.asm */
#ifndef __proto__
#define __proto__
 #define TRUE 1
 #define FALSE 0
extern void	disp_str(char * info);
extern void	disp_color_str(char * info, int color);
extern void	clear();
char *itoa(char * str, unsigned long num);
void disp_int(int addr);
extern void delay();
#endif
