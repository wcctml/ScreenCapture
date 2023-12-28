#include "CutMask.h"
#include "WindowMain.h"
#include "include/core/SkColor.h"
#include "State.h"

CutMask* cutMask;

CutMask::CutMask()
{
    for (size_t i = 0; i < 8; i++)
    {
        masks.push_back(SkRect::MakeEmpty());
    }
}

CutMask::~CutMask()
{
}

void CutMask::init()
{
    if (!cutMask)
    {
        cutMask = new CutMask();
    }
}

CutMask* CutMask::get()
{
    return cutMask;
}

bool CutMask::OnMouseDown(const int& x, const int& y)
{    
    start.set(x, y);
    return false;
}
bool CutMask::OnMouseMove(const int& x, const int& y)
{
    return true;
}
bool CutMask::OnMouseDrag(const int& x, const int& y)
{
    auto win = WindowMain::get();
    if (win->state != State::mask)
    {
        return false;
    }
    CutRect.setLTRB(start.x(), start.y(), x, y);
    CutRect.sort();
    //path.reset();
    //path.addRect(SkRect::MakeXYWH(0, 0, win->w, win->h));
    //path.addRect(CutRect);
    //path.setFillType(SkPathFillType::kEvenOdd);
    masks[0].setLTRB(0, 0, CutRect.fLeft, CutRect.fTop);
    masks[1].setLTRB(CutRect.fLeft, 0, CutRect.fRight, CutRect.fTop);
    masks[2].setLTRB(CutRect.fRight, 0, win->w, CutRect.fTop);
    masks[3].setLTRB(CutRect.fRight, CutRect.fTop, win->w, CutRect.fBottom);
    masks[4].setLTRB(CutRect.fRight, CutRect.fBottom, win->w, win->h);
    masks[5].setLTRB(CutRect.fLeft, CutRect.fBottom, CutRect.fRight, win->h);
    masks[6].setLTRB(0, CutRect.fBottom, CutRect.fLeft, win->h);
    masks[7].setLTRB(0, CutRect.fTop, CutRect.fLeft, CutRect.fBottom);
    win->Refresh();
    return true;
}
bool CutMask::OnPaint(SkCanvas *canvas)
{
    SkPaint paint;
    //canvas->drawPath(path, paint);
    paint.setColor(SkColorSetARGB(160, 0, 0, 0));
    for (auto& mask :masks)
    {
        canvas->drawRect(mask, paint);
    }
    paint.setColor(SkColorSetARGB(255, 22, 118, 255));
    paint.setStrokeWidth(3);
    paint.setStyle(SkPaint::Style::kStroke_Style);
    canvas->drawRect(CutRect, paint);
    return false;
}
bool CutMask::OnMouseUp(const int& x, const int& y)
{
    auto winMain = WindowMain::get();
    if (winMain->state != State::mask) {
        return false;
    }
    winMain->state = State::tool;
    winMain->Refresh();
    return true;
}