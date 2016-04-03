#include <stdlib.h>
#include <string.h>

#include <ps4/elfloader.h>
#include <sys/mman.h>

#include <debugnet.h>

/* Defines */

#define elfRelocationSymbol __ELFN(R_SYM)
#define elfRelocationType __ELFN(R_TYPE)
#define elfRelocationInfo __ELFN(R_INFO)

#define elfSymbolBind __ELFN(ST_BIND)
#define elfSymbolType __ELFN(ST_TYPE)
#define elfSymbolInfo __ELFN(ST_INFO)

#define elfIsElf(e) IS_ELF(*elfHeader(e)) // FIXME: Null deref

#define elfClass(e) (e == NULL ? 0 : e->data[4])
#define elfEncoding(e) (e == NULL ? 0 : e->data[5])
#define elfVersion(e) (e == NULL ? 0 : e->data[6])
#define elfABI(e) (e == NULL ? 0 : e->data[7])

/* Constants */

enum{ ElfMaximalStringLength = 4096 };

/* Type */

typedef struct Elf
{
	uint8_t *data;
	uint64_t size; // FIXME: Do more checks on size
}
Elf;

/* --- elf header --- */

ElfHeader *elfHeader(Elf *elf)
{
	if(!elf)
		return NULL;
	return (ElfHeader *)elf->data;
}

uint64_t elfEntry(Elf *elf)
{
	if(!elf)
		return 0;
	ElfHeader *h = elfHeader(elf);
	if(!h)
		return 0;
	return h->e_entry;
}

uint64_t elfLargestAlignment(Elf *elf) //ignore ...
{
	uint16_t index = 0;
	uint64_t alignment = 0;

	while(1)
	{
		ElfSegment *h = elfSegment(elf, &index, ElfSegmentAttributeType, PT_LOAD);
		if(!h)
			break;

		// FIXME: Tired of bogus 2MB alignment -> ignore
		if(alignment < h->p_align)// && h->p_align < 0x200000)
			alignment = h->p_align;
		++index;
	}
	return alignment;
}

size_t elfMemorySize(Elf *elf)
{
	ElfSection *sections;
	ElfSegment *segments;

	uint16_t size;
	uint16_t length;
	uint16_t index;

	size_t memorySize = 0;

	if(!elf)
		return 0;

	segments = elfSegments(elf, &size, &length);
	if(segments)
	{
		for(index = 0; index < length; ++index)
		{
			ElfSegment *s = (ElfSegment *)((uint8_t *)segments + index * size);
			if(memorySize < s->p_paddr + s->p_memsz)
				memorySize = s->p_paddr + s->p_memsz;
		}
	}
	else
	{
		length = 0;
		sections = elfSections(elf, &size, &length);
		if(!sections)
			return 0;
		for(index = 0; index < length; ++index)
		{
			ElfSection *s = (ElfSection *)((uint8_t *)sections + index * size);
			if(memorySize < s->sh_addr + s->sh_size)
				memorySize = s->sh_addr + s->sh_size;
		}
	}
	debugNetPrintf(DEBUG,"[PS4LINK] protectedMemoryCreate(%zu) -> ", memorySize);
	return memorySize;
}

/* --- elf section header --- */

char *elfSectionStrings(Elf *elf, uint64_t *size)
{
	ElfHeader *h;
	uint16_t i;
	ElfSection *s;
 	h = elfHeader(elf);
	i = h->e_shstrndx;
	s = elfSection(elf, &i, ElfSectionAttributeNone, 0);
	if(size)
		*size = s->sh_size;
	return (char *)elf->data + s->sh_offset;
}

uint64_t elfSectionAttribute(ElfSection *elfSection, ElfSectionAttribute attribute)
{
	switch(attribute)
	{
		case ElfSectionAttributeName:
			return elfSection->sh_name;
		case ElfSectionAttributeType:
			return elfSection->sh_type;
		case ElfSectionAttributeFlags:
			return elfSection->sh_flags;
		case ElfSectionAttributeAddress:
			return elfSection->sh_addr;
		case ElfSectionAttributeOffset:
			return elfSection->sh_offset;
		case ElfSectionAttributeSize:
			return elfSection->sh_size;
		case ElfSectionAttributeLink:
			return elfSection->sh_link;
		case ElfSectionAttributeInfo:
			return elfSection->sh_info;
		case ElfSectionAttributeMemoryAlignment:
			return elfSection->sh_addralign;
		case ElfSectionAttributeEntrySize:
			return elfSection->sh_entsize;
		default:
			break;
	}
	return 0;
}

ElfSection *elfSections(Elf *elf, uint16_t *size, uint16_t *length)
{
	ElfHeader *h;

	if(!elf)
		return NULL;

	h = elfHeader(elf);

	if(h->e_shoff == 0)
		return NULL;

	if(size != NULL)
		*size = h->e_shentsize;
	if(length != NULL)
		*length = h->e_shnum;

	return (ElfSection *)(elf->data + h->e_shoff);
}

ElfSection *elfSection(Elf *elf, uint16_t *index, ElfSectionAttribute attribute, uint64_t value)
{
	uint16_t size;
	uint16_t length;
	ElfSection *h, *t;
	uint16_t i = 0;

	if(!index)
 		index = &i;

	h = elfSections(elf, &size, &length);

	if(!h)
		return NULL;

	for(; *index < length; ++(*index))
	{
		t = (ElfSection *)((uint8_t *)h + *index * size);
		if(attribute == ElfSectionAttributeNone || elfSectionAttribute(t, attribute) == value)
			return t;
	}

	return NULL;
}

ElfSection *elfSectionByName(Elf *elf, char *name)
{
	uint64_t size;
	char *mem = elfSectionStrings(elf, &size);

	uint32_t offset = elfStringToOffset(mem, size, name);
	ElfSection *sh = elfSection(elf, NULL, ElfSectionAttributeName, offset);

	return sh;
}

/* --- elf segment header --- */

uint64_t elfSegmentAttribute(ElfSegment *elfSegment, ElfSegmentAttribute attribute)
{
	switch(attribute)
	{
		case ElfSegmentAttributeType:
			return elfSegment->p_type;
		case ElfSegmentAttributeFlags:
			return elfSegment->p_flags;
		case ElfSegmentAttributeOffset:
			return elfSegment->p_offset;
		case ElfSegmentAttributeVirtualAddress:
			return elfSegment->p_vaddr;
		case ElfSegmentAttributePhysicalAddress:
			return elfSegment->p_paddr;
		case ElfSegmentAttributeFileSize:
			return elfSegment->p_filesz;
		case ElfSegmentAttributeMemorySize:
			return elfSegment->p_memsz;
		case ElfSegmentAttributeAlignment:
			return elfSegment->p_align;
		default:
			break;
	}
	return 0;
}

ElfSegment *elfSegments(Elf *elf, uint16_t *size, uint16_t *length)
{
	ElfHeader *h;

	if(!elf)
		return NULL;

	h = elfHeader(elf);

	if(h->e_phoff == 0)
		return NULL;

	if(size != NULL)
		*size = h->e_phentsize;
	if(length != NULL)
		*length = h->e_phnum;

	return (ElfSegment *)(elf->data + h->e_phoff);
}

ElfSegment *elfSegment(Elf *elf, uint16_t *index, ElfSegmentAttribute attribute, uint64_t value)
{
	uint16_t size;
	uint16_t length;
	ElfSegment *h, *t;
	uint16_t i = 0;

	if(!index)
 		index = &i;

	h = elfSegments(elf, &size, &length);

	if(!h)
		return NULL;

	for(; *index < length; ++(*index))
	{
		t = (ElfSegment *)((uint8_t *)h + *index * size);
		if(attribute == ElfSegmentAttributeNone || elfSegmentAttribute(t, attribute) == value)
			return t;
	}

	return NULL;
}

/* --- elf dynamic section --- */

uint64_t elfDynamicAttribute(ElfDynamic *elfDynamic, ElfDynamicAttribute attribute)
{
	switch(attribute)
	{
		case ElfDynamicAttributeTag:
			return elfDynamic->d_tag;
		case ElfDynamicAttributeValue:
			return elfDynamic->d_un.d_val;
		case ElfDynamicAttributePointer:
			return elfDynamic->d_un.d_ptr;
		default:
			break;
	}
	return 0;
}

ElfDynamic *elfDynamics(Elf *elf, uint16_t *size, uint16_t *length)
{
	ElfSection *h;
	ElfSegment *h2;

	if(!elf)
		return NULL;

	if((h = elfSection(elf, NULL, ElfSectionAttributeType, SHT_DYNAMIC)))
	{
		if(size != NULL)
			*size = h->sh_entsize;
		if(length != NULL)
			*length = h->sh_size / h->sh_entsize;

		return (ElfDynamic *)(elf->data + h->sh_offset);
	}
	else if((h2 = elfSegment(elf, NULL, ElfSegmentAttributeType, SHT_DYNAMIC)))
	{
		if(size != NULL)
			*size = sizeof(ElfDynamic);
		if(length != NULL)
			*length = h2->p_filesz / sizeof(ElfDynamic);

		return (ElfDynamic *)(elf->data + h2->p_offset);
	}

	return NULL;
}

ElfDynamic *elfDynamic(Elf *elf, uint16_t *index, ElfDynamicAttribute attribute, uint64_t value)
{
	uint16_t size;
	uint16_t length;
	ElfDynamic *h, *t;
	uint16_t i = 0;

	if(!index)
 		index = &i;

	h = elfDynamics(elf, &size, &length);

	if(!h)
		return NULL;

	for(; *index < length; ++(*index))
	{
		t = (ElfDynamic *)((uint8_t *)h + *index * size);
		if(attribute == ElfDynamicAttributeNone || elfDynamicAttribute(t, attribute) == value)
			return t;
	}

	return NULL;
}

/* --- elf string tables --- */

char *elfStringFromIndex(char *mem, uint64_t size, uint32_t index)
{
	uint64_t i, j = 0;

	if(!mem)
		return NULL;

	if(index == 0)
		return mem;

	for(i = 0; i < size - 1; ++i)
		if(mem[i] == '\0' && ++j == index)
			return mem + i + 1;

	return NULL;
}

char *elfStringFromOffset(char *mem, uint64_t size, uint32_t offset)
{
	if(!mem || offset >= size)
		return NULL;

	return mem + offset;
}

uint32_t elfStringToOffset(char *mem, uint64_t size, char *str)
{
	uint64_t i, j;

	if(!str)
		return 0;

	for(i = 0; i < size; ++i)
	{
		for(j = 0; j < ElfMaximalStringLength && mem[i + j] == str[j]; ++j)
			if(str[j] == '\0')
				return i;
	}

	return 0;
}

uint32_t elfStringToIndex(char *mem, uint64_t size, char *str)
{
	uint64_t index, i, j;

	if(!str)
		return 0;

	index = 0;
	for(i = 0; i < size; ++i)
	{
		for(j = 0; j < ElfMaximalStringLength && mem[i + j] == str[j]; ++j)
			if(str[j] == '\0')
				return index;

		if(mem[i] == '\0')
			index++;
	}

	return 0;
}

/* --- elf relocations --- */

uint64_t elfAddendRelocationAttribute(ElfAddendRelocation *elfAddendRelocation, ElfAddendRelocationAttribute attribute)
{
	switch(attribute)
	{
		case ElfAddendRelocationAttributeInfo:
			return elfAddendRelocation->r_info;
		case ElfAddendRelocationAttributeOffset:
			return elfAddendRelocation->r_offset;
		case ElfAddendRelocationAttributeAddend:
			return elfAddendRelocation->r_addend;
		default:
			break;
	}
	return 0;
}

ElfAddendRelocation *elfAddendRelocations(Elf *elf, char *name, uint16_t *size, uint16_t *length)
{
	ElfSection *h;

 	h = elfSectionByName(elf, name);

	if(!h || h->sh_type != SHT_RELA)
		return NULL;

	if(size != NULL)
		*size = h->sh_entsize;
	if(length != NULL)
		*length = h->sh_size / h->sh_entsize;

	return (ElfAddendRelocation *)(elf->data + h->sh_offset);
}

// FIXME this is not performant, better to pass in the base ElfAddendRelocation *, size and length
/*
ElfAddendRelocation *elfAddendRelocation(Elf *elf, char *name, uint16_t *index, ElfAddendRelocationAttribute attribute, uint64_t value)
{
	uint16_t size;
	uint16_t length;
	ElfAddendRelocation *h, *t;
	uint16_t i = 0;

	if(!index)
 		index = &i;

	h = elfAddendRelocations(elf, name, &size, &length);

	if(!h)
		return NULL;

	for(; *index < length; ++(*index))
	{
		t = (ElfAddendRelocation *)((uint8_t *)h + *index * size);
		if(attribute == ElfAddendRelocationAttributeNone || elfAddendRelocationAttribute(t, attribute) == value)
			return t;
	}

	return NULL;
}
*/

/* --- elf symbols --- */

uint64_t elfSymbolAttribute(ElfSymbol *elfSymbol, ElfSymbolAttribute attribute)
{
	switch(attribute)
	{
		case ElfSymbolAttributeName:
			return elfSymbol->st_name;
		case ElfSymbolAttributeInfo:
			return elfSymbol->st_info;
		case ElfSymbolAttributeUnused:
			return elfSymbol->st_other;
		case ElfSymbolAttributeSectionIndex:
			return elfSymbol->st_shndx;
		case ElfSymbolAttributeValue:
			return elfSymbol->st_value;
		case ElfSymbolAttributeSize:
			return elfSymbol->st_size;
		default:
			break;
	}
	return 0;
}

ElfSymbol *elfSymbols(Elf *elf, char *name, uint16_t *size, uint16_t *length)
{
	ElfSection *h;

 	h = elfSectionByName(elf, name);

	if(!h || (h->sh_type != SHT_SYMTAB && h->sh_type != SHT_DYNSYM))
		return NULL;

	if(size != NULL)
		*size = h->sh_entsize;
	if(length != NULL)
		*length = h->sh_size / h->sh_entsize;

	return (ElfSymbol *)(elf->data + h->sh_offset);
}

/*
ElfSymbol *elfSymbol(Elf *elf, char *name, uint16_t *index, ElfSymbolAttribute attribute, uint64_t value)
{
	uint16_t size;
	uint16_t length;
	ElfSymbol *h, *t;
	uint16_t i = 0;

	if(!index)
 		index = &i;

	h = elfSymbols(elf, name, &size, &length);

	if(!h)
		return NULL;

	for(; *index < length; ++(*index))
	{
		t = (ElfSymbol *)((uint8_t *)h + *index * size);
		if(attribute == ElfSymbolAttributeNone || elfSymbolAttribute(t, attribute) == value)
			return t;
	}

	return NULL;
}*/

/* actions */

Elf *elfCreate(void *data, uint64_t size)
{
	Elf *elf, t;

	if(data == NULL)
		return NULL;

	debugNetPrintf(DEBUG,"[PS4LINK] in elfCreate\n");

	t.data = data;
	t.size = size;

	if(!elfIsElf(&t))
	{
		debugNetPrintf(DEBUG,"[PS4LINK] is not an elf\n");
		
		return NULL;
	}
 //	elf = malloc(sizeof(Elf));
	
	elf=mmap(NULL, sizeof(Elf), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	
	
	debugNetPrintf(DEBUG,"[PS4LINK] reserved memory for elf at %p \n",elf);
	
	elf->data = (uint8_t *)data;
	elf->size = size;

	return elf;
}

void *elfDestroy(Elf *elf)
{
	void *data;

	if(elf == NULL)
		return NULL;

	data = elf->data;
	munmap(elf, elf->size);
	
	return NULL;
}

void elfDestroyAndFree(Elf *elf)
{
	void *d;

	if(elf == NULL)
		return;

	d = elfDestroy(elf);
//	if(d)
		//free(d);
}

/* ---  --- */

int elfLoaderIsLoadable(Elf *elf)
{
	ElfHeader *h;

	if(!elfIsElf(elf))
		return 0;

	h = elfHeader(elf);

	return elfClass(elf) == ELFCLASS64 &&
		elfEncoding(elf) == ELFDATA2LSB &&
		elfVersion(elf) == EV_CURRENT &&
		(elfABI(elf) == ELFOSABI_SYSV || elfABI(elf) == ELFOSABI_FREEBSD) &&
		h->e_type == ET_DYN &&
		h->e_phoff != 0 &&
		h->e_shoff != 0 &&
		h->e_machine == EM_X86_64 &&
 		h->e_version == EV_CURRENT;
}

int elfLoaderInstantiate(Elf *elf, void *memory)
{
	ElfSection *sections;
	ElfSegment *segments;

	uint16_t size;
	uint16_t length;
	uint16_t index;

	if(elf == NULL)
		return ElfLoaderReturnElfNull;
 	if(memory == NULL)
		return ElfLoaderReturnWritableMemoryNull;

	segments = elfSegments(elf, &size, &length);
	if(segments)
	{
		for(index = 0; index < length; ++index)
		{
			ElfSegment *s = (ElfSegment *)((uint8_t *)segments + index * size);
			if(s->p_filesz)
				memcpy((char *)memory + s->p_paddr, elf->data + s->p_offset, s->p_filesz);
			if(s->p_memsz - s->p_filesz)
				memset((char *)memory + s->p_paddr + s->p_filesz, 0, s->p_memsz - s->p_filesz);
		}
	}
	else
	{
		length = 0;
		sections = elfSections(elf, &size, &length);
		if(!sections)
			return 0;
		for(index = 0; index < length; ++index)
		{
			ElfSection *s = (ElfSection *)((uint8_t *)sections + index * size);
			if(!(s->sh_flags & SHF_ALLOC))
				continue;
			if(s->sh_size)
				memcpy((char *)memory + s->sh_addr, elf->data + s->sh_offset, s->sh_size);
		}
	}

	return ElfLoaderReturnOK;
}

int elfLoaderRelativeAddressIsExecutable(Elf *elf, uint64_t address)
{
	ElfSection *sections;
	ElfSegment *segments;

	uint16_t size;
	uint16_t length;
	uint16_t index;

	if(elf == NULL)
		return 0;

	segments = elfSegments(elf, &size, &length);
	if(segments)
	{
		for(index = 0; index < length; ++index)
		{
			ElfSegment *s = (ElfSegment *)((uint8_t *)segments + index * size);
			if(address >= s->p_paddr && address <= s->p_paddr + s->p_memsz)
				return s->p_flags & PF_X;
		}
	}
	else
	{
		length = 0;
		sections = elfSections(elf, &size, &length);
		if(!sections)
			return ElfLoaderReturnNoSectionsOrSegments;
		for(index = 0; index < length; ++index)
		{
			ElfSection *s = (ElfSection *)((uint8_t *)sections + index * size);
			if(address >= s->sh_addr && address <= s->sh_addr + s->sh_size)
				return s->sh_flags & SHF_EXECINSTR;
		}
	}

	return 1; // FIXME: Recheck
}

int elfLoaderRelocate(Elf *elf, void *writable, void *executable)
{
	int i, j;

	uint16_t relocationSize = 0;
	uint16_t relocationsLength = 0;
	ElfAddendRelocation *relocations;

	uint16_t dynamicSymbolSize = 0;
	uint16_t dynamicSymbolsLength = 0;
	ElfSymbol *dynamicSymbols;

	char *rel[2] = {".rela.dyn", ".rela.plt"};

	if(elf == NULL)
		return ElfLoaderReturnElfNull;
 	if(writable == NULL)
		return ElfLoaderReturnWritableMemoryNull;
 	if(executable == NULL)
		return ElfLoaderReturnExecutableMemoryNull;

	dynamicSymbols = elfSymbols(elf, ".dynsym", &dynamicSymbolSize, &dynamicSymbolsLength);
	//symbols = elfSymbols(elf, ".symtab", &symbolSize, &symbolsLength);

	for(j = 0; j < sizeof(rel) / sizeof(rel[0]); ++j)
	{
		relocationsLength = 0;
		relocations = elfAddendRelocations(elf, rel[j], &relocationSize, &relocationsLength);

		for(i = 0; i < relocationsLength; ++i)
		{
			ElfSymbol *symbol;
			ElfAddendRelocation *relocation = (ElfAddendRelocation *)(((uint8_t *)relocations) + relocationSize * i);
			uint16_t relocationType = (uint16_t)elfRelocationType(relocation->r_info);
			uint16_t relocationSymbol = (uint16_t)elfRelocationSymbol(relocation->r_info);
			uint64_t *offset = (uint64_t*)((char *)writable + relocation->r_offset);
			uint64_t value = 0;

			switch(relocationType)
			{
				case R_X86_64_RELATIVE:
					value = relocation->r_addend;
					break;
				case R_X86_64_64:
					symbol = (ElfSymbol *)(((uint8_t *)dynamicSymbols) + dynamicSymbolSize * relocationSymbol);
					value = symbol->st_value + relocation->r_addend;
					break;
				case R_X86_64_JMP_SLOT:
				case R_X86_64_GLOB_DAT:
					symbol = (ElfSymbol *)(((uint8_t *)dynamicSymbols) + dynamicSymbolSize * relocationSymbol);
					value = symbol->st_value;
					break;
				default:
					return ElfLoaderReturnUnknownRelocation;
			}

			if(elfLoaderRelativeAddressIsExecutable(elf, value))
				*offset = (uint64_t)executable + value;
			else
				*offset = (uint64_t)writable + value;
		}
	}

	return ElfLoaderReturnOK;
}

int elfLoaderLoad(Elf *elf, void *writable, void *executable)
{
	int r = ElfLoaderReturnOK;

	if(elf == NULL)
		return ElfLoaderReturnElfNull;
 	if(writable == NULL)
		return ElfLoaderReturnWritableMemoryNull;
 	if(executable == NULL)
		return ElfLoaderReturnExecutableMemoryNull;

	if(!elfLoaderIsLoadable(elf))
		return ElfLoaderReturnIsNotLoadable;

	if((r = elfLoaderInstantiate(elf, writable)) != ElfLoaderReturnOK)
		return r;

	r = elfLoaderRelocate(elf, writable, executable);

	return r;
}
