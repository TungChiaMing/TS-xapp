// vi: ts=4 sw=4 noet:
/*
==================================================================================
	Copyright (c) 2020 Nokia
	Copyright (c) 2020 AT&T Intellectual Property.

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

/*
	Mnemonic:	ts_xapp.cpp
	Abstract:	Traffic Steering xApp;
	                       1. Receives A1 Policy
			       2. Queries SDL to decide which UE to attempt Traffic Steering for
			       3. Requests prediction for UE throughput on current and neighbor cells
			       4. Receives prediction
			       5. Optionally exercises Traffic Steering action over E2

	Date:		22 April 2020
	Author:		Ron Shacham
		
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <thread>
#include <iostream>
#include <memory>

#include <sdl/syncstorage.hpp>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <unordered_map>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/schema.h>
#include <rapidjson/reader.h>


#include "ricxfcpp/xapp.hpp"

#include "xapp.hpp"


using namespace rapidjson;
using namespace std;
using Namespace = std::string;
using Key = std::string;
using Data = std::vector<uint8_t>;
using DataMap = std::map<Key, Data>;
using Keys = std::set<Key>;


// ----------------------------------------------------------

std::unique_ptr<Xapp> xfw;

std::string sdl_namespace_u = "TS-UE-metrics";
std::string sdl_namespace_c = "TS-cell-metrics";

int rsrp_threshold = 0;

std::unique_ptr<shareddatalayer::SyncStorage> sdl;

Namespace nsu;
Namespace nsc;

struct UEData {
  string serving_cell;
  int serving_cell_rsrp;

};

struct PolicyHandler : public BaseReaderHandler<UTF8<>, PolicyHandler> {
  unordered_map<string, string> cell_pred;
  std::string ue_id;
  bool ue_id_found = false;
  string curr_key = "";
  string curr_value = "";
  int policy_type_id;
  int policy_instance_id;
  int threshold;
  std::string operation;
  bool found_threshold = false;

  
  bool Null() { return true; }
  bool Bool(bool b) { return true; }
  bool Int(int i) {

    if (curr_key.compare("policy_type_id") == 0) {
      policy_type_id = i;
    } else if (curr_key.compare("policy_instance_id") == 0) {
      policy_instance_id = i;
    } else if (curr_key.compare("threshold") == 0) {
      found_threshold = true;
      threshold = i;
    }

    return true;
  }
  bool Uint(unsigned u) {

    if (curr_key.compare("policy_type_id") == 0) {
      policy_type_id = u;
    } else if (curr_key.compare("policy_instance_id") == 0) {
      policy_instance_id = u;
    } else if (curr_key.compare("threshold") == 0) {
      found_threshold = true;
      threshold = u;
    }

    return true;
  }    
  bool Int64(int64_t i) {  return true; }
  bool Uint64(uint64_t u) {  return true; }
  bool Double(double d) {  return true; }
  bool String(const char* str, SizeType length, bool copy) {
    
    if (curr_key.compare("operation") != 0) {
      operation = str;
    }

    return true;
  }
  bool StartObject() {

    return true;
  }
  bool Key(const char* str, SizeType length, bool copy) {

    curr_key = str;

    return true;
  }
  bool EndObject(SizeType memberCount) {  return true; }
  bool StartArray() {  return true; }
  bool EndArray(SizeType elementCount) {  return true; }

};

struct PredictionHandler : public BaseReaderHandler<UTF8<>, PredictionHandler> {
  unordered_map<string, int> cell_pred_down;
  unordered_map<string, int> cell_pred_up;
  std::string ue_id;
  bool ue_id_found = false;
  string curr_key = "";
  string curr_value = "";
  bool down_val = true;
  bool Null() {  return true; }
  bool Bool(bool b) {  return true; }
  bool Int(int i) {  return true; }
  bool Uint(unsigned u) {    

    if (down_val) {
      cell_pred_down[curr_key] = u;
      down_val = false;
    } else {
      cell_pred_up[curr_key] = u;
      down_val = true;
    }

    return true;    

  }
  bool Int64(int64_t i) {  return true; }
  bool Uint64(uint64_t u) {  return true; }
  bool Double(double d) {  return true; }
  bool String(const char* str, SizeType length, bool copy) {

    return true;
  }
  bool StartObject() {  return true; }
  bool Key(const char* str, SizeType length, bool copy) {
    if (!ue_id_found) {

      ue_id = str;
      ue_id_found = true;
    } else {
      curr_key = str;
    }
    return true;
  }
  bool EndObject(SizeType memberCount) {  return true; }
  bool StartArray() {  return true; }
  bool EndArray(SizeType elementCount) {  return true; }
};


struct UEDataHandler : public BaseReaderHandler<UTF8<>, UEDataHandler> {
  unordered_map<string, string> cell_pred;
  std::string serving_cell_id;
  int serving_cell_rsrp;
  int serving_cell_rsrq;
  int serving_cell_sinr;
  bool in_serving_array = false;
  int rf_meas_index = 0;

  bool in_serving_report_object = false;  

  string curr_key = "";
  string curr_value = "";
  bool Null() { return true; }
  bool Bool(bool b) { return true; }
  bool Int(int i) {

    return true;
  }
  
  bool Uint(unsigned i) {

    if (in_serving_report_object) {
      if (curr_key.compare("rsrp") == 0) {
	serving_cell_rsrp = i;
      } else if (curr_key.compare("rsrq") == 0) {
	serving_cell_rsrq = i;
      } else if (curr_key.compare("rssinr") == 0) {
	serving_cell_sinr = i;
      }
    }          
    
    return true; }
  bool Int64(int64_t i) {

    return true; }
  bool Uint64(uint64_t i) {

    return true; }
  bool Double(double d) { return true; }
  bool String(const char* str, SizeType length, bool copy) {
    
    if (curr_key.compare("ServingCellID") == 0) {
      serving_cell_id = str;
    } 

    return true;
  }
  bool StartObject() {
    if (curr_key.compare("ServingCellRF") == 0) {
      in_serving_report_object = true;
    }

    return true; }
  bool Key(const char* str, SizeType length, bool copy) {
    
    curr_key = str;
    return true;
  }
  bool EndObject(SizeType memberCount) {
    if (curr_key.compare("ServingCellRF") == 0) {
      in_serving_report_object = false;
    }
    return true; }
  bool StartArray() {

    if (curr_key.compare("ServingCellRF") == 0) {
      in_serving_array = true;
    }
    
    return true;
  }
  bool EndArray(SizeType elementCount) {

    if (curr_key.compare("servingCellRF") == 0) {
      in_serving_array = false;
      rf_meas_index = 0;
    }

    return true; }
};


unordered_map<string, UEData> get_sdl_ue_data() {

  fprintf(stderr, "In get_sdl_ue_data()\n");

  unordered_map<string, string> ue_data;

  unordered_map<string, UEData> return_ue_data_map;
    
  std::string prefix3="";
  Keys K2 = sdl->findKeys(nsu, prefix3);
  DataMap Dk2 = sdl->get(nsu, K2);
  
  string ue_json;
  string ue_id;
  
  for(auto si=K2.begin();si!=K2.end();++si){
    std::vector<uint8_t> val_v = Dk2[(*si)]; // 4 lines to unpack a string
    char val[val_v.size()+1];                               // from Data
    int i;

    for(i=0;i<val_v.size();++i) val[i] = (char)(val_v[i]);
    val[i]='\0';
      ue_id.assign((std::string)*si);
      
      ue_json.assign(val);
      ue_data[ue_id] =  ue_json;
  }
  
  for (auto map_iter = ue_data.begin(); map_iter != ue_data.end(); map_iter++) {
    UEDataHandler handler;
    Reader reader;
    StringStream ss(map_iter->second.c_str());
    reader.Parse(ss,handler);

    string ueID = map_iter->first;
    string serving_cell_id = handler.serving_cell_id;
    int serv_rsrp = handler.serving_cell_rsrp;
    
    return_ue_data_map[ueID] = {serving_cell_id, serv_rsrp};
    
  }  

  return return_ue_data_map;
}

void policy_callback( Message& mbuf, int mtype, int subid, int len, Msg_component payload,  void* data ) {

  int response_to = 0;	 // max timeout wating for a response
  int rmtype;		// received message type

  
  fprintf(stderr, "Policy Callback got a message, type=%d, length=%d\n", mtype, len);

  const char *arg = (const char*)payload.get();

  fprintf(stderr, "payload is %s\n", payload.get());

  PolicyHandler handler;
  Reader reader;
  StringStream ss(arg);
  reader.Parse(ss,handler);

  //Set the threshold value

  if (handler.found_threshold) {
    fprintf(stderr, "Setting RSRP Threshold to A1-P value: %d\n", handler.threshold);
    rsrp_threshold = handler.threshold;
  }

  mbuf.Send_response( 101, -1, 5, (unsigned char *) "OK1\n" );	// validate that we can use the same buffer for 2 rts calls
  mbuf.Send_response( 101, -1, 5, (unsigned char *) "OK2\n" );
  
  
}

void send_prediction_request(vector<string> ues_to_predict) {

  std::unique_ptr<Message> msg;
  Msg_component payload;                                // special type of unique pointer to the payload
  
  int nthreads = 1;  
  int response_to = 0;   // max timeout wating for a response
  int mtype = 30000;
  int sz;
  int i;
  Msg_component send_payload;
  
  msg = xfw->Alloc_msg( 2048 );
  
  sz = msg->Get_available_size();  // we'll reuse a message if we received one back; ensure it's big enough
  if( sz < 2048 ) {
    fprintf( stderr, "<SNDR> fail: message returned did not have enough size: %d [%d]\n", sz, i );
    exit( 1 );
  }

  string ues_list = "[";

  for (int i = 0; i < ues_to_predict.size(); i++) {
    if (i == ues_to_predict.size() - 1) {
      ues_list = ues_list + " \"" + ues_to_predict.at(i) + "\"]";
    } else {
      ues_list = ues_list + " \"" + ues_to_predict.at(i) + "\"" + ",";
    }
  }

  string message_body = "{\"UEPredictionSet\": " + ues_list + "}";

  const char *body = message_body.c_str();

  //  char *body = "{\"UEPredictionSet\": [\"12345\"]}";
  
  send_payload = msg->Get_payload(); // direct access to payload
  //  snprintf( (char *) send_payload.get(), 2048, '{"UEPredictionSet" : ["12345"]}', 1 );
  snprintf( (char *) send_payload.get(), 2048, body);
  //snprintf( (char *) send_payload.get(), 2048, "{\"UEPredictionSet\": [\"12345\"]}");

  fprintf(stderr, "message body %s\n", send_payload.get());  
  fprintf(stderr, "payload length %d\n", strlen( (char *) send_payload.get() ));
  
  // payload updated in place, nothing to copy from, so payload parm is nil
  if ( ! msg->Send_msg( mtype, Message::NO_SUBID, strlen( (char *) send_payload.get() ), NULL )) {
    fprintf( stderr, "<SNDR> send failed: %d\n", msg->Get_state() );
  }

  /*
  msg = xfw->Receive( response_to );
  if( msg != NULL ) {
    rmtype = msg->Get_mtype();
    send_payload = msg->Get_payload();
    fprintf( stderr, "got: mtype=%d payload=(%s)\n", rmtype, (char *) send_payload.get() );
  } 
  */

}

void prediction_callback( Message& mbuf, int mtype, int subid, int len, Msg_component payload,  void* data ) {

  long now;
  long total_count;

  int sz;
  int i;

  int response_to = 0;	 // max timeout wating for a response

  int send_mtype = 0;
  int rmtype;							// received message type
  int delay = 1000000;				// mu-sec delay; default 1s
  std::unique_ptr<Message> msg;
  Msg_component send_payload;
  
  cout << "Prediction Callback got a message, type=" << mtype << " , length=" << len << "\n";
  cout << "payload is " << payload.get() << "\n";

  mtype = 0;

  const char* arg = (const char*)payload.get();
  PredictionHandler handler;

  try {

    Reader reader;
    StringStream ss(arg);
    reader.Parse(ss,handler);
  } catch (...) {
    cout << "got an exception on stringstream read parse\n";
  }
  
  std::string pred_ue_id = handler.ue_id;
  
  cout << "Prediction for " << pred_ue_id << endl;
  
  unordered_map<string, int> throughput_map = handler.cell_pred_down;

  cout << endl;
 
  unordered_map<string, UEData> sdl_data = get_sdl_ue_data();

  //Decision about CONTROL message
  //(1) Identify UE Id in Prediction message
  //(2) Get UEData struct for this UE Id
  //(3) Identify the UE's service cell ID
  //(4) Iterate through Prediction message.
  //    If one of the cells, have a higher throughput prediction than serving cell, log a CONTROL request

  UEData pred_ue_data = sdl_data[pred_ue_id];
  std::string serving_cell_id = pred_ue_data.serving_cell;

  int serving_cell_throughput;
  int highest_throughput;
  std::string highest_throughput_cell_id;
  std::string::size_type str_size;

  for (auto map_iter = throughput_map.begin(); map_iter != throughput_map.end(); map_iter++) {

    std::string curr_cellid = map_iter->first;
    int curr_throughput = map_iter->second;

    if (curr_cellid.compare(serving_cell_id) == 0) {
      serving_cell_throughput = curr_throughput;
      highest_throughput = serving_cell_throughput;
    }

  }

  //Iterating again to identify the highest throughput prediction

  for (auto map_iter = throughput_map.begin(); map_iter != throughput_map.end(); map_iter++) {

    std::string curr_cellid = map_iter->first;
    int curr_throughput = map_iter->second;

    if (curr_throughput > serving_cell_throughput) {
      highest_throughput = curr_throughput;
      highest_throughput_cell_id = curr_cellid;
    }
  }

  if (highest_throughput > serving_cell_throughput) {
    cout << "WE WOULD SEND A CONTROL REQUEST NOW" << endl;
    cout << "UE ID: " << pred_ue_id << endl;
    cout << "Source cell " << serving_cell_id << endl;
    cout << "Target cell " << highest_throughput_cell_id << endl;
  }

  mbuf.Send_response( 101, -1, 5, (unsigned char *) "OK1\n" );	// validate that we can use the same buffer for 2 rts calls
  mbuf.Send_response( 101, -1, 5, (unsigned char *) "OK2\n" );
  /*CEI*/  
  msg = xfw->Alloc_msg( 20480 );
  sz = msg->Get_available_size(); 
  
  const unsigned char message_body1[] = 
  {0x00,0x08,0x00,0x28,0x00,0x00,0x03,0x00,0x1d,0x00,0x05,0x00,0x03,0xe9,0x03,0xe9,
   0x00,0x05,0x00,0x02,0x00,0x00,0x00,0x1e,
   0x00,0x12,0x00,0x08,0x20,0x34,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x13,0x00,
   0x03,0x00,0x00,0x00};
  
  //string Ken_meid = "&{%!d(string=373437) %!d(string=10110101110001100111011110001) %!d(string=gnb_734_733_b5c67788)}";	
  //const char *msg_buf = message_body1.c_str();
  //char *body = "{\"CEU_SEND_TS\"}";
  
  //const char *msg_buf = message_body1.c_str();
 
     
  send_payload = msg->Get_payload(); // direct access to payload
  //send_meid = msg->Get_meid(); // direct access to meid
  //strcpy((char*)send_meid.get(), "Ken_Meid");
  memcpy((char*)send_payload.get(), message_body1, 44);
 
  
  //cout << send_meid.get() << "\n" ;
  
  //strcpy((string*)send_payload.get(), Ken_meid, Ken_meid.size()); 
  //snprintf( (char *) send_payload.get(), 2048, "{\"UEPredictionSet\": [\"12345\"]}");
  fprintf(stderr, "message msg_buf %s\n", send_payload.get());  
  //fprintf(stderr, "payload length %d\n", sizeof( (char *) send_payload.get() ));
  // payload updated in place, nothing to copy from, so payload parm is nil
  if ( ! msg->Send_msg( 12010, 1002, 44, NULL )) {
    fprintf( stderr, "<SNDR> send failed: %d\n", msg->Get_state() );
  }else{
    fprintf( stderr, "<SNDR> send successful[SUB_REQ]\n");
  }  
  
}


//This function runs a loop that continuously checks SDL for any UE

void run_loop() {

  cout << "in Traffic Steering run_loop()\n";

  unordered_map<string, UEData> uemap;

  while (1) {

    uemap = get_sdl_ue_data();

    vector<string> prediction_ues;

    for (auto map_iter = uemap.begin(); map_iter != uemap.end(); map_iter++) {
      string ueid = map_iter->first;
      fprintf(stderr,"found a ueid %s\n", ueid.c_str());
      UEData data = map_iter->second;

      fprintf(stderr, "current rsrp is %d\n", data.serving_cell_rsrp);

      if (data.serving_cell_rsrp < rsrp_threshold) {
	fprintf(stderr,"it is less than the rsrp threshold\n");
	prediction_ues.push_back(ueid);
      } else {
	fprintf(stderr,"it is not less than the rsrp threshold\n");
      }
    }

    fprintf(stderr, "the size of pred ues is %d\n", prediction_ues.size());

    if (prediction_ues.size() > 0) {
      send_prediction_request(prediction_ues);
    }

    sleep(20);
  }
}

/* This function works with Anomaly Detection(AD) xApp. It is invoked when anomalous UEs are send by AD xApp.
 * It just print the payload received from AD xApp and send an ACK with same UEID as payload to AD xApp.
 */
void ad_callback( Message& mbuf, int mtype, int subid, int len, Msg_component payload,  void* data ) {
  cout << "payload is " << payload.get() << "\n";
  mbuf.Send_response(30004, -1, strlen((char *) payload.get()), (unsigned char *) payload.get());
}


void signalHandler( int signum ) {
   cout << "Interrupt signal (" << signum << ") received.\n";
   exit(signum);
}
/*
void subscribe_requests(void ){

   bool res;
   size_t data_size = ASN_BUFF_MAX_SIZE;
   unsigned char	data[data_size];
   unsigned char meid[RMR_MAX_MEID];
   std::string xapp_id = config_ref->operator [](XappSettings::SettingName::XAPP_ID);

   mdclog_write(MDCLOG_INFO,"Preparing to send subscription in file= %s, line=%d",__FILE__,__LINE__);

   //auto gnblist = get_rnib_gnblist();

   //int sz = gnblist.size();

   //if(sz <= 0)
   //mdclog_write(MDCLOG_INFO,"Subscriptions cannot be sent as GNBList in RNIB is NULL");

   for(int i = 0; i<1; i++)
   {
	 sleep(15); 
	 //give the message to subscription handler, along with the transmitter.
	 strcpy((char*)meid,"gnb_734_733_b5c67788");

     //mdclog_write(MDCLOG_INFO,"GNBList size : %d", sz);

	 subscription_helper  din;
	 subscription_helper  dout;

	 subscription_request sub_req;
	 subscription_request sub_recv;

	 unsigned char buf[BUFFER_SIZE];
	 size_t buf_size = BUFFER_SIZE;
	 bool res;


	 //Random Data  for request
	 int request_id = 1;
	 int function_id = 0;
	 std::string event_def = "01";

	 din.set_request(request_id);
	 din.set_function_id(function_id);
	 din.set_event_def(event_def.c_str(), event_def.length());

	 std::string act_def = "01";

	 din.add_action(1,0,(void*)act_def.c_str(), act_def.length(), 0);

	 res = sub_req.encode_e2ap_subscription(&buf[0], &buf_size, din);

 	 //mdclog_write(MDCLOG_INFO,"GNBList = %s and ith val = %d", gnblist[i], i);

	 mdclog_write(MDCLOG_INFO,"Sending subscription in file= %s, line=%d for MEID %s",__FILE__,__LINE__, meid);
	 
	 xapp_rmr_header rmr_header;
 	 rmr_header.message_type = RIC_SUB_REQ;
 	 rmr_header.payload_length = buf_size; //data_size

	 strcpy((char*)rmr_header.meid,"gnb_734_733_b5c67788");

	 auto transmitter = std::bind(&XappRmr::xapp_rmr_send,rmr_ref, &rmr_header, (void*)buf); //(void*)data);
         
         int result = subhandler_ref->manage_subscription_request("gnb_734_733_b5c67788", transmitter);
         
       	 if(result==SUBSCR_SUCCESS){

     	      mdclog_write(MDCLOG_INFO,"Subscription SUCCESSFUL in file= %s, line=%d for MEID %s",__FILE__,__LINE__, meid);
          }
          else {
		 mdclog_write(MDCLOG_ERR,"Subscription FAILED in file= %s, line=%d for MEID %s",__FILE__,__LINE__, meid);
              }  
	} 
}

*/
/*
void begin_sub_req(SubscriptionHandler &sub_ref) {

	subhandler_ref = &sub_ref;
	

	sleep(100);


	subscribe_requests();


	return;
}
*/

extern int main( int argc, char** argv ) {


  
  int nthreads = 1;

  //char*	port = (char *) "4560";

  sdl = shareddatalayer::SyncStorage::create();

  nsu = Namespace(sdl_namespace_u);
  nsc = Namespace(sdl_namespace_c);
  
  /* Send E2 Subscription */

  // Get the thread id
  std::thread::id my_id = std::this_thread::get_id();
  std::stringstream thread_id;
  std::stringstream ss;

  thread_id << my_id;

  mdclog_write(MDCLOG_INFO, "Starting thread %s",  thread_id.str().c_str());

  //get configuration
  XappSettings config;
  //change the priority depending upon application requirement
  config.loadDefaultSettings();
  config.loadEnvVarSettings();
  config.loadCmdlineSettings(argc, argv);

  //Register signal handler to stop
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);

  //getting the listening port and xapp name info
  std::string  port_b = config[XappSettings::SettingName::BOUNCER_PORT];
  std::string  name = config[XappSettings::SettingName::XAPP_NAME];

  //initialize rmr
  std::unique_ptr<XappRmr> rmr = std::make_unique<XappRmr>(port_b);
  rmr->xapp_rmr_init(true);


  //Create Subscription Handler if Xapp deals with Subscription.
  //std::unique_ptr<SubscriptionHandler> sub_handler = std::make_unique<SubscriptionHandler>();

  SubscriptionHandler sub_handler;

  //create Bouncer Xapp Instance.
  std::unique_ptr<Xapp_b> b_xapp;
  b_xapp = std::make_unique<Xapp_b>(std::ref(config),std::ref(*rmr));

  mdclog_write(MDCLOG_INFO, "Created TS Xapp Instance");
  //Startup E2 subscription
  b_xapp->startup(sub_handler);
  int num_threads = std::stoi(config[XappSettings::SettingName::THREADS]);
  mdclog_write(MDCLOG_INFO, "Starting Listener Threads. Number of Workers = %d", num_threads);

  std::unique_ptr<XappMsgHandler> mp_handler = std::make_unique<XappMsgHandler>(config[XappSettings::SettingName::XAPP_ID], sub_handler);

  b_xapp->start_xapp_receiver(std::ref(*mp_handler));

  sleep(1);



  while(1){
	sleep(1);
  }

  return 0;
  /*
  fprintf( stderr, "<XAPP> listening on port: %s\n", port );
  xfw = std::unique_ptr<Xapp>( new Xapp( port, true ) ); 
  
  xfw->Add_msg_cb( 20010, policy_callback, NULL );
  xfw->Add_msg_cb( 30002, prediction_callback, NULL );
  xfw->Add_msg_cb( 30003, ad_callback, NULL ); 
  
  std::thread loop_thread;

  loop_thread = std::thread(&run_loop);

  xfw->Run( nthreads );
  */
}
