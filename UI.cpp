/** 
 * \class UI
 */

#include "UI.h"

#include <ncurses.h>

UI *UI::_instance = NULL;

UI *UI::getInstance() {
    if (_instance == NULL) {
        _instance = new UI();
    }
    return _instance;
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
    initscr();
    printw("Hello world!");
    refresh();
    getch();
}

void UI::shutdown() {
    endwin();
}