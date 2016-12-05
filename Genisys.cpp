#include <Arduino.h>
#include <EthernetUdp.h>
#include "Genisys.h"
typedef void (*fn)(byte*) FuncPtr;
#ifdef C++Code
// Wrapper function uses a global to remember the object:
FPtrExceptions* object_which_will_handle_signal;
void FPtrExceptions_memberFn_wrapper()
{
  object_which_will_handle_signal->memberFn();
}
#endif

Genisys::Genisys() {
	_opcodemsg = NULL;
	_pollmsg = NULL;
}

Genisys::~Genisys() {
	if (_opcodemsg != NULL)
		delete _opcodemsg;
	if (_pollmsg != NULL) 
		delete _pollmsg;
}

void Genisys::begin (byte *localip, byte *remoteip, char *hostname) {
	for (int i = 0; i < 4; i++)
		_ip[i] = remoteip[i];
	_pollmsg = new char[60 + strlen(hostname)];
	_polllen = sprintf (_pollmsg, "POLL: HOSTNAME:%sENDHOSTNAME  HOSTIP:%d.%d.%d.%dENDHOSTIP", hostname, localip[0], localip[1], localip[2], localip[3]);
	_opcodemsg = new char[80 + strlen(hostname)];
	_opcodelen = sprintf (_opcodemsg, "RS232:xxxxxxxxENDRS232  HOSTNAME:%sENDHOSTNAME  HOSTIP:%d.%d.%d.%dENDHOSTIP", hostname, localip[0], localip[1], localip[2], localip[3]);
	_udp.begin (1001);
}

void Genisys::poll () {
	_udp.beginPacket (_ip, 1000);
	_udp.write (_pollmsg, _polllen);
	_udp.endPacket();
}

void Genisys::receiveOpCodes (FuncPtr &RcvOpcode) {
	byte opcode[8] = {0};
	while (_udp.parsePacket () > 0)
		while (_udp.available ()) {
			char buf[256] = {0};
			char *ArrayBuf = (char*)buf;
			int i = _udp.read (buf, 256);
			buf[i < 256 ? i : 255] = '\0';
			if (strlen (buf) > 36 && strncmp (buf, "RS232:", 6) == 0) {
				sscanf (&buf[11], "%2x %2x %2x %2x %2x %2x %2x %2x",
					&opcode[0], &opcode[1], &opcode[2], &opcode[3], &opcode[4], &opcode[5], &opcode[6], &opcode[7]);
				if (checkchecksum(opcode)) 
					try{
					RcvOpcode(opcode);
					}catch(...){
					trapinit();
				        signal(SIGINT,RcvOpcode);                                         
					}
			}
			delete ArrayBuf;
		}
}
#ifdef C++Code
void Genisys::receiveOpCodes () {
	byte opcode[8] = {0};
	while (_udp.parsePacket () > 0)
		while (_udp.available ()) {
			char buf[256] = {0};
			char *ArrayBuf = (char*)buf;
			int i = _udp.read (buf, 256);
			buf[i < 256 ? i : 255] = '\0';
			if (strlen (buf) > 36 && strncmp (buf, "RS232:", 6) == 0) {
				sscanf (&buf[11], "%2x %2x %2x %2x %2x %2x %2x %2x",
					&opcode[0], &opcode[1], &opcode[2], &opcode[3], &opcode[4], &opcode[5], &opcode[6], &opcode[7]);
				if (checkchecksum(opcode)) 
					try{
					RcvOpcode(opcode);
					}catch(...){
					trapinit();
				        signal(SIGINT,RcvOpcode);                                         
					}
			}
			delete ArrayBuf;
		}
signal(SIGINT,FPtrExceptions_memberFn_wrapper);  
signal(SIGINT,FPtrExceptions::staticMemberFn);   
#endif
void Genisys::sendOpCode (byte *opcode) {
	try{
	setchecksum (opcode);
	}catch(exceptions &excp){
	//Need to add the Exceptions
	
	
        }catch(...){
	delete opcode;	
	}
	for (short i = 0; i < 8; i++)
		_opcodemsg[i+6] = opcode[i];
	_udp.beginPacket (_ip, 1000);
	_udp.write (_opcodemsg, _opcodelen);
	_udp.endPacket();
}

byte Genisys::checksum (byte *opcode) {
	byte *Byteopcode = opcode;
	byte = Retval;
	try{
	Retval = (~(Byteopcode[0] + Byteopcode[1] + Byteopcode[2] + Byteopcode[3] + Byteopcode[4] + Byteopcode[5] + Byteopcode[6]) + 1) & 0xFF;
	}catch(...){
	delete Byteopcode;
	}
	return Retval; 
}
void Genisys::setchecksum (byte *opcode) { 
	byte *Byteopcode = opcode;
	try{
	Byteopcode[7] = checksum (opcode); 
	}catch(...){
	delete Byteopcode;
	}
}
bool Genisys::checkchecksum (byte *opcode) { 
	bool RetboolVal;
	byte *Byteopcode = opcode;
	        try{
		if(Byteopcode[7] == checksum (Byteopcode)){
		//Logic Required
		
		}
			
		}catch(...){
		delete Byteopcode;
		}
	return RetboolVal;
}
