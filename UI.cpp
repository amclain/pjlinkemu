/** 
 * \class UI
 */

#include "UI.h"
#include "Projector.h"

#include <cstdlib>
#include <ncurses.h>
#include <string>

using namespace std;

#define KEY_ESC         (27)


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
    _showCommandBar = false;
    _projector = Projector::getInstance();

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

    _borders = newwin(y, x, 0, 0);
    
    _title = newwin(1, x, 0, 0);
    _command = newwin(1, x, y - 1, 0);
    _console = newwin(y - 10 - 3, x, 10, 0);

    _stateHeader = newwin(2, x, 1, 0);
    _stateLabels = newwin(6, 6, 3, 3);
    _stateRawValues = newwin(6, 6, 3, 11);
    _stateValues = newwin(6, 8, 3, 19);
    _stateHotkeys = newwin(6, 2, 3, 0);

    // Paint title.
    wattron(_title, A_STANDOUT);
    wmove(_title, 0, 0);

    for (int i = 0; i < x; i++) {
        wprintw(_title, " ");
    }

    string titleString = "PJLink Device Emulator";
    mvwprintw(_title, 0, (x - titleString.length()) / 2, titleString.c_str());

    wattroff(_title, A_STANDOUT);
    

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
    mvwprintw(_stateHotkeys, 5, 0, " 6");

    mvwprintw(_stateLabels, 0, 0, "Power:");
    mvwprintw(_stateLabels, 1, 0, "Input:");
    mvwprintw(_stateLabels, 2, 0, "Mute:");
    mvwprintw(_stateLabels, 3, 0, "Error:");
    mvwprintw(_stateLabels, 4, 0, "Lamp:");
    mvwprintw(_stateLabels, 5, 0, "Hours:");
    
    this->refresh();
    doUserInput();
}

void UI::shutdown() {
    delwin(_title);
    delwin(_stateHeader);
    delwin(_stateLabels);
    delwin(_stateRawValues);
    delwin(_stateValues);
    delwin(_stateHotkeys);

    UI::end();
}

void UI::refresh() {
    int x, y;
    getmaxyx(stdscr, y, x);
    
    wclear(_stateRawValues);
    wclear(_stateValues);
    
    // Draw section borders.
    mvwhline(_borders, 9, 0, 0, x - 1);
    mvwhline(_borders, y - 2, 0, 0, x - 1);

    mvwprintw(_stateRawValues, 0, 0, "%i", _projector->getPowerState());
    mvwprintw(_stateRawValues, 1, 0, "%i", _projector->getInputState());
    mvwprintw(_stateRawValues, 2, 0, "%i", _projector->getAVMuteState());
    mvwprintw(_stateRawValues, 3, 0, "%06i", _projector->getErrorState());
    mvwprintw(_stateRawValues, 5, 0, "%i", _projector->getLampHours());

    // PJLINK STATE VALUES //

    // Power State
    std::string value = "";
    switch (_projector->getPowerState()) {
        case Projector::POWER_OFF:      value = "Off";          break;
        case Projector::POWER_ON:       value = "On";           break;
        case Projector::POWER_COOLING:  value = "Cooling";      break;
        case Projector::POWER_WARMING:  value = "Warming";      break;
        default:                        value = "Unknown";      break;
    }
    mvwprintw(_stateValues, 0, 0, "%s", value.c_str());

    // Input State
    value = "";
    switch (_projector->getInputState()) {
        case Projector::INPUT_RGB_1:    value = "RGB 1";        break;
        case Projector::INPUT_RGB_2:    value = "RGB 2";        break;
        case Projector::INPUT_RGB_3:    value = "RGB 3";        break;
        case Projector::INPUT_RGB_4:    value = "RGB 4";        break;
        case Projector::INPUT_RGB_5:    value = "RGB 5";        break;
        case Projector::INPUT_RGB_6:    value = "RGB 6";        break;
        case Projector::INPUT_RGB_7:    value = "RGB 7";        break;
        case Projector::INPUT_RGB_8:    value = "RGB 8";        break;
        case Projector::INPUT_RGB_9:    value = "RGB 9";        break;
        
        case Projector::INPUT_VIDEO_1:  value = "Video 1";      break;
        case Projector::INPUT_VIDEO_2:  value = "Video 2";      break;
        case Projector::INPUT_VIDEO_3:  value = "Video 3";      break;
        case Projector::INPUT_VIDEO_4:  value = "Video 4";      break;
        case Projector::INPUT_VIDEO_5:  value = "Video 5";      break;
        case Projector::INPUT_VIDEO_6:  value = "Video 6";      break;
        case Projector::INPUT_VIDEO_7:  value = "Video 7";      break;
        case Projector::INPUT_VIDEO_8:  value = "Video 8";      break;
        case Projector::INPUT_VIDEO_9:  value = "Video 9";      break;
        
        case Projector::INPUT_DIGITAL_1: value = "Digital 1";   break;
        case Projector::INPUT_DIGITAL_2: value = "Digital 2";   break;
        case Projector::INPUT_DIGITAL_3: value = "Digital 3";   break;
        case Projector::INPUT_DIGITAL_4: value = "Digital 4";   break;
        case Projector::INPUT_DIGITAL_5: value = "Digital 5";   break;
        case Projector::INPUT_DIGITAL_6: value = "Digital 6";   break;
        case Projector::INPUT_DIGITAL_7: value = "Digital 7";   break;
        case Projector::INPUT_DIGITAL_8: value = "Digital 8";   break;
        case Projector::INPUT_DIGITAL_9: value = "Digital 9";   break;
        
        case Projector::INPUT_STORAGE_1: value = "Storage 1";   break;
        case Projector::INPUT_STORAGE_2: value = "Storage 2";   break;
        case Projector::INPUT_STORAGE_3: value = "Storage 3";   break;
        case Projector::INPUT_STORAGE_4: value = "Storage 4";   break;
        case Projector::INPUT_STORAGE_5: value = "Storage 5";   break;
        case Projector::INPUT_STORAGE_6: value = "Storage 6";   break;
        case Projector::INPUT_STORAGE_7: value = "Storage 7";   break;
        case Projector::INPUT_STORAGE_8: value = "Storage 8";   break;
        case Projector::INPUT_STORAGE_9: value = "Storage 9";   break;
        
        case Projector::INPUT_NETWORK_1: value = "Network 1";   break;
        case Projector::INPUT_NETWORK_2: value = "Network 2";   break;
        case Projector::INPUT_NETWORK_3: value = "Network 3";   break;
        case Projector::INPUT_NETWORK_4: value = "Network 4";   break;
        case Projector::INPUT_NETWORK_5: value = "Network 5";   break;
        case Projector::INPUT_NETWORK_6: value = "Network 6";   break;
        case Projector::INPUT_NETWORK_7: value = "Network 7";   break;
        case Projector::INPUT_NETWORK_8: value = "Network 8";   break;
        case Projector::INPUT_NETWORK_9: value = "Network 9";   break;
        
        default:                         value = "Unknown";     break;
    }
    mvwprintw(_stateValues, 1, 0, "%s", value.c_str());

    // A/V Mute State
    value = "";
    switch (_projector->getAVMuteState()) {
        case Projector::AVMUTE_NONE:    value = "Unmuted";      break;
        case Projector::AVMUTE_VIDEO:   value = "Video Mute";   break;
        case Projector::AVMUTE_AUDIO:   value = "Audio Mute";   break;
        case Projector::AVMUTE_BOTH:    value = "A/V Muted";    break;
        default:                        value = "Unknown";      break;
    }
    mvwprintw(_stateValues, 2, 0, "%s", value.c_str());

    // Error State
    mvwprintw(_stateValues, 3, 0, "None");

    // Lamp State
    mvwprintw(_stateValues, 4, 0, "");

    // Lamp Hours
    mvwprintw(_stateValues, 5, 0, "%i", _projector->getLampHours());

    
    wrefresh(_borders);
    
    wrefresh(_stateHeader);
    wrefresh(_stateLabels);
    wrefresh(_stateRawValues);
    wrefresh(_stateValues);
    wrefresh(_stateHotkeys);

    wrefresh(_title);
    wrefresh(_console);
    
    if (_showCommandBar == true) {
        curs_set(1);
        wrefresh(_command);
    }
    else {
        curs_set(0);
        
        // Paint menu.
        wattron(_command, A_STANDOUT);
        wmove(_command, 0, 0);

        for (int i = 0; i < x; i++) {
            wprintw(_command, " ");
        }

        mvwprintw(_command, 0, 0, " Quit |");
        wattroff(_command, A_STANDOUT);
        
        wrefresh(_command);
    }
}

void UI::doUserInput() {
    while (1) {
        int key = wgetch(_command);

        // Are we processing a user command?
        if (_showCommandBar == true) {
            switch (key) {
                case KEY_ESC:
                    wclear(_command);
                    _showCommandBar = false;
                    break;
                    
                default:
                    break;
            }
        }
        else {
            switch (key) {
                case '1':
                    break;
                    
                case '2':
                    break;
                    
                case '3':
                    break;
                    
                case '4':
                    break;
                    
                case '5':
                    break;
                    
                case ':':
                    wclear(_command);
                    mvwprintw(_command, 0, 0, ":");
                    _showCommandBar = true;
                    break;
                
                case 'q':
                    exit(0);
                    break;
                    
                default:
                    break;
            }
        }
        
        this->refresh();
    }
    
    shutdown();
}