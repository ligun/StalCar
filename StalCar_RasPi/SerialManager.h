#pragma once

#include <string.h>
#include <stdlib.h>  
#include <unistd.h>  
#include <fcntl.h>
#include <termios.h>  
#include <string>
#include <iostream>

#define BAUDRATE B9600  
#define MODEMDEVICE "/dev/ttyUSB0"
#define COULDNOTFORK -1
#define BUFSIZE 256

class SerialManager{
private:
	int fd;
	termios oldtio;
	
public:
	SerialManager(std::string device = MODEMDEVICE,int baudrate = BAUDRATE) throw(const char*);
	~SerialManager();
	std::string readLine() throw(const char*);
	void writeLine(std::string &buf) throw(const char*);
};
