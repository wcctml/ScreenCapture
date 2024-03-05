﻿#include "ShapeText.h"
#include "include/core/SkPaint.h"
#include "include/utils/SkTextUtils.h"
#include "../App.h"
#include "../WindowBase.h"
#include "../ToolSub.h"
#include "../Cursor.h"
#include "../Recorder.h"
#include "../ToolMain.h"

ShapeText::ShapeText(const int &x, const int &y) : ShapeBase(x, y)
{
    IsTemp = false;
    DraggerCursors.push_back(Cursor::cursor::input);
    auto tool = ToolSub::Get();
    color = tool->GetColor();
    auto font = App::GetFontText();
    font->setSize(fontSize);
    float left{ (float)startX - 10 }, top{ (float)startY - 10 }, width{ 20 }, height{ 0 };
    SkRect lineRect;
    font->measureText(L"中", 2, SkTextEncoding::kUTF16, &lineRect);
    lineHeight = lineRect.height() + lineRect.bottom() + 10;
    height = lineHeight;
    top += lineRect.top();
    Rect.setXYWH(left, top, width, height);
    activeKeyboard(getCursorX(), startY);
}
ShapeText::~ShapeText()
{
}
bool ShapeText::OnMouseDown(const int &x, const int &y)
{
    if (!Rect.contains(x, y)) {
        return true;
    }
    App::GetWin()->ClearTimeout(WM_FLASH_CURSOR);
    App::GetWin()->SetTimeout(WM_FLASH_CURSOR, 600);
    if (lines.size() == 0) {
        showCursor = true;
        Paint(nullptr);//对象刚刚被创建，需要渲染方框和光标
        return false;
    }
    hoverX = x;
    hoverY = y;
    lineIndex = (y - Rect.top()) / lineHeight;
    int index = 0;
    float width = x - Rect.left() - 10;    
    SkRect Rect;
    float right{ 0 };
    bool flag = false;
    auto font = App::GetFontText();
    font->setSize(fontSize);
    for (size_t i = 0; i < lines[lineIndex].length() + 1; i++)
    {
        auto str = lines[lineIndex].substr(0, i);
        auto data = str.data();
        auto length = wcslen(data) * 2;
        font->measureText(data, length, SkTextEncoding::kUTF16, &Rect);
        if (Rect.right() > width) {
            float half = (Rect.right() - right) / 2 + right;
            if (half > width) {
                wordIndex = i - 1;
            }
            else
            {
                wordIndex = i;
            }
            flag = true;
            break;
        }
        right = Rect.right();
    }
    if (!flag) {
        wordIndex = lines[lineIndex].length();
    }
    showCursor = true;
    return false;
}
bool ShapeText::OnMouseMove(const int &x, const int &y)
{
    if (Rect.contains(x, y)) {
        HoverIndex = 0;
        return true;
    }
    return false;
}
bool ShapeText::OnMouseUp(const int &x, const int &y)
{
    return false;
}
bool ShapeText::OnMoseDrag(const int &x, const int &y)
{
    auto xSpan = x - hoverX;
    auto ySpan = y - hoverY;
    startX += xSpan;
    startY += ySpan;
    hoverX = x;
    hoverY = y;
    showCursor = true;
    Paint(nullptr);
    return false;
}
bool ShapeText::OnMouseWheel(const int& delta)
{
    if (delta > 0) {
        fontSize += 3;
    }
    else {
        fontSize -= 3;
    }
    if (fontSize < 28) {
        fontSize = 28;
    }
    auto font = App::GetFontText();
    font->setSize(fontSize);    
    showCursor = true;
    Paint(nullptr);
    return false;
}
void ShapeText::OnShowDragger(SkCanvas* canvas)
{
    setRect(canvas);
}
bool ShapeText::OnChar(const unsigned int& val)
{
    if (val == 13) { //enter
        if (wordIndex != lines[lineIndex].length()) {
            auto str1 = lines[lineIndex].substr(0, wordIndex);
            auto str2 = lines[lineIndex].substr(wordIndex);
            lines[lineIndex] = str1;
            lines.insert(lines.begin() + lineIndex+1, str2);
        }
        else
        {
            lines.push_back(L"");
        }
        lineIndex += 1;
        wordIndex = 0;
    }
    else if (val == 8) { //backspace 删除一个字
        if (lines.size() == 0) {
            return false;
        }
        if (wordIndex == 0) {
            if (lineIndex == 0) {
                return false;
            }
            else {
                wordIndex = lines[lineIndex-1].size();
                lines[lineIndex-1] = lines[lineIndex-1] + lines[lineIndex];
                lines.erase(lines.begin() + lineIndex);
                lineIndex -= 1;
                showCursor = true;
                Paint(nullptr);
                return false;
            }
        }
        lines[lineIndex] = lines[lineIndex].substr(0, wordIndex - 1) + lines[lineIndex].substr(wordIndex);
        if (lines[lineIndex].empty()) {
            lines.erase(lines.begin() + lineIndex);
            lineIndex -= 1;
            if (lineIndex < 0) {
                lineIndex = 0;
                wordIndex = 0;
            }
            else {
                wordIndex = lines[lineIndex].length();
            }            
        }
        else {
            wordIndex -= 1;
        } 
    }
    else {
        std::wstring word{ (wchar_t)val };
        if (lines.size() == 0) {
            lines.push_back(word);
        }
        else {
            auto str1 = lines[lineIndex].substr(0, wordIndex);
            auto str2 = lines[lineIndex].substr(wordIndex);
            lines[lineIndex] = str1+ word + str2;
        }
        wordIndex += 1;
    }
    showCursor = true;
    Paint(nullptr);
    return false;
}
bool ShapeText::OnKeyDown(const unsigned int& val)
{
    if (val == VK_UP) {
        if (lineIndex <= 0) {
            return false;
        }
        lineIndex -= 1;
        if (wordIndex > lines[lineIndex].length()) {
            wordIndex = lines[lineIndex].length();
        }
        showCursor = true;
        Paint(nullptr);
        return true;
    }
    else if (val == VK_DELETE) {
        if (lines.size() == 0) {
            return false;
        }
        if (wordIndex >= lines[lineIndex].length()) {
            if (lineIndex != lines.size() - 1) {
                lines[lineIndex] = lines[lineIndex] + lines[lineIndex + 1];
                lines.erase(lines.begin()+ lineIndex+1);
                showCursor = true;
                Paint(nullptr);
                return true;
            }
        }
        else {
            lines[lineIndex] = lines[lineIndex].substr(0, wordIndex) + lines[lineIndex].substr(wordIndex + 1);
            showCursor = true;
            Paint(nullptr);
            return true;
        }        
        if (lines[lineIndex].empty()) {
            lines.erase(lines.begin() + lineIndex);
            lineIndex -= 1;
            if (lineIndex < 0) {
                lineIndex = 0;
                wordIndex = 0;
            }
            else {
                wordIndex = lines[lineIndex].length();
            }
            showCursor = true;
            Paint(nullptr);
            return true;
        }
    }
    else if (val == VK_DOWN) {
        if (lineIndex >= lines.size()-1) {
            return false;
        }
        lineIndex += 1;
        if (wordIndex > lines[lineIndex].length()) {
            wordIndex = lines[lineIndex].length();
        }
        showCursor = true;
        Paint(nullptr);
        return true;
    }
    else if (val == VK_LEFT) {
        if (wordIndex == 0) {
            if (lineIndex == 0) {
                return false;
            }
            lineIndex -= 1;
            wordIndex = lines[lineIndex].length();
            showCursor = true;
            Paint(nullptr);
            return true;
        }
        wordIndex -= 1;
        showCursor = true;
        Paint(nullptr);
        return true;        
    }
    else if (val == VK_RIGHT) {
        if (wordIndex == lines[lineIndex].length()) {
            if (lineIndex == lines.size() - 1) {
                return false;
            }
            lineIndex += 1;
            wordIndex = 0;
            showCursor = true;
            Paint(nullptr);
            return true;
        }
        wordIndex += 1;
        showCursor = true;
        Paint(nullptr);
        return true;
    }
    return false;
}
void ShapeText::Paint(SkCanvas *canvas)
{
    bool refreshFlag = false;
    if (!canvas) {
        auto win = App::GetWin();
        canvas = win->surfaceFront->getCanvas();
        canvas->clear(SK_ColorTRANSPARENT);
        refreshFlag = true;
    }
    if (IsWip) {
        setRect(canvas);
        if (showCursor) {
            setCursor(canvas);
        }
        showCursor = !showCursor;
        activeKeyboard(getCursorX(), startY + lineIndex * lineHeight);
    }
    if (lines.size() > 0) {
        auto font = App::GetFontText();
        font->setSize(fontSize);
        SkPaint paint;
        paint.setStroke(false);
        paint.setColor(color);
        SkRect textBounds;
        paint.setAntiAlias(true);
        float y{ (float)startY };
        for (const auto& line : lines) {
            auto data = line.data();
            auto length = wcslen(data) * 2;
            SkTextUtils::Draw(canvas, data, length, SkTextEncoding::kUTF16, startX, y, *font, paint, SkTextUtils::kLeft_Align);
            y += lineHeight;
        }
    }      
    if (refreshFlag) {
        App::GetWin()->Refresh();
    }
    
    //font->measureText(str.c_str(), str.size(), SkTextEncoding::kUTF8, &textBounds);
    //SkScalar x = startX - textBounds.width() / 2 - textBounds.left();
    //SkScalar y = startY + textBounds.height() / 2 - textBounds.bottom();    
    //canvas->drawSimpleText(str.c_str(), str.size(), SkTextEncoding::kUTF8, x, y, *font, paint);

    //std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    //// 创建多行宽字符文本
    //std::wstring text = L"This is a multi-line text.\nSecond line.";
    //// 分割文本为多行
    //std::vector<SkString> lines;
    //std::wstring::size_type pos = 0;
    //while ((pos = text.find(L'\n', pos)) != std::wstring::npos) {
    //    auto str = converter.to_bytes(text.substr(0, pos));
    //    lines.push_back(SkString(str.c_str()));
    //    text.erase(0, pos + 1);
    //}
    //auto str = converter.to_bytes(text.substr(0, pos));
    //lines.push_back(SkString(str.c_str()));
    //SkFontMetrics metrics;
    //font->getMetrics(&metrics);
    //auto height = metrics.fBottom - metrics.fTop;
    //for (const auto& line : lines) {
    //    SkTextUtils::DrawString(canvas, line.c_str(), startX, startY, *font, paint,SkTextUtils::kLeft_Align);
    //    startY += height; // 下一行的位置在当前行下方加上文本大小
    //}
}
void ShapeText::activeKeyboard(long x, long y)
{
    auto win = App::GetWin();
    if (HIMC himc = ImmGetContext(win->hwnd))
    {
        COMPOSITIONFORM comp = {};
        comp.ptCurrentPos.x = x;
        comp.ptCurrentPos.y = y;
        comp.dwStyle = CFS_FORCE_POSITION;
        ImmSetCompositionWindow(himc, &comp);
        CANDIDATEFORM cand = {};
        cand.dwStyle = CFS_CANDIDATEPOS;
        cand.ptCurrentPos.x = x;
        cand.ptCurrentPos.y = y;
        ImmSetCandidateWindow(himc, &cand);
        ImmReleaseContext(win->hwnd, himc);
    }
}
void ShapeText::setRect(SkCanvas* canvas)
{
    auto font = App::GetFontText();
    font->setSize(fontSize);
    float left{ (float)startX - 10 }, top{ (float)startY - 10 }, width{ 20 }, height{ 0 };
    SkRect lineRect;
    if (lines.size() == 0) {
        font->measureText(L"中", 2, SkTextEncoding::kUTF16, &lineRect);
        lineHeight = lineRect.height()+lineRect.bottom()+10;
        height = lineHeight;
        top += lineRect.top();
    }
    else {
        for (size_t i = 0; i < lines.size(); i++)
        {
            auto data = lines[i].data();
            auto length = wcslen(data) * 2;
            font->measureText(data, length, SkTextEncoding::kUTF16, &lineRect);
            width = std::max(width, lineRect.width());            
            if (i == 0) {
                lineHeight = lineRect.height() + lineRect.bottom() + 10;
                top += lineRect.top();
            }
            height += lineHeight;
        }
        if (width > 20) {
            width += 20;
        }
    }
    Rect.setXYWH(left, top, width, height);
    SkPaint paint;
    paint.setColor(color);
    paint.setStroke(true);
    paint.setStrokeWidth(2);
    canvas->drawRect(Rect, paint);
}
void ShapeText::setCursor(SkCanvas* canvas)
{
    SkPaint paint;
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    paint.setColor(color);
    auto font = App::GetFontText();
    float inputCursorTop{ Rect.top() + lineHeight * lineIndex + lineHeight / 6 };
    float inputCursorBottom{ Rect.top() + lineHeight * (lineIndex + 1) - lineHeight / 6 };
    float inputCursorX{ getCursorX() };
    canvas->drawLine(inputCursorX, inputCursorTop, inputCursorX, inputCursorBottom, paint);
}
bool ShapeText::EndInput()
{
    auto win = App::GetWin();
    win->ClearTimeout(WM_FLASH_CURSOR);
    bool flag{ false };
    if (lines.size() == 0 || lines[0].empty()) {
        flag = true;
    }
    else if (IsWip) {
        IsWip = false;
        auto canvasBack = win->surfaceBack->getCanvas();
        Paint(canvasBack);
        auto toolMain = ToolMain::Get();
        toolMain->SetUndoDisable(false);
    }
    auto canvasFront = win->surfaceFront->getCanvas();
    canvasFront->clear(SK_ColorTRANSPARENT);
    win->refresh();
    return flag;
}
float ShapeText::getCursorX()
{
    auto font = App::GetFontText();
    float inputCursorX{ Rect.left() + 10 };
    if (wordIndex > 0) {
        SkRect lineRect;
        auto subStr = lines[lineIndex].substr(0, wordIndex);
        auto data = subStr.data();
        auto length = wcslen(data) * 2;
        font->measureText(data, length, SkTextEncoding::kUTF16, &lineRect);
        inputCursorX += lineRect.right();
    }
    return inputCursorX+1;
}
