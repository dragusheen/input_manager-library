/*
** EPITECH PROJECT, 2024
** input_manager-library
** File description:
** InputManager
*/

#ifndef INPUT_MANAGER_HPP_
    #define INPUT_MANAGER_HPP_

    #include <algorithm>
    #include <iostream>
    #include <string>
    #include <vector>
    #include <termios.h>
    #include <unistd.h>

class InputManager {
    public:
        class EOF_exception : public std::exception {
            public:
                const char *what() const noexcept override;
        };
        InputManager();
        ~InputManager();

        [[ nodiscard ]] std::string getInput() const;
        [[ nodiscard ]] std::string getPrompt() const;
        [[ nodiscard ]] std::vector<std::string> getHistory() const;
        [[ nodiscard ]] std::vector<std::string> getTabOptions() const;

        void setPrompt(const std::string &prompt);

        void setHistory(const std::vector<std::string> &history);
        void addHistory(const std::string &history);
        void clearHistory();
        void removeHistory(const std::string &history);

        void setTabOptions(const std::vector<std::string> &tabOptions);
        void addTabOption(const std::string &tabOption);
        void clearTabOptions();
        void removeTabOption(const std::string &tabOption);

        std::string readInput();

    protected:
    private:
        bool _end;
        std::string _input;
        std::size_t _index;
        struct termios _old;
        std::unordered_map<int, void (InputManager::*)(void)> _inputMap;
        std::string _prompt;
        std::vector<std::string> _history;
        std::size_t _historyIndex;
        std::vector<std::string> _tabOptions;

        void _setupTermios();
        void _resetTermios();
        void _handleInput(char c);
        void _addChar(char c);
        void _displayInput(std::size_t len, bool clear = false);
        void _historyUp();
        void _historyDown();

        void _handleCtrlA();
        void _handleCtrlD();
        void _handleCtrlE();
        void _handleTab();
        void _handleEnter();
        void _handleCtrlK();
        void _handleCtrlU();
        void _handleCtrlW();
        void _handleArrow();
        void _handleSuppr();
        void _handleBackspace();
};

#endif /* !INPUMANAGER_HPP_ */
