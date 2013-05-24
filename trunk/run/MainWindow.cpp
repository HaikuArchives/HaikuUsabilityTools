#include "MainWindow.h"
#include "IconView.h"

#include <Application.h>
#include <Bitmap.h>
#include <Catalog.h>
#include <ControlLook.h>
#include <File.h>
#include <FilePanel.h>
#include <GridLayout.h>
#include <GridLayoutBuilder.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <Locale.h>
#include <Roster.h>
#include <StorageKit.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const uint32 MSG_RUN			= 'mRUN';
const uint32 MSG_BROWSE			= 'mBRO';
const uint32 MSG_CHANGED		= 'mTXC';

const char *kTrackerSignature	= "application/x-vnd.Be-TRAK";
const char *kTerminalSignature	= "application/x-vnd.Haiku-Terminal";

//#define DEBUG(message) printf(message);
//#define DEBUG(message) //

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Run Window"

void DEBUG(const char *message) {
	printf("%s\n", message);
}

MainWindow::MainWindow(void)
	:	BWindow(BRect(100,100,500,200),B_TRANSLATE("Run"),B_TITLED_WINDOW_LOOK, 
		B_NORMAL_WINDOW_FEEL, 
		B_NOT_ZOOMABLE | B_CLOSE_ON_ESCAPE |
		B_NOT_MINIMIZABLE | B_NOT_RESIZABLE | 
		B_ASYNCHRONOUS_CONTROLS),
		fTargetPanel(NULL)
{
	BGroupLayout* layout = new BGroupLayout(B_VERTICAL, 0);
	SetLayout(layout);
	
	fRunButton = new BButton(B_TRANSLATE("Run"), new BMessage(MSG_RUN));
	fBrowseButton = new BButton(B_TRANSLATE("Browse" B_UTF8_ELLIPSIS), new BMessage(MSG_BROWSE));
	
	fTargetText = new AutoComplete(B_TRANSLATE("Command to run:"), NULL, new BMessage(MSG_CHANGED));
	fTargetText->SetModificationMessage(new BMessage(MSG_CHANGED));
		
	fIconView = new IconView();
	
	BView* topView = layout->View();
	const float spacing = be_control_look->DefaultItemSpacing();
	topView->AddChild(BGroupLayoutBuilder(B_VERTICAL, spacing)
		.AddGroup(B_VERTICAL, spacing)
			.Add(BGridLayoutBuilder()
				.Add(fIconView, 0, 0)
				.Add(fTargetText, 1, 0, 6, 1)
			)
			.AddGroup(B_HORIZONTAL, 5.0)
				.Add(fUseTerminal = new BCheckBox(NULL, B_TRANSLATE("Open in a terminal"), new BMessage(MSG_CHANGED)))
				.Add(fRunButton)
				.Add(fBrowseButton)
			.End()
		.End()		
		.SetInsets(spacing, spacing, spacing, spacing)
	);
	
	fTargetText->MakeFocus(true);
	fRunButton->MakeDefault(true);
	CenterOnScreen();
	Show();
}

MainWindow::~MainWindow() {
	delete fTargetPanel;
}

void
MainWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case MSG_CHANGED:
		{
			_ParseTarget();
			break;
		}
		
		case MSG_BROWSE:
		{
			BEntry entry(fTargetText->Text(), true);
			entry_ref srcRef;
			if (entry.Exists() && entry.IsDirectory())
				entry.GetRef(&srcRef);
			if (!fTargetPanel) {
				BMessenger messenger(this);
				fTargetPanel = new BFilePanel(B_OPEN_PANEL, &messenger, &srcRef, B_FILE_NODE, false);
				(fTargetPanel->Window())->SetTitle(B_TRANSLATE("Run: Open"));
			} else
				fTargetPanel->SetPanelDirectory(&srcRef);
			fTargetPanel->Show();
			break;
		}
		
		case MSG_RUN:
		{			
			if (_Launch() == B_OK) 
				QuitRequested();
			else {
				BAlert* alert = new BAlert(B_TRANSLATE("Can't Run That"), B_TRANSLATE("Sorry. We couldn't find the resource you're trying to run."), "Try Again", NULL, NULL, B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_WARNING_ALERT);
				alert->SetShortcut(0, B_ESCAPE);
				alert->Go();
			}
			break;
		}
		
		case B_REFS_RECEIVED: 
		{
			entry_ref entryRef;
			msg->FindRef("refs", &entryRef);
			BEntry entry(&entryRef);
			BPath path;
			if (entry.GetPath(&path) == B_OK)
				fTargetText->SetText(path.Path());
			break;
		}
		
		default:
		{
			//msg->PrintToStream();
			BWindow::MessageReceived(msg);
			break;
		}
	}
}


bool
MainWindow::QuitRequested(void)
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}

status_t
MainWindow::_OpenFile(const char* openWith, BEntry &entry, int32 line = -1, int32 col = -1)
{
	entry_ref ref;
	status_t status = entry.GetRef(&ref);
	if (status < B_OK)
		return status;

	BMessenger target(openWith ? openWith : kTrackerSignature);
	if (!target.IsValid())
		return be_roster->Launch(&ref);

	BMessage message(B_REFS_RECEIVED);
	message.AddRef("refs", &ref);
	if (line > -1)
		message.AddInt32("be:line", line);
	if (col > -1)
		message.AddInt32("be:column", col);

	// tell the app to open the file
	return target.SendMessage(&message);
}


int
MainWindow::_Launch()
{
	int exitcode = EXIT_SUCCESS;
	const char *openWith = NULL;

	status_t status = B_OK;
	
	if (fUseTerminal->Value() == B_CONTROL_ON) {
		char* argv[] = {(char*)fTargetText->Text()};
		status = be_roster->Launch(kTerminalSignature, 1, argv);
	} 
	
	BEntry entry(fTargetText->Text());
	if ((status = entry.InitCheck()) == B_OK && entry.Exists()) {
		status = _OpenFile(openWith, entry);
	} else if (!strncasecmp("application/", fTargetText->Text(), 12)) {
		// maybe it's an application-mimetype?

		// subsequent files are open with that app
		openWith = fTargetText->Text();

		// in the case the app is already started, 
		// don't start it twice if we have other args
		BList teams;
		be_roster->GetAppList(fTargetText->Text(), &teams);

		if (teams.IsEmpty())
			status = be_roster->Launch(fTargetText->Text());
		else
			status = B_OK;
	} else if (strchr(fTargetText->Text(), ':')) {
		// try to open it as an URI
		//BPrivate::Support::BUrl url(fTargetText->Text());
		//if (url.OpenWithPreferredApplication() == B_OK)
		//	return B_OK;

		// maybe it's "file:line" or "file:line:col"
		int line = 0, col = 0, i;
		status = B_ENTRY_NOT_FOUND;
		// remove gcc error's last :
		BString arg(fTargetText->Text());
		if (arg[arg.Length() - 1] == ':')
			arg.Truncate(arg.Length() - 1);

		i = arg.FindLast(':');
		if (i > 0) {
			line = atoi(arg.String() + i + 1);
			arg.Truncate(i);

			status = entry.SetTo(arg.String());
			if (status == B_OK && entry.Exists()) {
				status = _OpenFile(openWith, entry, line);
				if (status == B_OK)
					return status;
			}

			// get the column
			col = line;
			i = arg.FindLast(':');
			line = atoi(arg.String() + i + 1);
			arg.Truncate(i);

			status = entry.SetTo(arg.String());
			if (status == B_OK && entry.Exists())
				status = _OpenFile(openWith, entry, line, col);
		}
	} else {
		// scan through the path
		char * path = strtok(getenv("PATH"), ":");

		while (path != NULL) {
			BString fullname(path);
			fullname.Append("/");
			fullname.Append(fTargetText->Text());
			entry = BEntry(fullname.String(), false);
			if ((status = entry.InitCheck()) == B_OK && entry.Exists()) {
				status = _OpenFile(openWith, entry);
				break;
			} 
			path = strtok(NULL, ":");
		}
		
		if (path == NULL)
			status = B_ENTRY_NOT_FOUND;
	}

	if (status != B_OK && status != B_ALREADY_RUNNING) {
		fprintf(stderr, "%s: \"%s\": %s\n", "Run", fTargetText->Text(),
			strerror(status));
		// make sure the shell knows this
		exitcode = EXIT_FAILURE;
	}
	
	return exitcode;
}

char*
_whereis(const char* target) {
	DEBUG("Scanning path for");
	DEBUG(target);
	// scan through the path
	char *path = strdup(getenv("PATH"));
	DEBUG("The path is");
	DEBUG(path);
	path = strtok(path, ":");

	while (path != NULL) {
		BString fullname(path);
		fullname.Append("/");
		fullname.Append(target);
		DEBUG("Looking for");
		DEBUG(fullname.String());
		BEntry entry(fullname.String(), false);
		if ((entry.InitCheck()) == B_OK && entry.Exists()) {
			//delete path;
			DEBUG("Located at");
			DEBUG(fullname);
			return (char*) fullname.String();
		} 
		path = strtok(NULL, ":");
	}
	return NULL;
}

bool
MainWindow::_TestTarget(const char* target) {
	BEntry app, entry(target);	
	entry_ref appRef, entryRef;
	DEBUG("Does it exist? ");
	if ((entry.InitCheck()) == B_OK && entry.Exists()) {
		DEBUG("Such a file exists. ");
		status_t status = entry.GetRef(&entryRef) && app.GetRef(&appRef);
		if (status == B_OK) {
			DEBUG("Got the refs ");
			if (be_roster->FindApp(&entryRef, &appRef) == B_OK) {
				DEBUG("Found app for ");
				app_info info;
				if (be_roster->GetAppInfo(&appRef, &info) == B_OK) {
					DEBUG("Do setting icon");
					fIconView->SetIcon(&info);
					fUseTerminal->SetValue(B_CONTROL_OFF);
					return true;
				} else {
					fIconView->SetIcon(kTerminalSignature);
					fUseTerminal->SetValue(B_CONTROL_ON);
					return true;
				}
			}
		}
	}
	fIconView->SetDefault();
	return false;
}

void
MainWindow::_ParseTarget()
{
	DEBUG("Parsing target");
	if (fUseTerminal->Value() == B_CONTROL_ON) {
		fIconView->SetIcon(kTerminalSignature);
		return;
	}

	if (_TestTarget(fTargetText->Text())) {
		return;
	} else if (!strchr(fTargetText->Text(), '/')) {
		if (_TestTarget(_whereis(fTargetText->Text()))) {
			return;
		}
	}
	
	fIconView->SetDefault();
	return;
}
