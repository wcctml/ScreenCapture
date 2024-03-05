#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "ToolBase.h"

class ToolMain : public ToolBase
{
public:
    ~ToolMain();
    static void Init();
    static ToolMain *Get();
    bool OnMouseDown(const int& x, const int& y) override;
    void SetPositionByCutMask();
    void SetPosition(const float& x,const float& y);
    bool OnPaint(SkCanvas *canvas) override;
    void SetUndoDisable(bool flag);
    void SetRedoDisable(bool flag);
    void InitBtns();
    void UnSelectAndHoverAll();
private:
    ToolMain();
    /// <summary>
    /// 0����������ѡ�л�ȡ��ѡ��ĳ����ťʱ����Ҫ�ı�λ��
    /// 1ѡ��ĳ����ťʱ��λ������
    /// 2ȡ��ѡ��ĳ����ťʱ��λ������
    /// </summary>
    bool topFlag{ false };
};