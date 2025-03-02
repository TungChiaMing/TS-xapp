/*
==================================================================================

        Copyright (c) 2019-2020 AT&T Intellectual Property.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
==================================================================================
*/

#ifndef XAPP_RMR_XAPP_RMR_H_
#define XAPP_RMR_XAPP_RMR_H_


#ifdef __GNUC__
#define likely(x)  __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <error.h>
#include "assert.h"
#include <sstream>
#include <thread>
#include <functional>
#include <map>
#include <mutex>
#include <sys/epoll.h>
#include <rmr/rmr.h>
#include <rmr/RIC_message_types.h>
#include <mdclog/mdclog.h>
#include <vector>
#include <ctime>
#include <chrono>
#include <sys/time.h>

#include "a1_helper.hpp"
#include "e2ap_control.hpp"
#include "e2ap_control_response.hpp"
#include "e2ap_indication.hpp"
#include "subscription_delete_request.hpp"
#include "subscription_delete_response.hpp"
#include "subscription_helper.hpp"
#include "subscription_request.hpp"
#include "subscription_request.hpp"
#include "subscription_response.hpp"
#include "e2sm_subscription.hpp"
#include "subs_mgmt.hpp"

typedef struct{
	struct timespec ts;
	int32_t message_type;
	int32_t state;
	int32_t payload_length;
	
	unsigned char sid[RMR_MAX_SID]; //Subscription ID.
	unsigned char src[RMR_MAX_SRC]; //Xapp Name
	unsigned char meid[RMR_MAX_MEID]={};

}  xapp_rmr_header;


class XappRmr{
private:
	std::string _proto_port;
	int _nattempts;
	bool _rmr_is_ready;
    bool _listen;
	void* _xapp_rmr_ctx;
	rmr_mbuf_t*		_xapp_send_buff;					// send buffer
	rmr_mbuf_t*		_xapp_received_buff;					// received buffer


public:

	XappRmr(std::string, int rmrattempts=10);
	~XappRmr(void);
	void xapp_rmr_init(bool);

	template <class MessageProcessor>
	void xapp_rmr_receive(MessageProcessor&&, XappRmr *parent);

	bool xapp_rmr_send(xapp_rmr_header*, void*);

	bool rmr_header(xapp_rmr_header*);
	void set_listen(bool);
	bool get_listen(void);
	int get_is_ready(void);
	bool get_isRunning(void);
	void* get_rmr_context(void);

};


// main workhorse thread which does the listen->process->respond loop
template <class MsgHandler>
void XappRmr::xapp_rmr_receive(MsgHandler&& msgproc, XappRmr *parent){

	bool* resend = new bool(false);
	// Get the thread id
	std::thread::id my_id = std::this_thread::get_id();
	std::stringstream thread_id;
	std::stringstream ss;
	std::fstream io_file;

	thread_id << my_id;

	// Get the rmr context from parent (all threads and parent use same rmr context. rmr context is expected to be thread safe)
	if(!parent->get_is_ready()){
			mdclog_write( MDCLOG_ERR, "RMR Shows Not Ready in RECEIVER, file= %s, line=%d ",__FILE__,__LINE__);
			return;
	}
	void *rmr_context = parent->get_rmr_context();
	assert(rmr_context != NULL);

	// Get buffer specific to this thread
	this->_xapp_received_buff = NULL;
	this->_xapp_received_buff = rmr_alloc_msg(rmr_context, RMR_DEF_SIZE);
	assert(this->_xapp_received_buff != NULL);

	mdclog_write(MDCLOG_INFO, "Starting receiver thread %s",  thread_id.str().c_str());
	io_file.open("/tmp/timestamp.txt", std::ios::in|std::ios::out|std::ios::app);
        std::time_t sentMsg_time;
        std::time_t recvMsg_time;
        struct timeval ts_recv;
        struct timeval ts_sent;
        int num = 0;

	while(parent->get_listen()) {
		mdclog_write(MDCLOG_INFO, "Listening at Thread: %s",  thread_id.str().c_str());

		this->_xapp_received_buff = rmr_rcv_msg( rmr_context, this->_xapp_received_buff );
		//this->_xapp_received_buff = rmr_call( rmr_context, this->_xapp_received_buff);
		
                if (io_file) {
                        gettimeofday(&ts_recv, NULL);
                        io_file << "Received Msg with msgType: " << this->_xapp_received_buff->mtype << " at time: " <<  (ts_recv.tv_sec * 1000) + (ts_recv.tv_usec/1000) << std::endl;
                }

		if( this->_xapp_received_buff->mtype < 0 || this->_xapp_received_buff->state != RMR_OK ) {
			mdclog_write(MDCLOG_ERR, "bad msg:  state=%d  errno=%d, file= %s, line=%d", this->_xapp_received_buff->state, errno, __FILE__,__LINE__ );
			return;
		}
		else
		{
			mdclog_write(MDCLOG_INFO,"RMR Received Message of Type: %d",this->_xapp_received_buff->mtype);
			mdclog_write(MDCLOG_INFO,"RMR Received Message: %s",(char*)this->_xapp_received_buff->payload);

		    //in case message handler returns true, need to resend the message.
			msgproc(this->_xapp_received_buff, resend);

			//start of code to check decoding indication payload

                         num++;
                         mdclog_write(MDCLOG_DEBUG, "Total Indications received : %d", num);

			if(*resend){
				mdclog_write(MDCLOG_INFO,"RMR Return to Sender Message of Type: %d",this->_xapp_received_buff->mtype);
				mdclog_write(MDCLOG_INFO,"RMR Return to Sender Message: %s",(char*)this->_xapp_received_buff->payload);
				
				if (io_file) {
                                        gettimeofday(&ts_sent, NULL);
										
                                        io_file << "Send Msg with msgType: " << this->_xapp_received_buff->mtype << " at time: " << (ts_sent.tv_sec * 1000) + (ts_sent.tv_usec/1000) << std::endl;

                                        io_file << "Time diff: " << ((ts_sent.tv_sec - ts_recv.tv_sec)*1000 + (ts_sent.tv_usec - ts_recv.tv_usec)/1000) << std::endl;
                                }

				rmr_rts_msg(rmr_context, this->_xapp_received_buff );
				//sleep(1);

				*resend = false;
			}
			continue;
		}

	}

	if (io_file) {
                io_file.close();
        }

	// Clean up
	try{
		delete resend;
		rmr_free_msg(this->_xapp_received_buff);
	}
	catch(std::runtime_error &e){
		std::string identifier = __FILE__ +  std::string(", Line: ") + std::to_string(__LINE__) ;
		std::string error_string = identifier = " Error freeing RMR message ";
		mdclog_write(MDCLOG_ERR, error_string.c_str(), "");
	}

	return;
}

#endif /* XAPP_RMR_XAPP_RMR_H_ */
