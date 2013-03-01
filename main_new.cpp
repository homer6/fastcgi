/*
 * threaded.c -- A simple multi-threaded FastCGI application.
 */

#ifndef lint
static const char rcsid[] = "$Id: threaded.c,v 1.9 2001/11/20 03:23:21 robs Exp $";
#endif /* not lint */

#include "fcgi_config.h"

#include <pthread.h>
#include <sys/types.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "fcgiapp.h"

#include <string>
#include <set>



#define THREAD_COUNT 100

using namespace std;

static int counts[THREAD_COUNT];
static set<string> client_ips;

static void *doit(void *a)
{
    int rc, thread_id = (int)a;
    int i = 0;
    pid_t pid = getpid();
    FCGX_Request request;
    char *server_name;
    char *remote_address;

    FCGX_InitRequest(&request, 0, 0);

    for (;;)
    {
        static pthread_mutex_t accept_mutex = PTHREAD_MUTEX_INITIALIZER;
        static pthread_mutex_t counts_mutex = PTHREAD_MUTEX_INITIALIZER;
        static pthread_mutex_t client_ips_mutex = PTHREAD_MUTEX_INITIALIZER;

        /* Some platforms require accept() serialization, some don't.. */
        pthread_mutex_lock(&accept_mutex);
            rc = FCGX_Accept_r(&request);
        pthread_mutex_unlock(&accept_mutex);

        if (rc < 0)
            break;

        server_name = FCGX_GetParam("SERVER_NAME", request.envp);
        remote_address = FCGX_GetParam("REMOTE_ADDR", request.envp);
        string client_ip( remote_address );

        FCGX_FPrintF( request.out,

            "Content-type: text/html\r\n"
            "\r\n"
            "<title>FastCGI Hello! (multi-threaded C, fcgiapp library)</title>"
            "<h1>FastCGI Hello! (multi-threaded C, fcgiapp library)</h1>"
            "Thread %d, Process %ld<p>"
            "Request counts for %d threads running on host <i>%s</i>"
            "Client IP: <i></i><p><code>",

            thread_id,
            pid,
            THREAD_COUNT,
            server_name ? server_name : "?",
            remote_address ? remote_address : "?"

        );

        //sleep(2);
        pthread_mutex_lock( &client_ips_mutex );
            client_ips.insert( client_ip );
        pthread_mutex_unlock( &client_ips_mutex );

        pthread_mutex_lock( &counts_mutex );
            ++counts[thread_id];
            i++;
            FCGX_FPrintF( request.out, "%5d ", i );

            for (i = 0; i < THREAD_COUNT; i++)
                FCGX_FPrintF(request.out, "%5d " , counts[i]);

        pthread_mutex_unlock( &counts_mutex );


        set<string>::iterator it;
        for( it = client_ips.begin(); it != client_ips.end(); it++ ){
            FCGX_FPrintF( request.out, "<br/>%s\n", (*it).c_str() );
        }


        FCGX_Finish_r(&request);
    }

    return NULL;
}

int main(void)
{
    int i;
    pthread_t id[THREAD_COUNT];

    FCGX_Init();

    for (i = 1; i < THREAD_COUNT; i++)
        pthread_create(&id[i], NULL, doit, (void*)i);

    doit(0);

    return 0;
}
