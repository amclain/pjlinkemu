/** 
 * \class UI
 * 
 * \brief 
 */

#ifndef UI_H
#define	UI_H

#include <ncurses.h>

class UI {
public:
    static UI *getInstance();
    
    ~UI();
    
    void initialize();
    void shutdown();

private:
    static UI *_instance;
    
    static void end();
    
    WINDOW *_title;
    WINDOW *_menu;
    
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

