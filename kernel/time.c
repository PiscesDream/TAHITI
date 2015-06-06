#include <utils/port.h>

const int cmos_address = 0x70;
const int cmos_data    = 0x71;

unsigned char get_RTC_register(int reg) {
    outb(reg, cmos_address);
    return inb(cmos_data);
}

//    second = get_RTC_register(0x00);
//    minute = get_RTC_register(0x02);
//    hour = get_RTC_register(0x04);
//    day = get_RTC_register(0x07);
//    month = get_RTC_register(0x08);
//    year = get_RTC_register(0x09);
