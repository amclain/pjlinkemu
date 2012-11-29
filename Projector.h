/** 
 * \class Projector
 * 
 * \brief 
 */

#ifndef PROJECTOR_H
#define	PROJECTOR_H

#include <atomic>
#include <mutex>
#include <string>
#include <thread>

#include <netinet/in.h>

class UI;

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
    
    static const int AVMUTE_UNMUTE_VIDEO= 10;
    static const int AVMUTE_UNMUTE_AUDIO= 20;
    static const int AVMUTE_UNMUTE_BOTH = 30;
    
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
    void closeClient();
    void listen();
    void listen(int port);
    
    bool isListening();
    bool isConnected();
    
    int getPort();
    void setPort(int port);
    

private:
    static Projector *_instance;
    
    UI *_ui;
    
    /**
     * Emulates a bug found in Sanyo projectors where the socket hangs open if
     * the client disconnects improperly.  The projector's ethernet cable must
     * be disconnected and reconnected before the socket closes and the
     * projector can accept another connection.
     */
    bool _emulateHangOpenBug;
    
    // PJLink Variables
    bool _PJLinkUseAuthentication;
    
    std::atomic_int _PJLinkPower;
    std::atomic_int _PJLinkInput;
    std::atomic_int _PJLinkAVMute;
    std::atomic_int _PJLinkError;
    std::atomic_int _PJLinkLampHours;
    
    std::string _PJLinkName;
    
    std::string _PJLinkPassword;
    
    // IP Connection Variables
    std::thread *_socketListener;
    std::thread *_socketTimer;
    
    std::atomic_int _port;
    
    std::atomic_bool _isListening;
    std::atomic_bool _isConnected;
    int _serverfd, _clientfd;
    socklen_t _clientlen;
    sockaddr_in _serveraddr, _clientaddr;
    
    FILE *_sin, *_sout;
    
    time_t _clientConnected;
    time_t _clientTimeout;
    
    
    Projector();
    Projector(const Projector &orig);
    const Projector &operator=(const Projector &orig);
    
    void accept();    
    
    /**
     * Traps a thread in a socket accept loop.
     */
    void doAccept();
    
    void doRead();
    
    void resetSocketTimeout();
};

#endif	/* PROJECTOR_H */
