/** 
 * \class Projector
 * 
 * \brief 
 */

#ifndef PROJECTOR_H
#define	PROJECTOR_H

#include <string>

class Connection;

class Projector {
public:
    static Projector *getInstance();
    
    ~Projector();

private:
    static Projector *_instance;
    
    Connection *_connection;
    
    // PJLink Variables
    int _PJLinkPower;
    int _PJLinkInput;
    int _PJLinkAVMute;
    int _PJLinkError;
    int _PJLinkLamp;
    
    std::string _PJLinkName;
    
    Projector();
    Projector(const Projector &orig);
    const Projector &operator=(const Projector &orig);
};

#endif	/* PROJECTOR_H */
