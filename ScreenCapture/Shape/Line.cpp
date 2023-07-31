#include "Line.h"
namespace Shape {
	void Line::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
	{
		auto context = Painter::Get()->paintCtx;
		context->begin(*Painter::Get()->prepareImage);
		context->clearAll();
		context->setCompOp(BL_COMP_OP_SRC_OVER);
		if (isFill) {
			context->setStrokeStyle(color);
		}
		else
		{			
			context->setStrokeStyle(BLRgba32(color.r(), color.g(), color.b(), 110));
		}		
		context->setStrokeWidth(strokeWidth);
		context->setStrokeCaps(BL_STROKE_CAP_ROUND);
		context->strokeLine(x2, y2, x1, y1);
		context->end();
	}
}