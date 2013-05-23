#include "IconView.h"

IconView::IconView()
 : BView(BRect(0, 0, 31, 31), NULL, B_FOLLOW_ALL, B_WILL_DRAW),
 fIconBitmap(NULL)
{
	SetDefault();
}

void IconView::AttachedToWindow() {
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

void IconView::SetDefault() {
	app_info info;
	if (be_app->GetAppInfo(&info) == B_OK) {
		SetIcon(&info);
	}
}

void IconView::Draw(BRect updateRect) {
	if (fIconBitmap) {
		SetDrawingMode(B_OP_OVER);
		DrawBitmapAsync(fIconBitmap); 
	}
}

void IconView::SetIcon(BBitmap* icon) {
	if (fIconBitmap)
		delete fIconBitmap;
	fIconBitmap = icon;
	Invalidate();
}

void IconView::SetIcon(app_info* info) {
	BBitmap* bitmap = new BBitmap(BRect(0, 0, 31, 31), B_RGBA32);
	BAppFileInfo appFileInfo(new BFile(&(info->ref), B_READ_ONLY));
	if (appFileInfo.GetTrackerIcon(bitmap) == B_OK)
		SetIcon(bitmap);
	else
		SetDefault();
}

void IconView::SetIcon(const char* signature) {
	app_info info;
	if (be_roster->GetAppInfo(signature, &info) == B_OK)
		SetIcon(&info);
	else
		SetDefault();
}
