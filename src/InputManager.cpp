/*
** EPITECH PROJECT, 2024
** tmp
** File description:
** InputManager
*/

#include "InputManager.hpp"


const char *InputManager::EOF_exception::what() const noexcept
{
    return "EOF";
}


InputManager::InputManager()
{
    _index = 0;
    _input.clear();
    _setupTermios();
    _prompt = "";
    _history.clear();
    _historyIndex = 0;
    _tabOptions.clear();

    _inputMap[1] = &InputManager::_handleCtrlA;
    _inputMap[4] = &InputManager::_handleCtrlD;
    _inputMap[5] = &InputManager::_handleCtrlE;
    _inputMap[9] = &InputManager::_handleTab;
    _inputMap[10] = &InputManager::_handleEnter;
    _inputMap[11] = &InputManager::_handleCtrlK;
    _inputMap[21] = &InputManager::_handleCtrlU;
    _inputMap[23] = &InputManager::_handleCtrlW;
    _inputMap[27] = &InputManager::_handleArrow;
    _inputMap[126] = &InputManager::_handleSuppr;
    _inputMap[127] = &InputManager::_handleBackspace;
}

InputManager::~InputManager()
{
    _resetTermios();
}


std::string InputManager::getInput() const
{
    return _input;
}

std::string InputManager::getPrompt() const
{
    return _prompt;
}

std::vector<std::string> InputManager::getHistory() const
{
    return _history;
}

std::vector<std::string> InputManager::getTabOptions() const
{
    return _tabOptions;
}


void InputManager::setPrompt(const std::string &prompt)
{
    _prompt = prompt;
}

void InputManager::setHistory(const std::vector<std::string> &history)
{
    _history = history;
}

void InputManager::addHistory(const std::string &history)
{
    _history.push_back(history);
}

void InputManager::clearHistory()
{
    _history.clear();
}

void InputManager::removeHistory(const std::string &history)
{
    _history.erase(std::remove(_history.begin(), _history.end(), history), _history.end());
}

void InputManager::setTabOptions(const std::vector<std::string> &tabOptions)
{
    _tabOptions = tabOptions;
}

void InputManager::addTabOption(const std::string &tabOption)
{
    if (std::find(_tabOptions.begin(), _tabOptions.end(), tabOption) == _tabOptions.end())
        _tabOptions.push_back(tabOption);
}

void InputManager::clearTabOptions()
{
    _tabOptions.clear();
}

void InputManager::removeTabOption(const std::string &tabOption)
{
    _tabOptions.erase(std::remove(_tabOptions.begin(), _tabOptions.end(), tabOption), _tabOptions.end());
}


std::string InputManager::readInput()
{
    _index = 0;
    _input.clear();
    _setupTermios();
    _historyIndex = _history.size();
    char c;
    _end = false;
    std::size_t len = _input.size();

    _displayInput(len);
    while (!_end) {
        len = _input.size();
        c = getchar();
        _handleInput(c);
        _displayInput(len);
    }
    _displayInput(len, true);
    std::cout << std::endl;
    _resetTermios();
    if (!_input.empty()) {
        if (!_history.empty()) {
            if (_input != _history.back())
                _history.push_back(_input);
        } else
            _history.push_back(_input);
    }
    return _input;
}


void InputManager::_setupTermios()
{
    struct termios new_term;

    tcgetattr(STDIN_FILENO, &_old);
    new_term = _old;
    new_term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
    std::cout << "\033[?25l";
}

void InputManager::_resetTermios()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &_old);
    std::cout << "\033[?25h";
}

void InputManager::_handleInput(char c)
{
    if (_inputMap.find(c) != _inputMap.end())
        (this->*_inputMap[c])();
    else
        _addChar(c);
}

void InputManager::_addChar(char c)
{
    _input.insert(_input.begin() + _index, c);
    _index++;
    _historyIndex = _history.size();
}


void InputManager::_displayInput(std::size_t len, bool clear)
{
    for (std::size_t i = 0; i <= len + _prompt.size(); i++)
        std::cout << "\b \b";
    std::cout << _prompt;
    for (std::size_t i = 0; i < _input.size(); i++) {
        if (i == _index && !clear)
            std::cout << "\033[7m" << _input[i] << "\033[0m";
        else
            std::cout << _input[i];
    }
    if (_index == _input.size() && !clear)
        std::cout << "\033[7m \033[0m";
    std::cout.flush();
}

void InputManager::_historyUp()
{
    if (_historyIndex <= 0)
        return;
    _historyIndex--;
    _input = _history[_historyIndex];
    _index = _input.size();
}

void InputManager::_historyDown()
{
    if (_historyIndex >= _history.size())
        return;
    _historyIndex++;
    if (_historyIndex == _history.size())
        _input.clear();
    else
        _input = _history[_historyIndex];
    _index = _input.size();
}


void InputManager::_handleCtrlA()
{
    _index = 0;
}

void InputManager::_handleCtrlD()
{
    _resetTermios();
    throw EOF_exception();
}

void InputManager::_handleCtrlE()
{
    _index = _input.size();
}

void InputManager::_handleTab()
{
    if (_tabOptions.empty())
        return;

    std::vector<std::string> matches;
    for (const std::string &option : _tabOptions) {
        if (option.find(_input) == 0)
            matches.push_back(option);
    }
    if (matches.empty())
        return;
    if (matches.size() == 1) {
        _input = matches[0];
        _index = _input.size();
    } else {
        std::string common = matches[0];
        for (std::size_t i = 1; i < matches.size(); i++) {
            std::size_t j = 0;
            while (j < common.size() && j < matches[i].size() && common[j] == matches[i][j])
                j++;
            common = common.substr(0, j);
        }
        _input = common;
        _index = _input.size();
        _displayInput(_input.size(), true);
        std::cout << std::endl;
        for (std::size_t i = 0; i < matches.size(); i++)
            std::cout << matches[i] << (i == matches.size() - 1 ? "\n" : "\t");
        std::cout << std::endl;
    }
}

void InputManager::_handleEnter()
{
    _end = true;
}

void InputManager::_handleCtrlK()
{
    while (_index < _input.size())
        _input.pop_back();
}

void InputManager::_handleCtrlU()
{
    _input.clear();
    _index = 0;
}

void InputManager::_handleCtrlW()
{
    for (std::size_t i = 0; i < _index; i++)
        _input.erase(_input.begin());
    _index = 0;
}

void InputManager::_handleArrow()
{
    char c = getchar();

    if (c == 91) {
        c = getchar();
        if (c == 65) // down arrow
            _historyUp();
        if (c == 66) // up arrow
            _historyDown();
        if (c == 67 && _index < _input.size()) // right arrow
            _index++;
        if (c == 68 && _index > 0) // left arrow
            _index--;
    }
}

void InputManager::_handleSuppr()
{
    if (_index < _input.size())
        _input.erase(_input.begin() + _index);
}

void InputManager::_handleBackspace()
{
    if (_index > 0) {
        _input.erase(_input.begin() + _index - 1);
        _index--;
    }
}