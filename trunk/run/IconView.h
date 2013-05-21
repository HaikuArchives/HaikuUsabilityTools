#ifndef ICONVIEW_H
#define ICONVIEW_H

#include <InterfaceKit.h>

class IconView : public BView { 
public:
	 				IconView(BRect frame, BBitmap* icon);
	virtual void	AttachedToWindow();
	virtual void	Draw(BRect updateRect);
	void			SetIcon(BBitmap* icon);
private:
 	BBitmap*	fIconBitmap;
};

#endif

