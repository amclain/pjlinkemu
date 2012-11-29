/** 
 * \class UI
 * 
 * \brief 
 */

#ifndef UI_H
#define	UI_H

#include <mutex>
#include <ncurses.h>
#include <string>
#include <vector>

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
    
    /**
     * Print a message to the console window.
     * A line feed will be appended.
     * @param message
     */
    void print(const std::string &message);

private:
    static const int CMD_STATE_MENU         = 0;
    static const int CMD_STATE_PROMPT       = 1;
    static const int CMD_STATE_POWER        = 2;
    static const int CMD_STATE_INPUT        = 3;
    static const int CMD_STATE_AVMUTE       = 4;
    static const int CMD_STATE_ERROR        = 5;
    static const int CMD_STATE_LAMP         = 6;
    static const int CMD_STATE_HOURS        = 7;
    
    static UI *_instance;
    
    static void end();
    
    Projector *_projector;
    
    std::recursive_mutex _refreshMutex;
    std::recursive_mutex _printMutex;
    
    WINDOW *_borders;
    
    WINDOW *_title;
    WINDOW *_command;
    WINDOW *_console;
    
    WINDOW *_stateHeader;
    WINDOW *_stateLabels;
    WINDOW *_stateRawValues;
    WINDOW *_stateValues;
    WINDOW *_stateHotkeys;
    
    WINDOW *_connectionStatus;
    
    int _commandWindowState;
    
    bool _consoleFrozen;
    int _consoleHeight;
    int _consoleEndLine;
            
    std::string _commandString;
    std::vector<std::string> _consoleBuffer;
    
    UI();
    UI(const UI &orig);
    const UI &operator=(const UI &orig);
    
    /**
     * Blocks for user input until the program exits.
     */
    void doUserInput();
    
    /**
     * Adds the key press to the _commandString buffer.
     * @param key
     */
    void bufferKeyPress(int key);
    
    void clearCommandBuffer();
    
    void consoleLineUp();
    void consoleLineDown();
    void consolePageUp();
    void consolePageDown();
    void consoleHome();
    void consoleEnd();
};

#endif	/* UI_H */

