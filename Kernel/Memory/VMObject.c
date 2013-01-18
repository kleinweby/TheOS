
#import "VMObject.h"

#import "Error/Assert.h"

bool VMObjectInitialize(VMObject* obj,
		VMSection* section,
		offset_t base,
		size_t size,
		void (*addToContext)(VMObject* object, VMContext* context),
		void (*removeFromContext)(VMObject* object, VMContext* context),
		char* (*description)(VMObject* object),
		void (*dealloc)(VMObject* object))
{
	assert(obj);
	assert(section);
	assert(base);
	assert(size);
	assert(addToContext);
	assert(removeFromContext);
	assert(description);
	assert(dealloc);
	
	obj->section = section;
	obj->base = base;
	obj->size = size;
	obj->addToContext = addToContext;
	obj->removeFromContext = removeFromContext;
	obj->description = description;
	obj->dealloc = dealloc;
	
	return true;
}
