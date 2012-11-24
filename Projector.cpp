/** 
 * \class Projector
 */

#include "Projector.h"

#include <cstdlib>

Projector *Projector::_instance = NULL;

Projector *Projector::getInstance() {
    if (_instance == NULL) {
        _instance = new Projector();
    }
    return _instance;
}

Projector::Projector() {
    _PJLinkUseAuthentication = false;
    
    _PJLinkPower = POWER_OFF;
    _PJLinkInput = 11;
    _PJLinkAVMute = 30;
    _PJLinkError = 0;
    _PJLinkLampHours = 0;
    
    _PJLinkName = "Emulator";
}

Projector::~Projector() {
}

Projector::Projector(const Projector &orig) {
}

const Projector &Projector::operator=(const Projector &orig) {
}

int Projector::getPowerState() {
    return _PJLinkPower;
}

int Projector::getInputState() {
    return _PJLinkInput;
}

int Projector::getAVMuteState() {
    return _PJLinkAVMute;
}

int Projector::getErrorState() {
    return _PJLinkError;
}

int Projector::getLampHours() {
    return _PJLinkLampHours;
}

void Projector::close() {
    
}

void Projector::listen() {
    listen(4352);
}

void Projector::listen(int port) {
    
}
