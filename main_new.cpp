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
#include <vector>
#include <exception>
#include <fstream>
#include <iostream>

#include <cstdlib>




#define THREAD_COUNT 30

using namespace std;

//static int counts[THREAD_COUNT];
//static set<string> client_ips;
static string *served_contents;


struct ThreadStore{

    int i;

};


static void *doit( void *a ){

    //ThreadStore *store = (ThreadStore*)a;

    int rc;
    //int i = 0;
    //pid_t pid = getpid();
    FCGX_Request request;
    //char *server_name;
    //char *remote_address;

    FCGX_InitRequest( &request, 0, 0 );

    int num_bytes_written;

    const char *served_contents_cstr = served_contents->c_str();
    int served_contents_number_of_bytes = served_contents->length();

    //static pthread_mutex_t accept_mutex = PTHREAD_MUTEX_INITIALIZER;
    //static pthread_mutex_t counts_mutex = PTHREAD_MUTEX_INITIALIZER;
    //static pthread_mutex_t client_ips_mutex = PTHREAD_MUTEX_INITIALIZER;


    for(;;){



        /* Some platforms require accept() serialization, some don't.. */
        //pthread_mutex_lock( &accept_mutex );
            rc = FCGX_Accept_r( &request );
        //pthread_mutex_unlock( &accept_mutex );


        if (rc < 0)
            continue;

        //server_name = FCGX_GetParam( "SERVER_NAME", request.envp );
        //remote_address = FCGX_GetParam( "REMOTE_ADDR", request.envp );
        //string client_ip( remote_address );



        FCGX_FPrintF( request.out,
            "Content-Type: application/xml; charset=UTF-8\r\n"
            "Content-Encoding: gzip\r\n"
            "\r\n"
        );

        num_bytes_written = FCGX_PutStr( served_contents_cstr, served_contents_number_of_bytes, request.out );
        if( num_bytes_written != served_contents_number_of_bytes || num_bytes_written == -1 ){
            break;
        }


        //sleep(2);
        /*
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
        */


        FCGX_Finish_r( &request );

    }

    return NULL;
}



/**
 * Reads the whole file into a string (that it allocates on the heap).
 * Call must free this string after use.
 * @see http://stackoverflow.com/a/2602258/278976
 */
string *file_to_string( string filepath ){

    std::ifstream input_file_stream( filepath.c_str() );
    input_file_stream.seekg( 0, std::ios::end );
    size_t size = input_file_stream.tellg();
    std::string *buffer = new string( size, ' ' );
    input_file_stream.seekg( 0 );
    input_file_stream.read( &(*buffer)[0], size );

    return buffer;

}




int main( int argc, char** argv ){

    int i;
    pthread_t id[THREAD_COUNT];

    if( argc < 2 ){
        cout << "usage: " << argv[0] << " <file-to-serve>" << endl;
        return 1;
    }

    try{
        served_contents = file_to_string( argv[1] );
    }catch( exception& e ){
        cout << "exception caught: " << e.what() << '\n';
        return 1;
    }

    //should delete the served_contents after use


    FCGX_Init();

    ThreadStore *current_store;
    vector<ThreadStore*> thread_stores;

    for( i = 0; i < THREAD_COUNT; i++ ){
        current_store = new ThreadStore;
        current_store->i = i;
        thread_stores.push_back( current_store );
        pthread_create( &id[i], NULL, doit, (void*)current_store );
    }

    for( i = 0; i < THREAD_COUNT; i++ ){
        pthread_join( id[i], NULL );
    }


    for( vector<ThreadStore*>::iterator it = thread_stores.begin(); it != thread_stores.end(); ++it ){
        delete *it;
    }

    delete served_contents;

    return 0;

}
