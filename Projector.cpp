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
#include <regex.h>
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
    
    ::shutdown(_clientfd, SHUT_RDWR);
    ::shutdown(_serverfd, SHUT_RDWR);
    
    // TODO: Null checks.
    fclose(_sin);
    fclose(_sout);
    
    _socketListener->join();
    delete _socketListener;
}

void Projector::closeClient() {
    _isConnected = false;
    
    ::shutdown(_clientfd, SHUT_RDWR);
    
    fclose(_sin);
    fclose(_sout);
}

void Projector::listen() {
    listen(4352);
}

void Projector::listen(int port) {
    if (_isListening == true) return;
    
    _port = port;
    
    _serveraddr.sin_family = AF_INET;
    _serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    _serveraddr.sin_port = htons(_port);
    
    _serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverfd < 0) {
        _ui->print("Failed to create socket.");
        return;
    }
    
    if (bind(_serverfd, (sockaddr *) &_serveraddr, sizeof(_serveraddr)) < 0) {
        _ui->print("Failed to bind socket.");
        return;
    }
    
    if (::listen(_serverfd, SOMAXCONN) < 0) {
        _ui->print("Failed to listen on socket.");
        return;
    }
    
    _isListening = true;
    
    _socketListener = new thread(&Projector::doAccept, this);
}

void Projector::doAccept() {
    while (_isListening == true) {
        this->accept();
    }
}

void Projector::accept() {
    if (_isConnected == true) return;
    
    bzero(&_clientaddr, sizeof(_clientaddr));
    
    // Only accept one connection.
    _clientfd = ::accept(_serverfd, (sockaddr *) &_clientaddr, &_clientlen);
    if (_clientfd < 0) {
        _ui->print("Failed to accept client.");
        return;
    }
    else {
        _clientConnected = time(NULL);
        _ui->print("Client connected.");
        
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
        string greeting = "PJLINK 0";
        
        if (_PJLinkUseAuthentication == true) {
            // TODO: Implement PJLink connection challenge.
            //       PJLink spec p23.
        }
        fprintf(_sout, "%s\r\n", greeting.c_str());
        fflush(_sout);
        
        _ui->print("> " + greeting);
        
        while (_isConnected == true) {
            doRead();
        }
    }
}

void Projector::doRead() {
    if (_isConnected == false) return;
    if (_sin == NULL) return;
    
    // Read command.
    char rbuf[1024]; // Read buffer.
    bzero (rbuf, sizeof(rbuf));
    
    ssize_t readResult = read(_clientfd, rbuf, sizeof(rbuf));
    if (readResult <= -1) {
        // Read error.
        _isConnected = false;
        _ui->print("Read error.");
        _ui->refresh();
        return;
    }
    else if (readResult == 0) {
        // EOF
        _isConnected = false;
        _ui->refresh();
        return;
    }
    
    resetSocketTimeout();
    
    string received(rbuf);
    
    // Check for command and parameter.
    regex_t regex;
    int result = 0;
    size_t nmatches = 3;
    regmatch_t pmatch[3];
    
    result = regcomp(&regex, "%1(\\w+)\\ (\\?|[0-9]+)\\r?\\n?", REG_EXTENDED);
    if (result != 0) {
        _ui->print("Error compiling regex.");
    }
    else {
        result = regexec(&regex, received.c_str(), nmatches, pmatch, REG_EXTENDED);
    }
    
    string command = (result == 0 && pmatch[1].rm_so >= 0) ? received.substr(pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so) : "";
    string value = (result == 0 && pmatch[2].rm_so >= 0) ? received.substr(pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so) : "";
    
    // DEBUG ////////////////////////////////////////////////////////////////////////////////
    // Change this to the entire received string minus line terminators.
    _ui->print("< " + command + " " + value);
    
    string response = "ERR1"; // Undefined PJLink command.
    
    if (command.compare("POWR") == 0) {
        if (value.compare("1") == 0) {
            _PJLinkPower = POWER_ON;
            response = "%1POWR=OK";
        }
        else if (value.compare("0") == 0) {
            _PJLinkPower = POWER_OFF;
            response = "%1POWR=OK";
        }
        // Power query.
        else if (value.compare("?") == 0) {
            switch (_PJLinkPower) {
                case POWER_ON:      response = "%1POWR=1";  break;
                case POWER_COOLING: response = "%1POWR=2";  break;
                case POWER_WARMING: response = "%1POWR=3";  break;
                case POWER_OFF:
                default:            response = "%1POWR=0";  break;
            }
        }
        // Power instruction out of parameter.
        else if (value.compare("") == 0) {
            response = "%1POWR=ERR2";
        }
    }
    
    fprintf(_sout, "%s\r\n", response.c_str());
    fflush(_sout);
    
    _ui->print("> " + response);
    
    _ui->refresh();
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
