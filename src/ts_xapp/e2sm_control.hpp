/*
# ==================================================================================
# Copyright (c) 2020 HCL Technologies Limited.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==================================================================================
*/


/* Classes to handle E2 service model based on Bouncer-v001.asn */
#ifndef SRC_XAPP_ASN_E2SM_E2SM_CONTROL_HPP_
#define SRC_XAPP_ASN_E2SM_E2SM_CONTROL_HPP_


#include <sstream>
#include "e2sm_helpers.hpp"
#include <mdclog/mdclog.h>
#include <vector>

#include "E2SM-Bouncer-ControlHeader.h"
#include "E2SM-Bouncer-ControlMessage.h"
#include "E2SM-Bouncer-ControlHeader-Format1.h"
#include "E2SM-Bouncer-ControlMessage-Format1.h"
#include "B-Header.h"
#include "B-Message.h"
class e2sm_control {
public:
	e2sm_control(void);
  ~e2sm_control(void);

  bool set_fields(E2SM_Bouncer_ControlHeader_t *, e2sm_control_helper &);
  bool set_fields(E2SM_Bouncer_ControlMessage_t *, e2sm_control_helper &);

  bool get_fields(E2SM_Bouncer_ControlHeader_t *, e2sm_control_helper &);
  bool get_fields(E2SM_Bouncer_ControlMessage_t *, e2sm_control_helper &);

  bool encode_control_header(unsigned char *, size_t *, e2sm_control_helper &);
  bool encode_control_message(unsigned char*, size_t *, e2sm_control_helper &);


  std::string  get_error (void) const {return error_string ;};

private:

  E2SM_Bouncer_ControlHeader_t * control_head; // used for encoding
  E2SM_Bouncer_ControlMessage_t* control_msg;
  E2SM_Bouncer_ControlHeader_Format1_t head_fmt1;
  E2SM_Bouncer_ControlMessage_Format1_t msg_fmt1;


  size_t errbuf_len;
  char errbuf[128];
  std::string error_string;
};



#endif /* SRC_XAPP_ASN_E2SM_E2SM_CONTROL_HPP_ */
