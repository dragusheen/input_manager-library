/*
** EPITECH PROJECT, 2024
** NanoTekSpice
** File description:
** OOP
*/

#ifndef INPUT_MANAGER_H_
    #define INPUT_MANAGER_H_

    #include <algorithm>
    #include <iostream>
    #include <list>
    #include <string>
    #include <termios.h>
    #include <unistd.h>

    class InputManager
    {
        public:
            class EOF_Exception: public std::exception
            {
                public:
                    const char *what() const throw() { return "EOF"; }
            };
            static InputManager &getInstance();
            std::string readInput();
            void setAutocompleteOptions(std::list<std::string> autocomplete_options);
            void setHistory(std::list<std::string> history);
            void clearHistory();
            void clearAutocompleteOptions();
            std::list<std::string> getHistory() const;
            std::list<std::string> getAutocompleteOptions() const;

        private:
            InputManager() = default;
            std::list<char> _input;
            struct termios _oldTermios;
            int _historyIndex = 0;
            std::list<std::string> _history;
            std::list<std::string> _autocomplete_options;

            void _setupTermios();
            void _restoreTermios();
            void _displayInput(int initialLen);
            void _finalDisplayInput(int initialLen);
            void _handleChar(char c);
            void _addChar(char c);
            void _removeChar();
            void _swapRight();
            void _swapLeft();
            void _historyUp();
            void _historyDown();
            void _moveToStart();
            void _moveToEnd();
            void _autocomplete();
            std::string _makeString();
    };

#endif /* INPUT_MANAGER_H_ */
