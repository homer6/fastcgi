#include <iostream>
#include <bitset>
#include "fcgio.h"

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

#include <string>
#include <sstream>

#include <map>

#include "sha256.h"

using namespace std;


#define INPUT_BUFFER_SIZE 10000
#define MAX_INPUT_MESSAGE_BODY 100000000  //100MB


template< class T >
void print_as_binary( T value, ostream& output_stream ){

    bitset< sizeof(T) * 8 > x( value );

    output_stream << x ;

}


void print_as_hex( string source_string, ostream& output_stream ){

    size_t number_of_characters = source_string.size();

    for( size_t x = 0; x < number_of_characters; x++ ){

        output_stream << hex
                      << setfill('0') << setw(2)
                      << int(source_string[ x ]) << " ";

        if( (x + 1) % 16 == 0 ){
            output_stream << "<br />" << endl;
        }

    }

}




class Request{

    public:
        Request( FCGX_Request *fastcgi_request )
            :fastcgi_request(fastcgi_request)
        {


        }

        void reload(){

            this->parameters.clear();

            this->parameters[ "GATEWAY_INTERFACE" ] = FCGX_GetParam( "GATEWAY_INTERFACE", this->fastcgi_request->envp );
            this->parameters[ "SERVER_SOFTWARE" ] = FCGX_GetParam( "SERVER_SOFTWARE", this->fastcgi_request->envp );
            this->parameters[ "QUERY_STRING" ] = FCGX_GetParam( "QUERY_STRING", this->fastcgi_request->envp );
            this->parameters[ "REQUEST_METHOD" ] = FCGX_GetParam( "REQUEST_METHOD", this->fastcgi_request->envp );
            this->parameters[ "CONTENT_TYPE" ] = FCGX_GetParam( "CONTENT_TYPE", this->fastcgi_request->envp );
            this->parameters[ "CONTENT_LENGTH" ] = FCGX_GetParam( "CONTENT_LENGTH", this->fastcgi_request->envp );
            this->parameters[ "SCRIPT_FILENAME" ] = FCGX_GetParam( "SCRIPT_FILENAME", this->fastcgi_request->envp );
            this->parameters[ "SCRIPT_NAME" ] = FCGX_GetParam( "SCRIPT_NAME", this->fastcgi_request->envp );
            this->parameters[ "REQUEST_URI" ] = FCGX_GetParam( "REQUEST_URI", this->fastcgi_request->envp );
            this->parameters[ "DOCUMENT_URI" ] = FCGX_GetParam( "DOCUMENT_URI", this->fastcgi_request->envp );
            this->parameters[ "DOCUMENT_ROOT" ] = FCGX_GetParam( "DOCUMENT_ROOT", this->fastcgi_request->envp );
            this->parameters[ "SERVER_PROTOCOL" ] = FCGX_GetParam( "SERVER_PROTOCOL", this->fastcgi_request->envp );
            this->parameters[ "REMOTE_ADDR" ] = FCGX_GetParam( "REMOTE_ADDR", this->fastcgi_request->envp );
            this->parameters[ "REMOTE_PORT" ] = FCGX_GetParam( "REMOTE_PORT", this->fastcgi_request->envp );
            this->parameters[ "SERVER_ADDR" ] = FCGX_GetParam( "SERVER_ADDR", this->fastcgi_request->envp );
            this->parameters[ "SERVER_PORT" ] = FCGX_GetParam( "SERVER_PORT", this->fastcgi_request->envp );
            this->parameters[ "SERVER_NAME" ] = FCGX_GetParam( "SERVER_NAME", this->fastcgi_request->envp );

            this->loadInput();

        }


        void loadInput(){

            size_t number_of_bytes_requested = INPUT_BUFFER_SIZE;
            size_t number_of_bytes_read;
            size_t total_bytes_read = 0;

            this->message_body = "";

            do{

                number_of_bytes_read = FCGX_GetStr( this->input_buffer, number_of_bytes_requested, fastcgi_request->in );
                if( number_of_bytes_read <= 0 ){
                    break;
                }
                total_bytes_read += number_of_bytes_read;

                if( total_bytes_read > MAX_INPUT_MESSAGE_BODY ){
                    this->message_body.append( "Max Body Length Reached" );
                    break;
                }

                this->message_body.append( this->input_buffer, number_of_bytes_read );

            }while( number_of_bytes_read == INPUT_BUFFER_SIZE );


        }


        string getParameterByName( string parameter_name ){

            string parameter_value;
            parameter_value = FCGX_GetParam( parameter_name.c_str(), fastcgi_request->envp );
            return parameter_value;

        }


        map< string, string > parameters;
        string message_body;

    protected:
        FCGX_Request *fastcgi_request;

        char input_buffer[ INPUT_BUFFER_SIZE ];




};


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

    FCGX_Request fastcgi_request;

    FCGX_Init();
    FCGX_InitRequest(&fastcgi_request, 0, 0);


    Timer my_timer;

    int x = 0;

    Request *request = new Request( &fastcgi_request );
    map<string,string>::iterator parameter_iterator;

    while( FCGX_Accept_r(&fastcgi_request) == 0 ){

        x++;

        my_timer.start();

        request->reload();

        //cout << request->message_body << endl;
        print_as_hex( request->message_body, cout );
        cout << endl;

        fcgi_streambuf cin_fcgi_streambuf( fastcgi_request.in );
        fcgi_streambuf cout_fcgi_streambuf( fastcgi_request.out );
        fcgi_streambuf cerr_fcgi_streambuf( fastcgi_request.err );

        cin.rdbuf( &cin_fcgi_streambuf );
        cout.rdbuf( &cout_fcgi_streambuf );
        cerr.rdbuf( &cerr_fcgi_streambuf );

        cout << "Content-type: text/html\r\n"
             << "\r\n"
             << "<html>\n"
             << "  <head>\n"
             << "    <title>Hello, World!</title>\n"
             << "  </head>\n"
             << "  <body>\n"
             << "    <h1>Hello, World!</h1>\n"
             ;

        //usleep(10000000);
        //cin >> input;

        cout << sha256(request->message_body) << endl;
        //cout << request->message_body << endl;



        cout << endl;

        cout << sizeof( FCGX_Request ) << endl;

        cout << x << "<br/>" << endl;

        for( parameter_iterator = request->parameters.begin(); parameter_iterator != request->parameters.end(); parameter_iterator++ ){
            cout << parameter_iterator->first << " => " << parameter_iterator->second << "<br/>" << endl;
        }

        cout << "</body></html>" << endl;

        my_timer.stop();
        cout << my_timer.getComment() << endl;

        // Note: the fcgi_streambuf destructor will auto flush
    }

    // restore stdio streambufs
    cin.rdbuf( cin_streambuf );
    cout.rdbuf( cout_streambuf );
    cerr.rdbuf( cerr_streambuf );

    delete request;

    return 0;

}



