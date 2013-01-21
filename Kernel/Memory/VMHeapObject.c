

#import "VMHeapObject.h"

#import "Error/Assert.h"
#import "PhyMem.h"
#import "kalloc.h"

DEFINE_CLASS(VMHeapObject, VMObject,
	
	// How many pages are allocated
	uint32_t allocatedPages;
	
	// here we need to save all allocated pages
	void* pages[];
	);

static void VMHeapObjectAddToContextImpl(VMObject object, VMContext context);
static void VMHeapObjectRemoveFromContextImpl(VMObject object, VMContext context);
static char* VMHeapObjectDescriptionImpl(VMObject object);
static void VMHeapObjectDeallocImpl(void* object);

VMHeapObject VMHeapObjectCreate(VMSection section, offset_t base, size_t size)
{
	assert(section);
	assert(base > 0);
	assert(size % kPhyMemPageSize == 0);
	
	VMHeapObject obj = kalloc(sizeof(struct _VMHeapObject) + size/kPhyMemPageSize);
	
	if (!obj)
		return NULL;
	
	if (!VMObjectInitialize((void*)obj, section, base, size, VMHeapObjectAddToContextImpl,
		VMHeapObjectRemoveFromContextImpl, VMHeapObjectDescriptionImpl,
		VMHeapObjectDeallocImpl))
		return NULL;
	
	obj->allocatedPages = 0;
	if (!PhyMemAlloc(obj->pages[obj->allocatedPages - 1]))
		return NULL;
	
	return obj;
}

bool VMHeapObjectExpand(VMHeapObject obj, size_t size)
{
	uint32_t pagesNeeded = size/kPhyMemPageSize;
	// Make the size a multiple of the page size
	if (size % kPhyMemPageSize != 0)
		pagesNeeded++;
	
	if (obj->super.size/kPhyMemPageSize < obj->allocatedPages + pagesNeeded)
		return false;
	
	while (pagesNeeded < obj->allocatedPages) {
		if (!PhyMemAlloc(obj->pages[obj->allocatedPages]))
			return false;
		obj->allocatedPages++;
	}
	
	return true;
}

static void VMHeapObjectAddToContextImpl(VMObject object, VMContext context)
{
	#pragma unused(object, context)
}

static void VMHeapObjectRemoveFromContextImpl(VMObject object, VMContext context)
{
	#pragma unused(object, context)
}

static char* VMHeapObjectDescriptionImpl(VMObject object)
{
	#pragma unused(object)
	return NULL;
}

static void VMHeapObjectDeallocImpl(void* object)
{
	#pragma unused(object)
}
