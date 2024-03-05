﻿#include "ShapeBase.h"
#include "../App.h"
#include "../WindowBase.h"
#include "../Recorder.h"

ShapeBase::ShapeBase(const int& x, const int& y):startX{x},startY{y}
{
}

ShapeBase::~ShapeBase()
{
}

bool ShapeBase::MouseInDragger(const int& x, const int& y)
{
    for (size_t i = 0; i < Draggers.size(); i++)
    {
        if (Draggers[i].contains(x, y)) {
            HoverIndex = i;
            if (IsDraggerVisible) {
                Cursor::SetCursor(DraggerCursors[HoverIndex]);
            }
            return true;
        }
    }
    return false;
}

void ShapeBase::HideDragger()
{
    HoverIndex = -1;
    auto win = App::GetWin();
    auto canvas = win->surfaceFront->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    win->Refresh();
    Cursor::Cross();
    IsDraggerVisible = false;
}

void ShapeBase::ShowDragger()
{
    auto win = App::GetWin();
    auto canvas = win->surfaceFront->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    SkPaint paint;
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    paint.setColor(SK_ColorBLACK);
    for (auto& dragger : Draggers) {
        canvas->drawRect(dragger, paint);
    }
    OnShowDragger(canvas);
    win->Refresh();
    Cursor::SetCursor(DraggerCursors[HoverIndex]);
    IsDraggerVisible = true;
}
