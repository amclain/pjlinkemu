/** 
 * \class Projector
 */

#include "Projector.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <regex>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "UI.h"

using namespace std;

Projector *Projector::_instance = NULL;

Projector *Projector::getInstance() {
    if (_instance == NULL) {
        _instance = new Projector();
    }
    return _instance;
}

Projector::Projector() {
    _emulateHangOpenBug = false;
    _PJLinkUseAuthentication = false;
    
    _PJLinkPower = POWER_OFF;
    _PJLinkInput = 11;
    _PJLinkAVMute = 30;
    _PJLinkError = 0;
    _PJLinkLampHours = 0;
    
    _PJLinkName = "Emulator";
    
    
    _socketListener = NULL;
    _socketTimer = NULL;
    
    _isListening = false;
    _isConnected = false;
    _serverfd = -1;
    _clientfd = -1;
    
    _clientConnected = 0;
    _clientTimeout = 0;
    
    bzero(&_serveraddr, sizeof(sockaddr_in));
    bzero(&_clientaddr, sizeof(sockaddr_in));
    
    _ui = UI::getInstance();
}

Projector::~Projector() {
    if (_isListening == true) this->close();
}

Projector::Projector(const Projector &orig) {
}

const Projector &Projector::operator=(const Projector &orig) {
}

int Projector::getPowerState() {
    return _PJLinkPower;
}

void Projector::setPowerState(int value) {
    _PJLinkPower = value;
}

int Projector::getInputState() {
    return _PJLinkInput;
}

void Projector::setInputState(int value) {
    _PJLinkInput = value;
}

int Projector::getAVMuteState() {
    return _PJLinkAVMute;
}

void Projector::setAVMuteState(int value) {
    _PJLinkAVMute = value;
}

int Projector::getErrorState() {
    return _PJLinkError;
}

void Projector::setErrorState(int value) {
    _PJLinkError = value;
}

int Projector::getLampHours() {
    return _PJLinkLampHours;
}

void Projector::setLampHours(int value) {
    _PJLinkLampHours = value;
}

bool Projector::isListening() {
    return _isListening;
}

bool Projector::isConnected() {
    return _isConnected;
}

void Projector::close() {
    _isConnected = false;
    _isListening = false;
    
    ::close(_clientfd);
    ::close(_serverfd);
    
    fclose(_sin);
    fclose(_sout);
    
    _socketListener->join();
    delete _socketListener;
}

void Projector::closeClient() {
    _isConnected = false;
    
    ::close(_clientfd);
    
    fclose(_sin);
    fclose(_sout);
}

void Projector::listen() {
    listen(4352);
}

void Projector::listen(int port) {
    if (_isListening == true) return;
    
    _serveraddr.sin_family = AF_INET;
    _serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    _serveraddr.sin_port = htons(_port);
    
    _serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverfd < 0) {
        _ui->print(string("Failed to create socket."));
        return;
    }
    
    if (bind(_serverfd, (sockaddr *) &_serveraddr, sizeof(sockaddr_in)) < 0) {
        _ui->print(string("Failed to bind socket."));
        return;
    }
    
    if (::listen(_serverfd, SOMAXCONN) < 0) {
        _ui->print(string("Failed to listen on socket."));
        return;
    }
    
    _isListening = true;
    
    // TODO: Loop goes here. //////////////////////////////////////////////////////////////////////////
    _socketListener = new thread(&Projector::accept, this);
}

void Projector::accept() {
    if (_isConnected == true) return;
    
    bzero(&_clientaddr, sizeof(_clientaddr));
    
    // Only accept one connection.
    _clientfd = ::accept(_serverfd, (sockaddr *) &_clientaddr, &_clientlen);
    if (_clientfd < 0) {
        _ui->print(string("Failed to accept client."));
        return;
    }
    else {
        _clientConnected = time(NULL);
        
        // Configure client socket to blocking.
        int sflags = fcntl(_clientfd, F_GETFL, 0);
        fcntl(_clientfd, F_SETFL, sflags & ~O_NONBLOCK);
        
        // Set TCP_NODELAY
        int tcpVal = 1;
        setsockopt(_clientfd, IPPROTO_TCP, TCP_NODELAY, &tcpVal, sizeof (int));
        
        _sin = fdopen(_clientfd, "r");
        _sout = fdopen(_clientfd, "w");
        
        // Set the fd buffers to line buffered (default is fully buffered).
        setvbuf(_sin, NULL, _IONBF, -1);
        setvbuf(_sout, NULL, _IOLBF, -1);
        
        _isConnected = true;
        
        resetSocketTimeout();
        
        // Acknowledge client connection.
        if (_PJLinkUseAuthentication == true) {

        }
        else {
            fprintf(_sout, "PJLINK 0\r\n");
            fflush(_sout);
        }
        
        while (_isConnected == true) {
            doNetworking();
        }
    }
}

void Projector::doNetworking() {
    if (_isConnected == false) return;
    if (_sin == NULL) return;
    
    // Read command.
    char rbuf[1024]; // Read buffer.
    bzero (rbuf, sizeof(rbuf));
    
    ssize_t readResult = read(_clientfd, rbuf, sizeof(rbuf));
    if (readResult <= -1) {
        // Read error.
        return;
    }
    else if (readResult == 0) {
        // EOF
        _isConnected = false;
        return;
    }
    
    resetSocketTimeout();
    
    string received = string(rbuf);
    string response = string("ERR1"); // Undefined PJLink command.
    
    // Check for command and parameter.
    smatch results;
    bool found = regex_match(received, results, regex("%1(\\w+)\\ (\\?|[\\d]*)[\\r\\n]"));
    
    string command = results.str(1);
    string value = results.str(2);
    
    // DEBUG ////////////////////////////////////////////////////////////////////////////////
    _ui->print(string(command + " " + value));
    
    
    // Acknowledge command.
    if (command.compare("POWR") == 0) {
        if (value.compare("1")) {
            _PJLinkPower = POWER_ON;
            response = string("%1POWR=OK");
        }
        else if (value.compare("0") == 0) {
            _PJLinkPower = POWER_OFF;
            response = string("%1POWR=OK\r\n");
        }
        else if (value.compare("?") == 0) {
            switch (_PJLinkPower) {
                case POWER_ON:      response = string("%1POWR=1");  break;
                case POWER_COOLING: response = string("%1POWR=2");  break;
                case POWER_WARMING: response = string("%1POWR=3");  break;
                case POWER_OFF:
                default:            response = string("%1POWR=0");  break;
            }
        }
        // Power instruction out of parameter.
        else if (received.find("%1POWR ")) {
            response = string("%1POWR=ERR2");
        }
    }
    
    fprintf(_sout, "%s\r\n", response.c_str());
    fflush(_sout);
    
    if (response.find("ERR") != string::npos) _ui->refresh();
}

void Projector::resetSocketTimeout() {
    // Set timeout.
    if (_emulateHangOpenBug == true) {
        _clientTimeout = 0;
    }
    else {
        _clientTimeout = time(NULL) + 30;
    }
}
