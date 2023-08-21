﻿#include "MainWin.h"
#include <chrono>


void MainWin::leftBtnDownStartDraw()
{
    switch (state)
    {
        case State::start:
        {
            break;
        }
        case State::maskReady:
        {
            dragStartCutBoxStartPos = BLPoint(cutBox.x0, cutBox.y0);
            dragStartCutBoxEndPos = BLPoint(cutBox.x1, cutBox.y1);
            if (mouseInMaskBoxIndex < 8) {
                setCutBox(MouseDownPos);
                Refresh();
            }
            return;
        }
        case State::box:
        {
            auto box = new Shape::Box();
            box->color = colors[colorBtnIndex];
            box->isFill = isFill;
            box->strokeWidth = strokeWidths[strokeBtnIndex];
            History::Push(box);
            preState = state;
            IsDrawing = true;
            break;
        }
        case State::ellipse:
        {
            auto ellipse = new Shape::Ellipse();
            ellipse->color = colors[colorBtnIndex];
            ellipse->isFill = isFill;
            ellipse->strokeWidth = strokeWidths[strokeBtnIndex];
            History::Push(ellipse);
            preState = state;
            IsDrawing = true;
            break;
        }
        case State::arrow:
        {
            auto arrow = new Shape::Arrow();
            arrow->color = colors[colorBtnIndex];
            arrow->isFill = isFill;
            arrow->strokeWidth = strokeWidths[strokeBtnIndex];
            History::Push(arrow);
            preState = state;
            IsDrawing = true;
            break;
        }
        case State::pen:
        {
            //Debug("start pen");
            auto shape = new Shape::Pen();
            shape->color = colors[colorBtnIndex];
            shape->strokeWidth = strokeWidths[strokeBtnIndex];
            History::Push(shape);
            IsDrawing = true;
            break;
        }
        case State::line:
        {
            auto shape = new Shape::Line();
            shape->color = colors[colorBtnIndex];
            shape->strokeWidth = strokeWidths[strokeBtnIndex] + 26;
            shape->isFill = isFill;
            History::Push(shape);
            preState = state;
            IsDrawing = true;
            break;
        }
        case State::number:
        {
            auto shape = new Shape::Number();
            shape->color = colors[colorBtnIndex];
            shape->strokeWidth = strokeWidths[strokeBtnIndex];
            shape->isFill = isFill;
            History::Push(shape);
            shape->SetNumber();
            preState = state;
            IsDrawing = true;
            break;
        }
        case State::eraser:
        {
            auto shape = new Shape::Eraser();
            shape->strokeWidth = strokeWidths[strokeBtnIndex] + 36;
            shape->isFill = isFill;
            shape->CopyCanvasImg();
            History::Push(shape);
            preState = state;
            IsDrawing = true;
            break;
        }
        case State::mosaic:
        {
            auto shape = new Shape::Mosaic();
            shape->strokeWidth = strokeWidths[strokeBtnIndex] + 6;
            shape->isFill = isFill;
            shape->InitMosaicImg();
            History::Push(shape);
            preState = state;
            IsDrawing = true;
            break;
        }
        case State::text:
        {
            auto shape = new Shape::Text();
            shape->color = colors[colorBtnIndex];
            History::Push(shape);
            shape->Draw(MouseDownPos.x, MouseDownPos.y, -1, -1);
            SetTimer(hwnd, 999, 660, (TIMERPROC)NULL);
            preState = state;
            IsDrawing = true;
            break;
        }
        case State::lastPathDrag:
        {
            break;
        }
    }
}


bool MainWin::OnLeftButtonDown(const int& x, const int& y)
{
    MouseDownPos.x = x;
    MouseDownPos.y = y;
    IsLeftBtnDown = true;
    if (state >= State::maskReady) {
        static auto t1 = std::chrono::system_clock::now();
        auto t2 = std::chrono::system_clock::now();
        auto count = floor<std::chrono::milliseconds>(t2 - t1).count();
        if (count > 0 && count < 260 && mouseEnterMainToolIndex == -1 && mouseEnterSubToolIndex == -1) {
            IsDoubleClick = true;
            if (!IsDrawing) {
                saveClipboard();
                return false;
            }
            else
            {
                auto shape = dynamic_cast<Shape::Text*>(History::GetLastDrawShape());
                if (shape) {
                    shape->EndDraw();
                }
                SetTimer(hwnd, 998, 60, (TIMERPROC)NULL);
                return false;
            }
        }
        t1 = t2;
    }
    if (state != State::start) {
        if (mouseEnterMainToolIndex == 9) //undo
        {
            History::Undo();
            return false;
        }
        else if (mouseEnterMainToolIndex == 10) //redo
        {
            History::Redo();
            return false;
        }
        else if (mouseEnterMainToolIndex == 11) //save file
        {
            IsLeftBtnDown = false;
            History::LastShapeDrawEnd();
            saveFile();
            return false;
        }
        else if (mouseEnterMainToolIndex == 12) //save clipboard
        {
            IsLeftBtnDown = false;
            History::LastShapeDrawEnd();
            SetTimer(hwnd, 998, 60, (TIMERPROC)NULL);
            return false;
        }
        else if (mouseEnterMainToolIndex == 13) //close
        {
            quitApp(1);
            return false;
        }
        if (!History::LastShapeDrawEnd()) { 
            //draggerIndex == -1时返回false
            //text shape也会在这里改变光标位置
            return false;
        }
        if (mouseEnterMainToolIndex != -1 && mouseEnterMainToolIndex < 9) {            
            selectedToolIndex = mouseEnterMainToolIndex;
            state = (State)(selectedToolIndex+2); 
            //设置几个图形默认是否需要填充
            if (state == State::box|| state == State::ellipse||
                state == State::line || state == State::eraser) 
            {
                isFill = false;
            }
            else if (state == State::arrow || state == State::number || state == State::mosaic) 
            {
                isFill = true;
            }
            else if (state == State::text) 
            {
                ChangeCursor(IDC_IBEAM);
            }
            Refresh();
            return false;
        }
        if (mouseEnterSubToolIndex != -1) {
            subToolBtnClick();
            return false;
        }
        leftBtnDownStartDraw();
    }
    return false;
}
bool MainWin::OnRightButtonDown(const int& x, const int& y)
{
    MouseDownPos.x = x;
    MouseDownPos.y = y;
    if (IsDrawing)
    {
        History::LastShapeDrawEnd();
        return false;
    }
    quitApp(2);
    return false;
}
bool MainWin::OnMouseMove(const int& x, const int& y)
{ 
    pixelX = x;
    pixelY = y;
    if (IsLeftBtnDown) {
        switch (state)
        {
            case State::start:
            {
                BLPoint startPos(x, y);
                BLPoint endPos(MouseDownPos.x, MouseDownPos.y);
                setCutBox(startPos, endPos);
                Refresh();
                break;
            }
            case State::maskReady:
            {
                setCutBox(POINT{ .x{x},.y{y}});
                Refresh();
                break;
            }
            case State::box:
            case State::ellipse:
            case State::arrow:
            case State::number:
            case State::line:
            case State::mosaic:
            case State::pen:
            case State::eraser:
            {
                History::LastShapeDraw(POINT{ .x{x},.y{y} },MouseDownPos);
                break;
            }
            case State::text:
            case State::lastPathDrag:
            {
                History::LastShapeDragDragger(POINT{ .x{x},.y{y} });
                break;
            }
            default:
                break;
        }
    }
    else {
        if(state == State::start){
            ChangeCursor(IDC_CROSS);
            for (size_t i = 0; i < windowBoxes.size(); i++)
            {
                if (windowBoxes[i].contains(x, y)) {
                    BLPoint startPos(windowBoxes[i].x0, windowBoxes[i].y0);
                    BLPoint endPos(windowBoxes[i].x1, windowBoxes[i].y1);
                    setCutBox(startPos, endPos);
                    break;
                }
            }
            Refresh();
            return false;
        }
        if (checkMouseEnterToolBox(POINT{ .x{x},.y{y} })) {
            return false;
        }
        if (state == State::maskReady) {
            checkMouseEnterMaskBox(POINT{ .x{x},.y{y} });
        }
        else
        {
            if (state == State::lastPathDrag) {
                History::LastShapeMouseInDragger(POINT{ .x{x},.y{y} });
            }
            else if (state == State::text) {
                ChangeCursor(IDC_IBEAM);
            }
            else
            {
                ChangeCursor(IDC_CROSS);
            }
        }        
    }
    return false;
}
bool MainWin::OnLeftButtonUp(const int& x, const int& y)
{
    IsLeftBtnDown = false;
    if (mouseEnterMainToolIndex != -1 || mouseEnterSubToolIndex != -1) {
        Refresh(); //undo redo 的按钮状态
        return false;
    }
    switch (state)
    {
        case State::start:
        {
            state = State::maskReady;
            Refresh();
            break;
        }
        case State::maskReady:
        {
            break;
        }
        case State::box:
        case State::ellipse:
        case State::arrow:
        case State::lastPathDrag:
        case State::line:
        case State::number:
        case State::text:
        case State::mosaic:
        {
            History::LastShapeShowDragger();
            state = State::lastPathDrag;
            break;
        }
        case State::eraser:
        case State::pen: 
        {
            History::LastShapeShowDragger();
            Refresh();
            break;
        }
    }
    return false;
}