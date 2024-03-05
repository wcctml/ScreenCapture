#pragma once
#include <Windows.h>
#include <string>
#include <format>
#include <vector>
#include "WindowBase.h"

class WindowMain : public WindowBase
{
public:
    WindowMain();
    ~WindowMain();
    void Save(const std::string& filePath) override;
    void SaveToClipboard() override;
    void paintToDie();
private:
    LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;
    void initCanvas() override;
    void paintCanvas() override;
    void initSize();
    void shotScreen();
    bool onMouseDown(const int& x, const int& y);
    bool onMouseDownRight(const int& x, const int& y);
    bool onMouseUp(const int& x, const int& y);
    bool onMouseMove(const int& x, const int& y);
    bool onMouseDrag(const int& x, const int& y);
    bool onChar(const unsigned int& val);
    bool onKeyDown(const unsigned int& val);
    bool onMouseWheel(const int& delta);
    bool onDoubleClick(const int& x, const int& y);
    bool onTimeout(const unsigned int& id);
};