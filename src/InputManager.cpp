/*
** EPITECH PROJECT, 2024
** InputManager
** File description:
** library
*/

#include "InputManager.hpp"

InputManager &InputManager::getInstance()
{
    static InputManager instance;
    return instance;
}


void InputManager::setAutocompleteOptions(std::list<std::string> autocomplete_options)
{
    _autocomplete_options = autocomplete_options;
}

void InputManager::setHistory(std::list<std::string> history)
{
    _history = history;
}

void InputManager::clearHistory()
{
    _history.clear();
}

void InputManager::clearAutocompleteOptions()
{
    _autocomplete_options.clear();
}

std::list<std::string> InputManager::getHistory() const
{
    return _history;
}

std::list<std::string> InputManager::getAutocompleteOptions() const
{
    return _autocomplete_options;
}


void InputManager::_setupTermios()
{
    tcgetattr(STDIN_FILENO, &_oldTermios);
    struct termios newTermios = _oldTermios;
    newTermios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
    std::cout << "\033[?25l";
}

void InputManager::_restoreTermios()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &_oldTermios);
    std::cout << "\033[?25h" << std::endl;
}

std::string InputManager::readInput()
{
    int len = 0;
    char c = 0;
    _input.clear();
    _input.push_back(-1);
    _historyIndex = -1;
    _setupTermios();

    _displayInput(0);
    while (c != '\n') {
        len = _input.size();
        c = getchar();
        _handleChar(c);
        if (c == '\n') {
            break;
        }
        _displayInput(len);
    }

    _finalDisplayInput(len);
    _restoreTermios();
    std::string str = _makeString();
    if (std::find(_history.begin(), _history.end(), str) == _history.end())
        _history.push_front(str);
    return str;
}

void InputManager::_displayInput(int initialLen) {
    for (int i = 0; i <= initialLen; i++)
        std::cout << "\b \b";
    for (auto &c : _input)
        if (c == -1)
            std::cout << "\033[7m \033[0m";
        else
            std::cout << c;
    std::cout << std::flush;
}

void InputManager::_finalDisplayInput(int initialLen) {
    for (int i = 0; i <= initialLen; i++)
        std::cout << "\b \b";
    for (auto &c : _input)
        if (c != -1)
            std::cout << c;
    std::cout << std::flush;
}


void InputManager::_handleChar(char c)
{
    switch (c)
    {
    case 1: // Ctrl+A
        _moveToStart();
        break;
    case 4: // Ctrl+D
        _restoreTermios();
        throw EOF_Exception();
    case 5: // Ctrl+E
        _moveToEnd();
        break;
    case 9: // Tab
        _autocomplete();
        break;
    case 10: // New line
        break;
    case 27: // Arrow keys
        getchar();
        switch (getchar())
        {
        case 65: // Up arrow
            _historyUp();
            break;
        case 66: // Down arrow
            _historyDown();
            break;
        case 67: // Right arrow
            _swapRight();
            break;
        case 68: // Left arrow
            _swapLeft();
            break;
        }
        break;
    case 127: // Backspace
        _removeChar();
        break;
    default: // Any other character
        _addChar(c);
        break;
    }
}

void InputManager::_addChar(char c)
{
    auto it = std::find(_input.begin(), _input.end(), -1);

    if (it != _input.end())
        _input.insert(it, c);
}

void InputManager::_removeChar()
{
    auto it = std::find(_input.begin(), _input.end(), -1);

    if (it != _input.begin())
        _input.erase(std::prev(it));
}

void InputManager::_swapRight()
{
    auto it = std::find(_input.begin(), _input.end(), -1);

    if (it != std::prev(_input.end()))
        std::iter_swap(it, std::next(it));
}

void InputManager::_swapLeft()
{
    auto it = std::find(_input.begin(), _input.end(), -1);

    if (it != _input.begin())
        std::iter_swap(it, std::prev(it));
}

void InputManager::_historyUp()
{
    if (_historyIndex >= (int)_history.size() - 1)
        return;
    _historyIndex++;
    auto it = _history.begin();
    for (int i = 0; i < _historyIndex; i++)
        it++;
    _input.clear();
    for (auto &c : *it)
        _input.push_back(c);
    _input.push_back(-1);
}

void InputManager::_historyDown()
{
    if (_historyIndex == 0) {
        _historyIndex = -1;
        _input.clear();
        _input.push_back(-1);
    }
    if (_historyIndex < 0)
        return;
    _historyIndex--;
    auto it = _history.begin();
    for (int i = 0; i < _historyIndex; i++)
        it++;
    _input.clear();
    for (auto &c : *it)
        _input.push_back(c);
    _input.push_back(-1);
}

void InputManager::_moveToStart()
{
    while (_input.front() != -1) {
        auto it = std::find(_input.begin(), _input.end(), -1);
        std::swap(*it, *std::prev(it));
    }
}

void InputManager::_moveToEnd()
{
    while (_input.back() != -1) {
        auto it = std::find(_input.begin(), _input.end(), -1);
        std::swap(*it, *std::next(it));
    }
}

void InputManager::_autocomplete()
{
    std::list<std::string> matches;

    for (auto &option : _autocomplete_options) {
        if (option.find(_makeString()) == 0)
            matches.push_back(option);
    }

    if (matches.size() >= 1) {
        std::list<std::string> real_matches;
        std::string str = _makeString();
        for (auto &match : matches) {
            if (match != str)
                real_matches.push_back(match);
        }
        if (real_matches.size() == 1)
            matches = real_matches;
    }

    if (matches.size() == 1) {
        _input.clear();
        for (auto &c : matches.front())
            _input.push_back(c);
        _input.push_back(-1);
    } else if (matches.size() > 1) {
        std::string str = _makeString();
        std::cout << std::endl;
        for (auto &match : matches) {
            if (match != str)
                std::cout << match << "\t";
        }
        std::cout << "\n" << std::endl;
        _displayInput(0);
    } else {
        std::cout << "\a";
    }
}

std::string InputManager::_makeString()
{
    std::string str;

    for (auto &c : _input) {
        if (c != -1 && c != '\n')
            str += c;
    }
    return str;
}
