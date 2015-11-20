#include "SerialManager.h"

SerialManager::SerialManager(std::string device,int baudrate) throw(const char*){
	termios tio;

	this->fd = open(device.c_str(), O_RDWR | O_NOCTTY);
	if (this->fd < 0) {  
		std::cerr << "can't open modemdevice:" << device.c_str() << std::endl;
		throw "ModemDeviceException";
	}
	
	tcgetattr(this->fd, &oldtio); /* ���݂̃V���A���|�[�g�̐ݒ��Ҕ�������*/
	memset(&tio, 0, sizeof(tio));
	
	tio.c_cflag = CS8 | CLOCAL | CREAD;
	tio.c_cc[VTIME] = 0; /* �L�����N�^�ԃ^�C�}�͖��g�p */
	tio.c_lflag = ICANON;
	tio.c_iflag = IGNPAR | ICRNL;
      
	// �{�[���[�g�̐ݒ�
	cfsetispeed(&tio, baudrate);
	cfsetospeed(&tio, baudrate);
	// �f�o�C�X�ɐݒ���s��
	tcsetattr(this->fd, TCSANOW, &tio);
}

SerialManager::~SerialManager(){
	if(this->fd > 0){
		close(this->fd);
	}
}

std::string SerialManager::readLine() throw(const char*){
	char buf[BUFSIZE];
	int count;
     
	memset(&buf, 0, sizeof(buf));  
	count = read(this->fd, &buf, BUFSIZE);  
	if (count < 0) {  
		std::cerr << "read error" << std::endl;
		throw "SerialReadException";
	}
	
	return std::string(buf);
}

void SerialManager::writeLine(std::string &buf) throw(const char*){
	int count;
	std::cout << "start write" << std::endl;
	count = write(this->fd, buf.c_str(), buf.size());
	std::cout << "send message:" << buf;  
	if (count < 0) {  
		std::cerr << "write error" << std::endl;  
		throw "SerialWriteException";  
	}
}
