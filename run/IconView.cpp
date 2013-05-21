#include "IconView.h"

IconView::IconView(BRect rect, BBitmap* icon)
 : BView(rect, NULL, B_FOLLOW_ALL, B_WILL_DRAW),
 fIconBitmap(icon)
{
}

void IconView::AttachedToWindow() {
	SetFont(be_bold_font);
}

void IconView::Draw(BRect updateRect) {
	if (fIconBitmap) {
		SetDrawingMode(B_OP_OVER);
		DrawBitmapAsync(fIconBitmap); 
	}
}

void IconView::SetIcon(BBitmap* icon) {
	fIconBitmap = icon;
}
