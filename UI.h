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
    
    /**
     * After initialization, the thread enters private function doUserInput and
     * blocks until the program exits.
     */
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
    
    /**
     * Blocks for user input until the program exits.
     */
    void doUserInput();
};

#endif	/* UI_H */

