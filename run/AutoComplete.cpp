#include "AutoComplete.h"

AutoComplete::AutoComplete(const char* name)
	:	BTextView(name) 
{
}

AutoComplete::~AutoComplete() {
}

void
AutoComplete::InsertText(const char* text, int32 length, int32 offset, const text_run_array* runs = NULL) {
	BTextView::InsertText(text, length, offset, runs);
	BAlert* alert = new BAlert("Text Changed To", Text(), "Gotcha");
	alert->Go(); 
}
