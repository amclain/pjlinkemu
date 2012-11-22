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
}

Projector::~Projector() {
}

Projector::Projector(const Projector &orig) {
}

const Projector &Projector::operator=(const Projector &orig) {
}
