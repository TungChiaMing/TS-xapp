# vim: ts=4 sw=4 noet:

#==================================================================================
#	Copyright (c) 2020 Nokia
#	Copyright (c) 2020 AT&T Intellectual Property.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#==================================================================================

# simple makefile to build the examples. This assumes that the xapp framework
# library has been installed or the LD_LIBRARY_PATH and C_INCLUDE_PATH environent
# variables are set to reference the needed files.


####### Logging library and flags
CLOGFLAGS:= `pkg-config mdclog --cflags`
LOG_LIBS:= `pkg-config mdclog --libs`
CURL_LIBS:= `pkg-config libcurl --libs`
RNIB_LIBS:= -pthread /usr/local/include/rnib/rnibreader.a
######## Keep include dirs separate so we have transparency

BASEFLAGS=  -Wall -std=c++14 $(CLOGFLAGS) 
C_BASEFLAGS= -Wall $(CLOGFLAGS) -DASN_DISABLE_OER_SUPPORT


%.o:: %.cpp %.hpp 
	g++ -g ${prereq%% *} -c 

LIBS = $(LOG_LIBS) $(CURL_LIBS) $(RNIB_LIBS)

% :: %.cpp
	g++ $< -g -o $@ -lricxfcpp -lrmr_si -lpthread -lm -lsdl $(LIBS)
  
all:: ts_xapp

install::
	cp ts_xapp /usr/local/bin/
