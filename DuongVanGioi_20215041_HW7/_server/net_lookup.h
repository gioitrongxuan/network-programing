// net_lookup.h
#ifndef NET_LOOKUP_H
#define NET_LOOKUP_H

#include <stdbool.h>

bool isIPv4(const char *str);
bool isDomain(const char *str);
char* lookup_ip(const char *ip);
char* lookup_domain(const char *domain);

#endif // NET_LOOKUP_H
