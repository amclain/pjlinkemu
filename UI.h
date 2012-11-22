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
    
    UI();
    UI(const UI &orig);
    const UI &operator=(const UI &orig);
};

#endif	/* UI_H */

