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
    _commandWindowState = CMD_STATE_MENU;
    
    _consoleFrozen = false;
    _consoleEndLine = 0;
    _consoleHeight = 0;
    
    _commandString.clear();
    _consoleBuffer.clear();
    
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
    _console = newwin(y - 10 - 2, x, 10, 0);

    _stateHeader = newwin(2, x, 1, 0);
    _stateLabels = newwin(6, 6, 3, 3);
    _stateRawValues = newwin(6, 6, 3, 11);
    _stateValues = newwin(6, 9, 3, 19);
    _stateHotkeys = newwin(6, 2, 3, 0);
    
    _connectionStatus = newwin(2, 10, 3, 33);

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
    mvwprintw(_stateHeader, 0, 33, "Connection");

    mvwhline(_stateHeader, 1, 3, 0, 6);
    mvwhline(_stateHeader, 1, 11, 0, 6);
    mvwhline(_stateHeader, 1, 19, 0, 6);
    mvwhline(_stateHeader, 1, 33, 0, 10);

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
    
    _consoleHeight = getmaxy(_console);
    
    this->refresh();
    doUserInput();
}

void UI::shutdown() {
    delwin(_title);
    delwin(_console);
    delwin(_command);
    
    delwin(_stateHeader);
    delwin(_stateLabels);
    delwin(_stateRawValues);
    delwin(_stateValues);
    delwin(_stateHotkeys);
    
    delwin(_connectionStatus);

    UI::end();
}

void UI::refresh() {
    lock_guard<recursive_mutex> lg(_refreshMutex);
    
    int x, y;
    getmaxyx(stdscr, y, x);
    
    wclear(_stateRawValues);
    wclear(_stateValues);
    
    // Draw section borders.
    mvwhline(_borders, 9, 0, 0, x - 1);
    mvwhline(_borders, y - 2, 0, 0, x - 1);
    
    mvwvline(_borders, 3, 30, 0, 6);

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
    
    
    // Connection status.
    wclear(_connectionStatus);
    wmove(_connectionStatus, 0, 0);
    
    if (_projector->isConnected() == true) {
        wprintw(_connectionStatus, "Connected");
    }
    else if (_projector->isListening() == true) {
        wprintw(_connectionStatus, "Listening");
    }
    else {
        wprintw(_connectionStatus, "Closed");
    }
    
    mvwprintw(_connectionStatus, 1, 0, "Port: %i", _projector->getPort());

    
    wrefresh(_borders);
    
    wrefresh(_stateHeader);
    wrefresh(_stateLabels);
    wrefresh(_stateRawValues);
    wrefresh(_stateValues);
    wrefresh(_stateHotkeys);
    
    wrefresh(_connectionStatus);

    wrefresh(_title);
    
    // Update the console from the buffer.
    wclear(_console);
    
    if (_consoleBuffer.size() > 0) {
        if (_consoleFrozen == false) {
            // Console follows last line.
            _consoleEndLine = _consoleBuffer.size() - 1;
        }
        else {
            // Window is frozen, but modify _consoleEndLine so that text fills
            // the entire window if available.
            if (_consoleEndLine < _consoleHeight && _consoleBuffer.size() >= _consoleHeight) {
                _consoleEndLine = _consoleHeight - 1;
            }
        }
        
        if (_consoleEndLine > _consoleBuffer.size() - 1) _consoleEndLine = _consoleBuffer.size() - 1;
        
        int consoleStartLine = (_consoleEndLine < _consoleHeight) ? 0 : _consoleEndLine - _consoleHeight + 1;
        
        for (int line = consoleStartLine; line <= _consoleEndLine; line++) {
            wprintw(_console, "%3i:  %s\n", line + 1, _consoleBuffer[line].c_str());
        }
    }
    
    wrefresh(_console);
    
    // Command Window State Machine - Display
    switch (_commandWindowState) {

        case CMD_STATE_PROMPT:
            curs_set(1);
            break;
            

        case CMD_STATE_POWER:
            curs_set(0);
            
            wattron(_command, A_STANDOUT);
            wmove(_command, 0, 0);
            for (int i = 0; i < x; i++) {
                wprintw(_command, " ");
            }
            mvwprintw(_command, 0, 0, " 0 Off | 1 On | 2 Cooling | 3 Warming |");
            wattroff(_command, A_STANDOUT);
            break;
            
            
        case CMD_STATE_INPUT:
            curs_set(0);
            
            wattron(_command, A_STANDOUT);
            wmove(_command, 0, 0);
            for (int i = 0; i < x; i++) {
                wprintw(_command, " ");
            }
            mvwprintw(_command, 0, 0, " 1x RGB | 2x VIDEO | 3x DIGITAL | 4x STORAGE | 5x NETWORK |");
            wattroff(_command, A_STANDOUT);
            break;
            
            
        case CMD_STATE_HOURS:
            curs_set(1);
            break;


        case CMD_STATE_MENU:
        default:
            curs_set(0);
            
            // Paint menu.
            wattron(_command, A_STANDOUT);
            wmove(_command, 0, 0);

            for (int i = 0; i < x; i++) {
                wprintw(_command, " ");
            }

            mvwprintw(_command, 0, 0, " Quit |");
            wattroff(_command, A_STANDOUT);
            break;
    }
    
    wrefresh(_command);
}

void UI::doUserInput() {
    int inputGroup = 0;
    
    int separatorPos;
    
    string command = "";
    string value = "";
    
    while (1) {    
        int key = wgetch(_command);
        int key2 = -1;
        int key3 = -1;
        int key4 = -1;
        
        // Detect ESC
        if (key == KEY_ESC) {
            halfdelay(1);
            key2 = wgetch(_command);
            
            // A special key other than ESC was pressed.
            if (key2 != ERR) {
                key = 0;
                key3 = wgetch(_command);
                key4 = wgetch(_command);
            }
            
            cbreak();
        }
        
        // DEBUG ////////////////////////////////////////////
        /*
        char bufk[255];
        sprintf(bufk, "%i %i %i %i", key, key2, key3, key4);
        string k(bufk);
        print(k);
        */
        
        // Console window navigation.
        
        // Home
        if (key2 == 79 && key3 == 72 && key4 == -1) {
            _consoleFrozen = true;
            _consoleEndLine = 0;
        }
        
        // End
        else if (key2 == 79 && key3 == 70 && key4 == -1) {
            _consoleFrozen = false;
        }
        
        // Page Up
        else if (key2 == 91 && key3 == 53 && key4 == 126) {
            _consoleFrozen = true;
            _consoleEndLine -= _consoleHeight / 2;
        }
        
        // Page Down
        else if (key2 == 91 && key3 == 54 && key4 == 126) {
            _consoleEndLine += _consoleHeight / 2;
            if (_consoleEndLine >= _consoleBuffer.size() - 1) _consoleFrozen = false;
        }
        
        // Arrow Up
        else if (key2 == 91 && key3 == 65 && key4 == -1) {
            _consoleFrozen = true;
            _consoleEndLine--;
        }
        
        // Arrow Down
        else if (key2 == 91 && key3 == 66 && key4 == -1) {
            if (_consoleEndLine < _consoleBuffer.size() - 1) _consoleEndLine++;
            if (_consoleEndLine == _consoleBuffer.size() - 1) _consoleFrozen = false;
        }
        
        // Arrow Left
        else if (key2 == 91 && key3 == 68 && key4 == -1) {
            
        }
        
        // Arrow Right
        else if (key2 == 91 && key3 == 67 && key4 == -1) {
            
        }
        

        // Command Window State Machine - Input
        switch (_commandWindowState) {
            
            case CMD_STATE_MENU:
                switch (key) {
                    case '1':
                        _commandWindowState = CMD_STATE_POWER;
                        break;

                    case '2':
                        _commandWindowState = CMD_STATE_INPUT;
                        break;

                    case '3':
                        _commandWindowState = CMD_STATE_AVMUTE;
                        break;

                    case '4':
                        _commandWindowState = CMD_STATE_ERROR;
                        break;

                    case '5':
                        break;
                        
                    case '6':
                        clearCommandBuffer();
                        mvwprintw(_command, 0, 0, "Lamp Hours:");
                        _commandWindowState = CMD_STATE_HOURS;
                        break;

                    case ':':   // Open command prompt.
                        clearCommandBuffer();
                        mvwprintw(_command, 0, 0, ":");
                        _commandWindowState = CMD_STATE_PROMPT;
                        break;
                        
                    case 'l':   // Listen for connection or close connection.
                        if (_projector->isListening() == true) {
                            _projector->close();
                        }
                        else {
                            _projector->listen();
                        }
                        break;

                    case 'q':
                        exit(0);
                        break;

                    default:
                        break;
                }
                break;
                
            
            // Are we processing a user command?
            case CMD_STATE_PROMPT:    
                bufferKeyPress(key);

                switch (key) {
                    // Process command.
                    case KEY_ENTER:
                    case '\n':
                        //print(_commandString); // DEBUG ////////////////////////////////////////////////////////////////////////////////
                        
                        // Search for command/value separator.
                        separatorPos = _commandString.find(" ");
                        if (separatorPos > -1) {
                            command = _commandString.substr(0, separatorPos);
                            if (_commandString.length() > separatorPos - 1) value = _commandString.substr(separatorPos + 1);
                        }
                        
                        // Execute command.
                        if (command == "port") {
                            if (stoi(value) > 0) _projector->setPort(stoi(value));
                        }
                        
                        //print("CMD: " + command + " " + value); // DEBUG ////////////////////////////////////////////////////////////////////////////////
                        
                        _commandWindowState = CMD_STATE_MENU;
                        break;
                        
                    // Abort command mode.
                    case KEY_ESC:
                        _commandWindowState = CMD_STATE_MENU;
                        break;

                    default:
                        break;
                }
                break;
                
                
            case CMD_STATE_POWER:
                switch (key) {
                    case '`':
                    case '0':   _projector->setPowerState(Projector::POWER_OFF);        break;
                    case '1':   _projector->setPowerState(Projector::POWER_ON);         break;
                    case '2':   _projector->setPowerState(Projector::POWER_COOLING);    break;
                    case '3':   _projector->setPowerState(Projector::POWER_WARMING);    break;
                    default:    break;
                }
                _commandWindowState = CMD_STATE_MENU;
                break;
                
                
            case CMD_STATE_INPUT:
                if (inputGroup == 0) {
                     // First key press: Select input group.
                    switch (key) {
                        case '1':   inputGroup = Projector::INPUT_RGB_1;        break;
                        case '2':   inputGroup = Projector::INPUT_VIDEO_1;      break;
                        case '3':   inputGroup = Projector::INPUT_DIGITAL_1;    break;
                        case '4':   inputGroup = Projector::INPUT_STORAGE_1;    break;
                        case '5':   inputGroup = Projector::INPUT_NETWORK_1;    break;
                        default:    _commandWindowState = CMD_STATE_MENU;       break;
                    }
                }
                else {
                    // Second key press: Select input.
                    if (key >= '1' && key <= '9') {
                        _projector->setInputState(inputGroup + (key - 48) - 1);
                    }
                    
                    inputGroup = 0;
                    _commandWindowState = CMD_STATE_MENU;
                }
                break;
                
                
            case CMD_STATE_HOURS:
                bufferKeyPress(key);
                
                switch (key) {
                    // Process command.
                    case KEY_ENTER:
                    case '\n':
                        _projector->setLampHours(atoi(_commandString.c_str()));
                        _commandWindowState = CMD_STATE_MENU;
                        break;
                        
                    // Abort command mode.
                    case KEY_ESC:
                        _commandWindowState = CMD_STATE_MENU;
                        break;

                    default:
                        break;
                }
                break;
               
                
            default:
                _commandWindowState = CMD_STATE_MENU;
                break;
        }
        
        this->refresh();
    }
    
    shutdown();
}

void UI::bufferKeyPress(int key) {
    // Let these characters pass through.
    if (key >= 0x20 && key <= 0x7E) {
        _commandString.push_back(key);
        wprintw(_command, "%c", key);

    }
    else if (key == KEY_BACKSPACE || key == 127) {
        if (_commandString.length() > 0) {
            _commandString.erase(_commandString.length() - 1);
            mvwprintw(_command, getcury(_command), getcurx(_command) - 1, " ");
            wmove(_command, getcury(_command), getcurx(_command) - 1);
        }
    }
}

void UI::clearCommandBuffer() {
    _commandString.clear();
    wclear(_command);
}

void UI::print(const std::string &message) {
    lock_guard<recursive_mutex> lg(_printMutex);
    
    _consoleBuffer.push_back(message);
    this->refresh();
}
