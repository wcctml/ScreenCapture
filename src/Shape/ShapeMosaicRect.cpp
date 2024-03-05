#include "ShapeMosaicRect.h"
#include "include/core/SkPathEffect.h"
#include "include/effects/SkDashPathEffect.h"
#include "../App.h"
#include "../WindowBase.h"
#include "../ToolSub.h"
#include "../Cursor.h"
#include "../ColorBlender.h"
#include "../Recorder.h"

ShapeMosaicRect::ShapeMosaicRect(const int &x, const int &y) : ShapeRect(x, y)
{
    rect = SkRect::MakeXYWH(x, y, 0, 0);
    HoverIndex = 4;
}

ShapeMosaicRect::~ShapeMosaicRect()
{
}

bool ShapeMosaicRect::OnMouseDown(const int& x, const int& y)
{
    startX = x;
    startY = y;
    return false;
}

void ShapeMosaicRect::OnShowDragger(SkCanvas* canvas)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    SkScalar intvls[] = { 6, 6 };
    auto effect = SkDashPathEffect::Make(intvls, 2, 0);
    paint.setPathEffect(effect);
    paint.setColor(SK_ColorBLACK);
    canvas->drawRect(rect, paint);
}

void ShapeMosaicRect::drawRectsByPoints(SkCanvas* canvas)
{
    auto rect = this->rect;
    rect.sort();
    auto win = App::GetWin();
    int columnNum = std::ceil((float)win->w / size); //������Ļ�����˵�����
    int rectColumnNum = std::ceil(rect.width() / size) + 1; //����������������
    int rectRowNum = std::ceil(rect.height() / size) + 1; //����������������
    int xIndex = rect.fLeft / size; //����������һ������������Ļ�����ڵڼ���
    int yIndex = rect.fTop / size; //������������һ������������Ļ�����ڵڼ���
    xIndex = xIndex < 0 ? 0 : xIndex;
    yIndex = yIndex < 0 ? 0 : yIndex;
    SkColor4f colorSum = { 0, 0, 0, 0 };
    SkPaint paint;
    for (size_t i = yIndex; i < yIndex + rectRowNum; i++)
    {
        for (size_t j = xIndex; j < xIndex + rectColumnNum; j++)
        {
            int key = i * columnNum + j;
            auto x = j * size;
            auto y = i * size;
            int count{ 0 };
            for (size_t x1 = x; x1 <= x + size; x1 += 2)
            {
                for (size_t y1 = y; y1 <= y + size; y1 += 2)
                {
                    SkColor4f currentColor;
                    if (x1 >= canvas->getSurface()->width() || y1 >= canvas->getSurface()->height()) {
                        currentColor = SkColor4f::FromColor(SK_ColorBLACK);
                    }
                    else {
                        auto colorSrc = win->pixSrc->getColor(x1, y1);
                        auto colorBack = win->pixBack->getColor(x1, y1);
                        if (colorBack == SK_ColorTRANSPARENT) {
                            currentColor = SkColor4f::FromColor(colorSrc);
                        }
                        else {
                            currentColor = ColorBlender::Blend(colorSrc, colorBack);
                        }
                    }
                    colorSum.fR += currentColor.fR;
                    colorSum.fG += currentColor.fG;
                    colorSum.fB += currentColor.fB;
                    count++;
                }
            }
            colorSum.fR /= count;
            colorSum.fG /= count;
            colorSum.fB /= count;
            colorSum.fA = 255;
            auto color = colorSum.toSkColor();
            paint.setColor(color);
            canvas->drawRect(SkRect::MakeXYWH(x, y, size, size), paint);
        }
    }
}

void ShapeMosaicRect::Paint(SkCanvas *canvas)
{
    auto win = App::GetWin();
    if (win->IsMouseDown && Recorder::Get()->CurShape == this) {
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStroke(true);
        paint.setStrokeWidth(1);
        SkScalar intvls[] = { 6, 6 };
        auto effect = SkDashPathEffect::Make(intvls, 2, 0);
        paint.setPathEffect(effect);
        paint.setColor(SK_ColorBLACK);
        canvas->drawRect(rect, paint);
    }

    canvas->saveLayer(nullptr, nullptr);
    drawRectsByPoints(canvas);
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStroke(false);
    paint.setBlendMode(SkBlendMode::kClear);
    auto rect = this->rect;
    rect.sort();
    auto s = canvas->getSurface();
    canvas->drawRect(SkRect::MakeLTRB(rect.fLeft, rect.fTop, s->width(), 0), paint);
    canvas->drawRect(SkRect::MakeLTRB(rect.fRight, rect.fTop, s->width(), s->height()), paint);
    canvas->drawRect(SkRect::MakeLTRB(rect.fRight, rect.fBottom, 0, s->height()), paint);
    canvas->drawRect(SkRect::MakeLTRB(rect.fLeft, rect.fBottom, 0, 0), paint);
    canvas->restore();
}

bool ShapeMosaicRect::OnMouseMove(const int& x, const int& y)
{
    if (MouseInDragger(x, y)) {
        return true;
    }
    if (rect.contains(x, y)) {
        HoverIndex = 8;
        return true;
    }
    return false;
}
