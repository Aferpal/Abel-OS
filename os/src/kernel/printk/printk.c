#include "kernel/printk.h"
#include "arch/x86/io.h"
#include "drivers/keyboard.h"
#include "arch/x86/stdarg.h"
#include "string.h"

static char *video=(char*) 0xb8000;

static short cur_pos_x = 0;
static short cur_pos_y = 0;

void
clear_line(uint32_t y)
{
	for (int j = 0; j < 80; j++) {
		video[160*y + 2*j] = ' ';
		video[160*y + j*2 + 1] = 0x07;
	}
}

void
clear_screen(void)
{       
        for (int i = 0; i < 25; i++) {
        
                clear_line(i);

        }
}

void
scroll_screen(uint32_t times)
{
	if (times >= 25) {
		clear_screen();
	} else {
		for (int i = 0; i < ( 25 - times ); i++) {
			memcpy(video+(times*160)+(i*160), video+(i*160), 160);	
		}

		for (int i = (25 - times); i < 25; i++) {
			clear_line(i);
		}
	}

}

void
update_cursor_vga(void)
{
	unsigned short pos = cur_pos_y * 80 + cur_pos_x;

	outb(0x3D4, 0x0e);
	outb(0x3d5, pos >> 8);

	outb(0x3d4, 0x0f);
	outb(0x3d5, pos & 0xFF);
}

void
advance_line(void)
{
	if (cur_pos_y >= 24) {
		scroll_screen(1);
		cur_pos_y--;
	}
	cur_pos_y++;
	cur_pos_x = 0;
	update_cursor_vga();
}

void
advance_char_n(short n)
{
	cur_pos_x = cur_pos_x + n;
	if (cur_pos_x >= 80) {
		cur_pos_y = cur_pos_y + (cur_pos_x / 80);
		if (cur_pos_y > 24) {
			scroll_screen(cur_pos_y - 24);
		}
		cur_pos_x = cur_pos_x % 80;
	}
	update_cursor_vga();
}

void
advance_char(void)
{
	advance_char_n(1);
}

void
put_char_at(char c, short x, short y)
{
	if (c == '\n') {
		advance_line();
	} else if (c == '\t') {
		advance_char_n(4);
	} else {
		video[160*y + 2*x] = c;
	}
}

void
put_char(char c)
{
	put_char_at(c, cur_pos_x, cur_pos_y);
	if (c != '\n' && c != '\t') {
		advance_char();
	}
}

void
remove_char()
{
	cur_pos_x--;
	if (cur_pos_x < 0) {
		cur_pos_y--;
		cur_pos_x=79;
	}

	put_char_at(' ', cur_pos_x, cur_pos_y);
	update_cursor_vga();
}

void
print_str(char* str)
{
        while (*str != 0) {
                put_char(*str);
                str++;
        }
}


void
print_int(int n)
{
	if (n < 0) {
		put_char('-');
		print_int(-n);
	} else if (n == 0) {
		put_char('0');
	} else {
		char nstr[17];
		nstr[16] = 0;
		int i = 15;
		while ( n > 0 && i >= 0 ) {
			nstr[i] = ( n % 10 ) + '0';
			n /= 10;
			i--;
		}

		i++;
		print_str(&nstr[i]);
	}

}

char
n_to_single_hex(unsigned char n)
{
	if (n < 10) {
		return n + '0';
	} else if (n < 16) {
		return (n - 10) + 'a';
	} else {
		return 'f';
	}
}

void
print_hex(unsigned int n)
{
	print_str("0x");
        if (n == 0) {
                put_char('0');
        } else {
                char nstr[17];
                nstr[16] = 0;
                int i = 15;
		unsigned char current;
                while ( n > 0 && i >= 0 ) {
                        current = ( n % 16 ) & 0x0f;
			nstr[i] = n_to_single_hex(current);
                        n /= 16;
                        i--;
                }

                i++;
                print_str(&nstr[i]);
        }
}

void
printk(char* fmt, ...)
{
	va_list args;

	va_start(args, fmt);

	while (*fmt != 0) {

		if (*fmt == '%') {
			fmt++;
			switch (*fmt) {
			
			case 'd':
				int n = va_arg(args, int);
				print_int(n);
				break;
			case 's':
				char *str = va_arg(args, char*);
				print_str(str);
				break;
			case 'c':
				char c = va_arg(args, int);
				put_char(c);
				break;
			case 'x':
				unsigned int un = va_arg(args, unsigned int);
				print_hex(un);
				break;
			case '%':
				put_char('%');
				break;

			}
		
		} else {
			put_char(*fmt);
		}
		fmt++;
	}
}
