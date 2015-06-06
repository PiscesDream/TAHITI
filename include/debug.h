#ifndef _DEBUG_H_
#define _DEBUG_H_

#define get_seg_byte(seg,addr) ({ \
register char __res; \
__asm__("push %%fs;mov %%ax,%%fs;movb %%fs:%2,%%al;pop %%fs" \
	:"=a" (__res):"0" (seg),"m" (*(addr))); \
__res;})

#define set_word(seg,addr,value)  \
__asm__("push %%fs; mov %%ax,%%fs; movw %%bx,%%fs:%1; pop %%fs" \
	::"a"(seg),"m"(*(addr)),"b"(value)); \

void __debug_keyboard();
void __debug_diplay_info();
void __debug_smile();
void init_debug();

#endif
