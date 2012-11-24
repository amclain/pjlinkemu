/** 
 * \class UI
 * 
 * \brief 
 */

#ifndef UI_H
#define	UI_H

#include <ncurses.h>

class Projector;

class UI {
public:
    static UI *getInstance();
    
    ~UI();
    
    void initialize();
    void shutdown();
    
    void refresh();

private:
    static UI *_instance;
    
    static void end();
    
    bool _showCommandBar;
    
    Projector *_projector;
    
    WINDOW *_borders;
    
    WINDOW *_title;
    WINDOW *_menu;
    WINDOW *_command;
    WINDOW *_console;
    
    WINDOW *_stateHeader;
    WINDOW *_stateLabels;
    WINDOW *_stateRawValues;
    WINDOW *_stateValues;
    WINDOW *_stateHotkeys;
    
    UI();
    UI(const UI &orig);
    const UI &operator=(const UI &orig);
};

#endif	/* UI_H */

