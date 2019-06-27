#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>

#define EVENT_SIZE  (sizeof(struct inotify_event))
#define BUF_LEN     (1024 * (EVENT_SIZE + 16))

typedef struct occurences_s
{
    int     occurence;
    size_t  count;
}occurences_t;

static int stop;

unsigned long long get_unix_timestamp_us(void)
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    unsigned long long us = (tp.tv_sec * 1000 * 1000) + (tp.tv_usec);

    return us;
}

static void usage()
{
    printf("./inotify-test [PATH] [IP] [PORT]\n");
    return;
}

// handle ctrl-c
static void sig_handler(int signum)
{
    if(stop)
    {
        printf("signal %d received, interrupting\n", signum);
        stop = 0;
    }
}

static int compare (const void * a, const void * b)
{
    const int *ca = a;
    const int *cb = b;

    return (*ca > *cb) - (*ca < *cb);
}

static int count_occurences(int tab[], size_t tab_len, occurences_t ** occurs, size_t * occurs_count)
{
    size_t          i                   = 0;
    size_t          current_occur_count = 0;
    occurences_t *  tmp_occurs          = NULL;
    size_t          tmp_occurs_count    = 0;

    for(i = 0; i < tab_len - 1; i++)
    {
        if(tab[i] == tab[i + 1])
        {
            current_occur_count++;
        }
        else
        {
            if(current_occur_count)
            {
                tmp_occurs = realloc(tmp_occurs, sizeof(occurences_t) * (tmp_occurs_count + 1));
                tmp_occurs[tmp_occurs_count].occurence = tab[i];
                tmp_occurs[tmp_occurs_count].count = current_occur_count;
                tmp_occurs_count++;
                current_occur_count = 0;
            }
        }
    }

    if(current_occur_count)
    {
        tmp_occurs = realloc(tmp_occurs, sizeof(occurences_t) * (tmp_occurs_count + 1));
        tmp_occurs[tmp_occurs_count].occurence = tab[i];
        tmp_occurs[tmp_occurs_count].count = current_occur_count;
        tmp_occurs_count++;
        current_occur_count = 0;
    }

    *occurs = tmp_occurs;
    *occurs_count = tmp_occurs_count;

    return 0;

}

static int process_file(char * path, char ** output)
{
    int         ret         = -1;
    char *      oldline     = NULL;
    char *      newline     = NULL;
    size_t      i, j        = 0;
    size_t      count       = 0;
    char *      fbuf        = NULL;
    long int    ftell_rval  = 0;
    FILE *      file        = fopen(path, "r");

    unsigned long start_time_us = get_unix_timestamp_us();
    unsigned long process_time_us = 0;

    *output = NULL;

    if(!file)
    {
        printf("fopen error %s : %s\n", path, strerror(errno));
        goto end;
    }

    // get size
    fseek(file, 0, SEEK_END);
    if ((ftell_rval = ftell(file)) < 0)
    {
        printf("ftell error : %s\n", strerror(errno));
        goto end;
    }

    fseek(file, 0, SEEK_SET);

    if(ftell_rval == 0)
    {
        printf("%s empty\n", path);
        goto end;
    }

    // alloc mem
    fbuf = malloc(ftell_rval + 1);

    // read file
    if(fread(fbuf, 1, ftell_rval, file) != ftell_rval)
    {
        printf("fread error on %s", path);
        goto end;
    }

    fbuf[ftell_rval] = 0;

    //process buffer
    int * tab = NULL;

    oldline = fbuf;
    newline = fbuf;

    while((newline = strstr(newline, "\n")))
    {
        char number[12];
        memset(number, 0, sizeof(number));
        strncpy(number, oldline, newline - oldline);
        tab = realloc(tab, (sizeof(int) * (count + 1)));
        tab[count] = strtol(number, NULL, 10);
        newline++;
        oldline = newline;
        count++;
    }

    if(!count)
        goto end;

    //~ printf("tab : ");
    //~ for(i = 0; i < count; i++)
    //~ {
        //~ printf(" %d", tab[i]);
    //~ }
    //~ printf("\n");

    qsort(tab, count, sizeof(*tab), compare);

    //~ printf("tab : ");
    //~ for(i = 0; i < count; i++)
    //~ {
        //~ printf(" %d", tab[i]);
    //~ }
    //~ printf("\n");

    occurences_t *  occurences = NULL;
    size_t          occurences_count = 0;

    count_occurences(tab, count, &occurences, &occurences_count);

    process_time_us = get_unix_timestamp_us() - start_time_us;
    //~ printf("Processed file %s in %lu us\n", path, process_time_us);

    if(!occurences_count)
        goto end;

    //~ for(i = 0; i < occurences_count; i++)
        //~ printf("Found %zu occurences of %d\n", occurences[i].count, occurences[i].occurence);

    // Generate JSON string to send to dest
    yajl_gen_status yajl_status = yajl_gen_status_ok;
    yajl_gen        yajl_handle = yajl_gen_alloc(NULL);
    char *          yajl_output = NULL;
    size_t          yajl_output_len = 0;

    yajl_gen_map_open(yajl_handle);

    yajl_gen_string(yajl_handle, (const unsigned char *)"filename", strlen("filename"));
    yajl_gen_string(yajl_handle, (const unsigned char *)path, strlen(path));

    yajl_gen_string(yajl_handle, (const unsigned char *)"processing_time_us", strlen("processing_time_us"));
    yajl_gen_integer(yajl_handle, process_time_us);

    yajl_gen_string(yajl_handle, (const unsigned char *)"occurences", strlen("occurences"));
    yajl_gen_array_open(yajl_handle);

    for(i = 0; i < occurences_count; i++)
    {
        yajl_gen_map_open(yajl_handle);
        // Number
        yajl_gen_string(yajl_handle, (const unsigned char *)"number", strlen("number"));
        yajl_gen_integer(yajl_handle, occurences[i].occurence);

        // Occurrences
        yajl_gen_string(yajl_handle, (const unsigned char *)"count", strlen("count"));
        yajl_gen_integer(yajl_handle, occurences[i].count);

        yajl_gen_map_close(yajl_handle);
    }

    yajl_gen_array_close(yajl_handle);
    yajl_gen_map_close(yajl_handle);

    yajl_gen_get_buf(yajl_handle, (const unsigned char **)&yajl_output, &yajl_output_len);

    *output = strdup(yajl_output);

    ret = 0;

end:
    if(file)
        fclose(file);

    if(yajl_handle)
    {
        yajl_gen_clear(yajl_handle);
        yajl_gen_free(yajl_handle);
    }

    free(occurences);
    free(tab);
    free(fbuf);
    return ret;
}

int main(int argc, char **argv)
{
    int         ret = -1;
    int         length, i = 0;
    int         fd;
    int         wd;
    char *      path = NULL;
    size_t      path_len = 0;
    char        buffer[BUF_LEN];
    int         udp_sock = -1;
    struct      sockaddr_in dest;
    unsigned short dest_port = 0;

    if(argc < 4)
    {
        usage();
        goto end;
    }

    struct sigaction int_handler = {.sa_handler=sig_handler};
    sigaction(SIGINT,&int_handler,0);

    // Initialize dest socket here
    if((udp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        printf("socket error : %s\n", strerror(errno));
        goto end;
    }

    memset(&dest, 0, sizeof(dest));

    sscanf(argv[3], "%hu", &dest_port);
    dest.sin_port   = htons(dest_port);
    dest.sin_family = AF_INET;

    if(!inet_aton(argv[2], &(dest.sin_addr)))
    {
        printf("invalid address : %s\n", argv[2]);
        goto end;
    }

    // Add some verification (is it folder, does it exist)
    path = argv[1];
    path_len = strlen(path);

    fd = inotify_init();

    if(fd < 0)
    {
        printf("inotify_init error : %s\n", strerror(errno));
        goto end;
    }

    stop = 1;

    wd = inotify_add_watch(fd, path, IN_MODIFY | IN_CREATE | IN_DELETE);

    while(stop)
    {
        i = 0;
        length = read(fd, buffer, BUF_LEN);

        if(length < 0)
        {
            if(errno == EINTR);
            {
                break;
            }
        }

        while(i < length)
        {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if(event->len)
            {
                if(event->mask == IN_CREATE)
                {
                    
                    pid_t worker = fork();
                    if(!worker)
                    {
                        int     fork_ret = -1;
                        char *  json_data = NULL;
                        size_t  fullpath_len = path_len + strlen(event->name) + 1;
                        char *  fullpath = (char *)malloc(fullpath_len + 2);
                        snprintf(fullpath, fullpath_len + 1, "%s/%s", path, event->name);
                        fork_ret = process_file(fullpath, &json_data);
                        free(fullpath);
                        if(fork_ret < 0)
                            return -1;
                        printf("JSON : %s\n", json_data);
                        sendto(udp_sock, json_data, strlen(json_data), 0, (const struct sockaddr *) &dest, sizeof(dest));
                        free(json_data);
                        fflush(stdout);
                        return 0;
                    }
                }
            }
            i += EVENT_SIZE + event->len;
        }
    }

    ret = 0;

end:
    if(fd)
    {
        inotify_rm_watch(fd, wd);
        close(fd);
    }

    close(udp_sock);

    printf("Closing with status %d\n", ret);
    return ret;
}
