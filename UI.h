/** 
 * \class UI
 * 
 * \brief 
 */

#ifndef UI_H
#define	UI_H

#include <ncurses.h>
#include <string>

class Projector;

class UI {
public:
    static UI *getInstance();
    
    ~UI();
    
    /**
     * After initialization, the thread enters private function doUserInput and
     * blocks until the program exits.
     */
    void initialize();
    void shutdown();
    
    void refresh();

private:
    static const int CMD_STATE_MENU         = 0;
    static const int CMD_STATE_PROMPT       = 1;
    static const int CMD_STATE_POWER        = 2;
    static const int CMD_STATE_INPUT        = 3;
    static const int CMD_STATE_MUTE         = 4;
    static const int CMD_STATE_ERROR        = 5;
    static const int CMD_STATE_LAMP         = 6;
    static const int CMD_STATE_HOURS        = 7;
    
    static UI *_instance;
    
    static void end();
    
    Projector *_projector;
    
    WINDOW *_borders;
    
    WINDOW *_title;
    WINDOW *_command;
    WINDOW *_console;
    
    WINDOW *_stateHeader;
    WINDOW *_stateLabels;
    WINDOW *_stateRawValues;
    WINDOW *_stateValues;
    WINDOW *_stateHotkeys;
    
    int _commandWindowState;
            
    std::string _commandString;
    
    UI();
    UI(const UI &orig);
    const UI &operator=(const UI &orig);
    
    /**
     * Blocks for user input until the program exits.
     */
    void doUserInput();
};

#endif	/* UI_H */

