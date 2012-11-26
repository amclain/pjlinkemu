/** 
 * \class Projector
 * 
 * \brief 
 */

#ifndef PROJECTOR_H
#define	PROJECTOR_H

#include <atomic>
#include <string>

class Projector {
public:
    static const int POWER_OFF          = 0;
    static const int POWER_ON           = 1;
    static const int POWER_COOLING      = 2;
    static const int POWER_WARMING      = 3;
    
    static const int AVMUTE_NONE        = 30;
    static const int AVMUTE_VIDEO       = 11;
    static const int AVMUTE_AUDIO       = 21;
    static const int AVMUTE_BOTH        = 31;
    
    static const int INPUT_RGB_1        = 11;
    static const int INPUT_RGB_2        = 12;
    static const int INPUT_RGB_3        = 13;
    static const int INPUT_RGB_4        = 14;
    static const int INPUT_RGB_5        = 15;
    static const int INPUT_RGB_6        = 16;
    static const int INPUT_RGB_7        = 17;
    static const int INPUT_RGB_8        = 18;
    static const int INPUT_RGB_9        = 19;
    
    static const int INPUT_VIDEO_1      = 21;
    static const int INPUT_VIDEO_2      = 22;
    static const int INPUT_VIDEO_3      = 23;
    static const int INPUT_VIDEO_4      = 24;
    static const int INPUT_VIDEO_5      = 25;
    static const int INPUT_VIDEO_6      = 26;
    static const int INPUT_VIDEO_7      = 27;
    static const int INPUT_VIDEO_8      = 28;
    static const int INPUT_VIDEO_9      = 29;
    
    
    static const int INPUT_DIGITAL_1    = 31;
    static const int INPUT_DIGITAL_2    = 32;
    static const int INPUT_DIGITAL_3    = 33;
    static const int INPUT_DIGITAL_4    = 34;
    static const int INPUT_DIGITAL_5    = 35;
    static const int INPUT_DIGITAL_6    = 36;
    static const int INPUT_DIGITAL_7    = 37;
    static const int INPUT_DIGITAL_8    = 38;
    static const int INPUT_DIGITAL_9    = 39;
    
    static const int INPUT_STORAGE_1    = 41;
    static const int INPUT_STORAGE_2    = 42;
    static const int INPUT_STORAGE_3    = 43;
    static const int INPUT_STORAGE_4    = 44;
    static const int INPUT_STORAGE_5    = 45;
    static const int INPUT_STORAGE_6    = 46;
    static const int INPUT_STORAGE_7    = 47;
    static const int INPUT_STORAGE_8    = 48;
    static const int INPUT_STORAGE_9    = 49;
    
    static const int INPUT_NETWORK_1    = 51;
    static const int INPUT_NETWORK_2    = 52;
    static const int INPUT_NETWORK_3    = 53;
    static const int INPUT_NETWORK_4    = 54;
    static const int INPUT_NETWORK_5    = 55;
    static const int INPUT_NETWORK_6    = 56;
    static const int INPUT_NETWORK_7    = 57;
    static const int INPUT_NETWORK_8    = 58;
    static const int INPUT_NETWORK_9    = 59;
    
    
    static Projector *getInstance();
    
    ~Projector();
    
    int getPowerState();
    void setPowerState(int value);
    int getInputState();
    void setInputState(int value);
    int getAVMuteState();
    void setAVMuteState(int value);
    int getErrorState();
    void setErrorState(int value);
    int getLampHours();
    void setLampHours(int value);
    
    void close();
    void listen();
    void listen(int port);
    

private:
    static Projector *_instance;
    
    // PJLink Variables
    bool _PJLinkUseAuthentication;
    
    std::atomic_int _PJLinkPower;
    std::atomic_int _PJLinkInput;
    std::atomic_int _PJLinkAVMute;
    std::atomic_int _PJLinkError;
    std::atomic_int _PJLinkLampHours;
    
    std::string _PJLinkName;
    
    // IP Connection Variables
    
    
    Projector();
    Projector(const Projector &orig);
    const Projector &operator=(const Projector &orig);
};

#endif	/* PROJECTOR_H */
