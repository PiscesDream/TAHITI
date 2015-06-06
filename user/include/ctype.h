#ifndef _CYPTE_H_
#define _CYPTE_H_

inline int isspace(const int c);
inline int isdigit(const int c);
void atob(int * dest, const char * src, int base);

#define ISSPACE " \t\n\r\f\v"
#define ISDIGIT "0123456789"

#endif

