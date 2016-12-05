#ifndef _REGISTER_FILE_H
#define _REGISTER_FILE_H

#include "param.h"

class RegisterFile
{
public:
	RegisterFile();
	~RegisterFile();

	void setPC(lint newPC);
	void setInteger(int index, lint value);
	void setFloat(int index, lint value);
	void setFSR(lint value);

	lint getPC();
	lint getInteger(int index);
	lint getFloat(int index);
	lint getFSR();
	
private:
	lint pc;
	lint x[32];
	lint f[32];
	lint fcsr;
};

#endif