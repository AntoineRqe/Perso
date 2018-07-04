#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_LINE 128

// Extract line from a buffer
static char * extract_line(const char * buf, char * line, size_t count)
{
    char * 	tok = NULL;
    size_t  len = 0;

    if(!buf || !line)
        return NULL;

    tok = strstr(buf, "\n");
    if(!tok)
        return NULL;
    if((len = (tok - buf)) == 0)
        return NULL;

    if(len >= count)
        return NULL;

    memcpy(line, buf, len);
    memcpy(line + len, "\0", 1);

    if(*(tok + 1) == '\0')
        return NULL;

    return (tok + 1);
}

// Extract local addr from /proc/net/tcp file
static int extract_local_addr(char * line, char * addr, int is_ipv6)
{
    int     addr_size           = 8;
    int     port_size           = 4;
    char *  p                   = NULL;
    char *  tok                 = NULL;
    char    buf[MAX_LINE]   = {0};

    if(!line || !addr)
        return -1;
    else if(strlen(line) < 3)
        return -1;

    if(is_ipv6)
        addr_size = 32;

    snprintf(buf, MAX_LINE, "%s", line);
    p = buf;

    tok = strsep(&p, ":");
    if(!p)
        return -1;

    memcpy(addr, p + 1, addr_size + port_size + 1);
    memcpy(addr + addr_size + port_size + 1, "\0", 1);

    return 0;
}

int main(int c, char* v[])
{
    const char s[] = "hello\nthis is\nit\nAntoine, Bye\n";
    char line[MAX_LINE] = {0};
    const char *p = s;
    char * tok = NULL;

    while(p)
    {
        p = extract_line(p, line, MAX_LINE);
        printf("Extract [%s][%zu]\n", line, strlen(line));
        printf("Remaining [%s]\n", p);
    }
    return 0;
}
