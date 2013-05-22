#include "AutoComplete.h"

AutoComplete::AutoComplete(const char* label, const char* text=NULL, BMessage* message=NULL)
	:	BTextControl(NULL, label, text, NULL) 
{
	SetModificationMessage(message);
}

AutoComplete::~AutoComplete() {
//	SetModificationMessage(NULL);
}

void
AutoComplete::Suggest(const char* text) {
	// We'll have to do this `a la google, with a menu/combobox
	//SetText(text);
}
