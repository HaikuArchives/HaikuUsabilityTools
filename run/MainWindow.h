#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "IconView.h"

#include <InterfaceKit.h>

class MainWindow : public BWindow
{
public:
						MainWindow(void);
			virtual		~MainWindow();
			void		MessageReceived(BMessage *msg);
			bool		QuitRequested(void);
			
private:
			BButton*		fRunButton;
			BButton*		fBrowseButton;			
			IconView*		fIconView;
			BTextView*		fTargetText;
			BFilePanel*		fTargetPanel;
			BCheckBox*		fUseTerminal;
			
			status_t		_OpenFile(const char* openWith, BEntry &entry, int32 line = -1, int32 col = -1);
			int				_Launch();
};

#endif
