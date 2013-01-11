#include <iostream>
#include "fcgio.h"

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

#include <string>
#include <sstream>

using namespace std;



class Timer{

    public:

        struct timeval start_time, end_time;

        void start(){

            gettimeofday( &start_time, NULL );

        }

        void stop(){

            gettimeofday( &end_time, NULL );

        }

        string getComment(){

            long seconds, useconds;

            double mtime;

            seconds  = end_time.tv_sec  - start_time.tv_sec;
            useconds = end_time.tv_usec - start_time.tv_usec;

            mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

            std::ostringstream output_stream;
            output_stream << "Elapsed time: " << mtime << " milliseconds\n";

            return output_stream.str();

        }


};


int main( void ){

    // Backup the stdio streambufs
    streambuf * cin_streambuf  = cin.rdbuf();
    streambuf * cout_streambuf = cout.rdbuf();
    streambuf * cerr_streambuf = cerr.rdbuf();

    FCGX_Request request;

    FCGX_Init();
    FCGX_InitRequest(&request, 0, 0);


    Timer my_timer;

    while( FCGX_Accept_r(&request) == 0 ){

        my_timer.start();

        fcgi_streambuf cin_fcgi_streambuf(request.in);
        fcgi_streambuf cout_fcgi_streambuf(request.out);
        fcgi_streambuf cerr_fcgi_streambuf(request.err);

        cin.rdbuf(&cin_fcgi_streambuf);
        cout.rdbuf(&cout_fcgi_streambuf);
        cerr.rdbuf(&cerr_fcgi_streambuf);

        cout << "Content-type: text/html\r\n"
             << "\r\n"
             << "<html>\n"
             << "  <head>\n"
             << "    <title>Hello, World!</title>\n"
             << "  </head>\n"
             << "  <body>\n"
             << "    <h1>Hello, World!</h1>\n"
             << "  </body>\n"
             << "</html>\n";

        //usleep(10);

        my_timer.stop();

        cout << my_timer.getComment() << endl;

        // Note: the fcgi_streambuf destructor will auto flush
    }

    // restore stdio streambufs
    cin.rdbuf(cin_streambuf);
    cout.rdbuf(cout_streambuf);
    cerr.rdbuf(cerr_streambuf);

    return 0;

}



