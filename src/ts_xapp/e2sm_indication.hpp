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


/* Classes to handle E2 service model based on e2sm-Bouncer-v001.asn */
#ifndef SRC_XAPP_ASN_E2SM_E2SM_INDICATION_HPP_
#define SRC_XAPP_ASN_E2SM_E2SM_INDICATION_HPP_

#include <sstream>
#include "e2sm_helpers.hpp"
#include <mdclog/mdclog.h>
#include <vector>

#include "E2SM-Bouncer-IndicationHeader.h"
#include "E2SM-Bouncer-IndicationMessage.h"
#include "E2SM-Bouncer-IndicationHeader-Format1.h"
#include "E2SM-Bouncer-IndicationMessage-Format1.h"
#include "B-Header.h"
#include "B-Message.h"

class e2sm_indication {
public:
	e2sm_indication(void);
  ~e2sm_indication(void);

  bool set_fields(E2SM_Bouncer_IndicationHeader_t *, e2sm_indication_helper &);
  bool set_fields(E2SM_Bouncer_IndicationMessage_t *, e2sm_indication_helper &);

  bool get_fields(E2SM_Bouncer_IndicationHeader_t *, e2sm_indication_helper &);
  bool get_fields(E2SM_Bouncer_IndicationMessage_t *, e2sm_indication_helper &);

  bool encode_indication_header(unsigned char *, size_t *, e2sm_indication_helper &);
  bool encode_indication_message(unsigned char*, size_t *, e2sm_indication_helper &);


  std::string  get_error (void) const {return error_string ;};

private:

  E2SM_Bouncer_IndicationHeader_t * indication_head; // used for encoding
  E2SM_Bouncer_IndicationMessage_t* indication_msg;
  E2SM_Bouncer_IndicationHeader_Format1_t head_fmt1;
  E2SM_Bouncer_IndicationMessage_Format1_t msg_fmt1;


  size_t errbuf_len;
  char errbuf[128];
  std::string error_string;
};




#endif /* SRC_XAPP_ASN_E2SM_E2SM_INDICATION_HPP_ */
