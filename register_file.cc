#include "register_file.h"

RegisterFile::RegisterFile()
{

}

RegisterFile::~RegisterFile()
{

}

void RegisterFile::setPC(lint newPC)
{
	pc = newPC;
}

void RegisterFile::setInteger(int index, lint value)
{
	if(index > 0 && index < 32)
	{
		x[index] = value;
	}
}

void RegisterFile::setFloat(int index, lint value)
{
	if(index >= 0 && index < 32)
	{
		f[index] = value;
	}
}

void RegisterFile::setFSR(lint value)
{
	fcsr = value;
}

lint RegisterFile::getPC()
{
	return pc;
}

lint RegisterFile::getInteger(int index)
{
	if(index >= 0 && index < 32)
	{
		return x[index];
	}
	return 0;
}

lint RegisterFile::getFloat(int index)
{
	if(index >= 0 && index < 32)
	{
		return f[index];
	}
	return 0;
}

lint RegisterFile::getFSR()
{
	return fcsr;
}