#ifndef ICONVIEW_H
#define ICONVIEW_H

#include <Application.h>
#include <InterfaceKit.h>
#include <Roster.h>
#include <StorageKit.h>

class IconView : public BView { 
public:
	 				IconView();
	virtual void	AttachedToWindow();
	virtual void	Draw(BRect updateRect);
	void			SetIcon(BBitmap* icon);
	void			SetIcon(app_info* info);
	void			SetIcon(const char* signature);
	void			SetDefault();
private:
 	BBitmap*		fIconBitmap;
};

#endif

