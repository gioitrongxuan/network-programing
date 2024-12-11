#include <netinet/in.h>
struct in_addr {
in_addr_t s_addr; // 32-bit IPv4 address
// network byte ordered
};
struct sockaddr_in {
uint8_t sin_len; // length of structure
sa_family_t sin_family; // AF_INET
in_port_t sin_port; // 16-bit TCP or UDP port number
// network byte ordered
struct in_addr sin_addr; // 32-bit IPv4 address
// network byte ordered
char sin_zero[8]; // unused
};