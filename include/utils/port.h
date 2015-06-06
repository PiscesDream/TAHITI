#define outportb(port, value) \
	__asm__ ("outb %%al, %%dx"::"a"(value), "d"(port))

#define inportb(port) ({\
	unsigned char _result; \
	__asm__ ("inb %%dx, %%al":"=a"(_result):"d"(port)); \
	_result; \
})


#define outb(value, port) \
	__asm__ ("outb %%al, %%dx"::"a"(value), "d"(port))

#define inb(port) ({\
	unsigned char _result; \
	__asm__ ("inb %%dx, %%al":"=a"(_result):"d"(port)); \
	_result; \
})

#define outb_pause(value, port) \
    __asm__ ("outb %%al, %%dx; jmp 1f; 1: jmp 1f;1:"::"a"(value), "b"(port))

#define inb_p(port) ({ \
    unsigned char _result; \
    __asm__ volatile ("inb %%dx,%%al; jmp 1f; 1:jmp 1f; 1:":"=a" (_result):"d" (port)); \
    _result; \
})

