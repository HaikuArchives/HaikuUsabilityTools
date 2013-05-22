#ifndef AUTOCOMPLETE_H
#define AUTOCOMPLETE_H

#include <InterfaceKit.h>

class AutoComplete : public BTextControl { 
public:
							AutoComplete(const char* label, const char* text=NULL, BMessage* message=NULL);
			virtual			~AutoComplete();
			void			Suggest(const char* text);
			//virtual void 	InsertText(const char* text, int32 length, int32 offset, const text_run_array* runs = NULL);
};

#endif
