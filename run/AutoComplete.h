#ifndef AUTOCOMPLETE_H
#define AUTOCOMPLETE_H

#include <InterfaceKit.h>

class AutoComplete : public BTextView { 
public:
							AutoComplete(const char* name);
			virtual			~AutoComplete();
			virtual void 	InsertText(const char* text, int32 length, int32 offset, const text_run_array* runs = NULL);
};

#endif
