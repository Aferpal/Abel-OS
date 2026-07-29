#ifndef _PRINTK_H_
#define _PRINTK_H_
void clear_screen();

void put_char(char c);

void remove_char();

void printk(char* fmt, ...);
#endif
