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

void Projector::close() {
    
}

void Projector::listen() {
    listen(4352);
}

void Projector::listen(int port) {
    
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
