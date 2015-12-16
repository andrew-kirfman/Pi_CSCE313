/* 
    File: client.cpp

    Author: Andrew Kirfman & Margaret Baxter
            CSCE-313 Machine Problem #3
            Texas A&M University
    Date  : 10/22/2015
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#include "reqchannel.h"

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

string int2string(int number) {
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

void process_mimic(RequestChannel & _channel, const string &  _request) {
  _channel.cwrite(int2string(rand() % 100));
}

/*string func_send_request(string str) {
    return int2string(rand() % 100);
}*/

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) 
{
    
    cout << "CLIENT STARTED:" << endl;
    timeval start_time, end_time;
    int ipc_delay, function_delay;
    
    int pid = fork();
    if(pid == 0)
    {
        cout << "Establishing control channel... " << flush;
        RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
        cout << "done." << endl;

        /* -- Start sending a sequence of requests */

       /* string reply1 = chan.send_request("hello");
        cout << "Reply to request 'hello' is '" << reply1 << "'" << endl;

        string reply2 = chan.send_request("data Joe Smith");
        cout << "Reply to request 'data Joe Smith' is '" << reply2 << "'" << endl;

        string reply3 = chan.send_request("data Jane Smith");
        cout << "Reply to request 'data Jane Smith' is '" << reply3 << "'" << endl;

        for(int i = 0; i < 100; i++) 
        {
            string request_string("data TestPerson" + int2string(i));
            string reply_string = chan.send_request(request_string);
	        cout << "Reply to request " << i << ":" << reply_string << endl;;
        }*/
        
        /* Calculate propogation delay */
        int ITERATIONS = 10000;
        double average_IPC_delay, average_func_delay, stdv_IPC_delay, stdv_func_delay;
        
        /* Generate an average */
        assert(gettimeofday(&start_time, 0) == 0);        
        for(int i=0; i<ITERATIONS; i++)
        {
            /* IPC delay */
            string request_string("data test " + int2string(i));
            string ipc_reply = chan.send_request(request_string);
        }
        assert(gettimeofday(&end_time, 0) == 0);
        average_IPC_delay = ((end_time.tv_sec - start_time.tv_sec)*1000000L + end_time.tv_usec) - start_time.tv_usec;
        average_IPC_delay = (average_IPC_delay)/ITERATIONS;
        
        /* Calculate the standard deviation */
        for(int i=0; i<ITERATIONS; i++)
        {
            /* IPC delay */
            assert(gettimeofday(&start_time, 0) == 0);
            string request_string("data test " + int2string(i));
            string ipc_reply = chan.send_request(request_string);
            assert(gettimeofday(&end_time, 0) == 0);
            stdv_IPC_delay += pow(((((end_time.tv_sec - start_time.tv_sec)*1000000L + end_time.tv_usec) - start_time.tv_usec) - average_IPC_delay) ,2);
        }
        stdv_IPC_delay = sqrt(stdv_IPC_delay/ITERATIONS);
           
        /** Repeat this process for a function that mimics the server **/
        
        /* Generate an average */
        assert(gettimeofday(&start_time, 0) == 0);        
        for(int i=0; i<ITERATIONS; i++)
        {
            /* Function/Processing delay */
            string request_string("data test " + int2string(i));
            process_mimic(chan, request_string);
        }
        assert(gettimeofday(&end_time, 0) == 0);
        average_func_delay = (((end_time.tv_sec - start_time.tv_sec)*1000000L + end_time.tv_usec) - start_time.tv_usec)/ITERATIONS;
        
        /* Calculate the standard deviation */
        for(int i=0; i<ITERATIONS; i++)
        {
            /* Function/Processing delay */
            assert(gettimeofday(&start_time, 0) == 0);
            string request_string("data test " + int2string(i));
            process_mimic(chan, request_string);
            assert(gettimeofday(&end_time, 0) == 0);
            stdv_func_delay += pow(((((end_time.tv_sec - start_time.tv_sec)*1000000L + end_time.tv_usec) - start_time.tv_usec) - average_func_delay),2);
        }
        stdv_func_delay = sqrt(stdv_func_delay/ITERATIONS);
        
        /** Generate some comprhensive data **/
        double average_propogation_delay = average_IPC_delay - average_func_delay;
        double stdv_propogation_delay = sqrt(stdv_IPC_delay + stdv_func_delay);
        
        cout << "\nAverage Invocation Delay: " << average_IPC_delay << endl << "Standard Deviation of Invocation Delay: " << stdv_IPC_delay << endl << endl;
        cout << "Average Function Delay: " << average_func_delay << endl << "Standard Deviation of Function Delay: " << stdv_func_delay << endl << endl;
        cout << "Average Propogation Delay: " << average_propogation_delay << endl << "Standard Deviation of Propogation Delay: " << stdv_propogation_delay << endl << endl;
        
        
        string reply4 = chan.send_request("quit");
        cout << "Reply to request 'quit' is '" << reply4 << endl;

        sleep(1);
    }
    else
    {
        execl("dataserver", "", (char *)0);
    }
}
