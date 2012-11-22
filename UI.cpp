/** 
 * \class UI
 */

#include "UI.h"

#include <cstdlib>
#include <ncurses.h>
#include <string>

using namespace std;


UI *UI::_instance = NULL;

UI *UI::getInstance() {
    if (_instance == NULL) {
        _instance = new UI();
    }
    return _instance;
}

void UI::end() {
    endwin();
}

UI::UI() {
}

UI::~UI() {
}

UI::UI(const UI &orig) {
}

const UI &UI::operator=(const UI &orig) {
}

void UI::initialize() {
    atexit(UI::end);
    at_quick_exit(UI::end);
    
    initscr();
    //start_color();
    cbreak();
    keypad(stdscr, true);
    noecho();
    curs_set(0);
    
    int x, y;
    getmaxyx(stdscr, y, x);
    
    _title = newwin(1, x, 0, 0);
    _menu = newwin(1, x, y - 1, 0);
    
    _stateHeader = newwin(2, x, 1, 0);
    _stateLabels = newwin(10, 6, 3, 3);
    _stateRawValues = newwin(10, 6 , 3, 11);
    _stateValues = newwin(10, 8, 3, 19);
    _stateHotkeys = newwin(10, 2, 3, 0);
    
    // Paint the title and menu bar backgrounds.
    wattron(_title, A_STANDOUT);
    wattron(_menu, A_STANDOUT);
    
    wmove(_title, 0, 0);
    wmove(_menu, 0, 0);
    
    for (int i = 0; i < x; i++) {
        wprintw(_title, " ");
        wprintw(_menu, " ");
    }
    
    string titleString = "PJLink Emulator";
    mvwprintw(_title, 0, (x - titleString.length()) / 2, titleString.c_str());
    mvwprintw(_menu, 0, 0, " Quit |");
    
    wattroff(_title, A_STANDOUT);
    wattroff(_menu, A_STANDOUT);
    
    mvwprintw(_stateHeader, 0, 3, "Param");
    mvwprintw(_stateHeader, 0, 11, "Raw");
    mvwprintw(_stateHeader, 0, 19, "State");
    
    mvwhline(_stateHeader, 1, 3, 0, 6);
    mvwhline(_stateHeader, 1, 11, 0, 6);
    mvwhline(_stateHeader, 1, 19, 0, 6);
    
    wattron(_stateHotkeys, A_STANDOUT);
    mvwprintw(_stateHotkeys, 0, 0, " 1");
    mvwprintw(_stateHotkeys, 1, 0, " 2");
    mvwprintw(_stateHotkeys, 2, 0, " 3");
    mvwprintw(_stateHotkeys, 3, 0, " 4");
    mvwprintw(_stateHotkeys, 4, 0, " 5");
    
    mvwprintw(_stateLabels, 0, 0, "Power:");
    mvwprintw(_stateLabels, 1, 0, "Input:");
    mvwprintw(_stateLabels, 2, 0, "Mute:");
    mvwprintw(_stateLabels, 3, 0, "Error:");
    mvwprintw(_stateLabels, 4, 0, "Lamp:");
    
    mvwprintw(_stateRawValues, 0, 0, "0");
    mvwprintw(_stateRawValues, 1, 0, "11");
    mvwprintw(_stateRawValues, 2, 0, "30");
    mvwprintw(_stateRawValues, 3, 0, "000000");
    
    mvwprintw(_stateValues, 0, 0, "Off");
    mvwprintw(_stateValues, 1, 0, "Analog 1");
    mvwprintw(_stateValues, 2, 0, "Unmuted");
    mvwprintw(_stateValues, 3, 0, "None");
    mvwprintw(_stateValues, 4, 0, "1234");
    
    wrefresh(_stateHeader);
    wrefresh(_stateLabels);
    wrefresh(_stateRawValues);
    wrefresh(_stateValues);
    wrefresh(_stateHotkeys);
    
    wrefresh(_title);
    wrefresh(_menu);
    
    
    wgetch(_stateLabels);
}

void UI::shutdown() {
    delwin(_title);
    delwin(_menu);
    delwin(_stateHeader);
    delwin(_stateLabels);
    delwin(_stateRawValues);
    delwin(_stateValues);
    delwin(_stateHotkeys);
    
    UI::end();
}
