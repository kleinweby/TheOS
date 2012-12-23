
#include "LinkerHelper.h"

#include "Multiboot/Multiboot.h"

LINKER_SYMBOL(KernelOffset);
LINKER_SYMBOL(KernelLength);

LINKER_SYMBOL(KernelStartupOffset);
LINKER_SYMBOL(KernelStartupLength);

LINKER_SYMBOL(KernelTextOffset);
LINKER_SYMBOL(KernelTextLength);

LINKER_SYMBOL(KernelDataOffset);
LINKER_SYMBOL(KernelDataLength);

void kmain()
{
	
}
