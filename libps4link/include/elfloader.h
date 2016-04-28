#ifndef ElfLoader_H
#define ElfLoader_H

#include <stdint.h>

/* Warning, written for 64bit systems - need to adapt stuff for 32 */
#if ULONG_MAX == 0xffffffff
#define __ELF_WORD_SIZE 32
#error Elf loader currently does not support 32 bit arches
#else
#define __ELF_WORD_SIZE 64
#endif

#include <elf.h>

/* Defines which may be missing*/

#ifndef __ElfN
	#define __ElfNC_(x,y) x ## y
	#define __ElfNC(x,y)  __ElfNC_(x,y)
	#define	__ElfN(x) __ElfNC(__ElfNC(__ElfNC(Elf,__ELF_WORD_SIZE),_),x)
#endif
#ifndef __ELFN
	#define __ELFNC_(x,y) x ## y
	#define __ELFNC(x,y)  __ElfNC_(x,y)
	#define	__ELFN(x) __ElfNC(__ElfNC(__ElfNC(ELF,__ELF_WORD_SIZE),_),x)
#endif

#ifndef R_X86_64_JMP_SLOT
	#define R_X86_64_JMP_SLOT 7
#endif

#ifndef IS_ELF
	#define IS_ELF(ehdr) \
		((ehdr).e_ident[EI_MAG0] == ELFMAG0 && \
		(ehdr).e_ident[EI_MAG1] == ELFMAG1 && \
		(ehdr).e_ident[EI_MAG2] == ELFMAG2 && \
		(ehdr).e_ident[EI_MAG3] == ELFMAG3)
#endif

/* Neat Types */

typedef __ElfN(Ehdr) ElfHeader;
typedef __ElfN(Phdr) ElfSegment;
typedef __ElfN(Shdr) ElfSection;
typedef __ElfN(Sym) ElfSymbol;
//typedef Elf_Rel ElfRelocation;
typedef __ElfN(Rela) ElfAddendRelocation;
typedef __ElfN(Dyn) ElfDynamic;

/* Type */

typedef struct Elf Elf;

/* Enums*/

typedef enum
{
	ElfLoaderReturnOK = 0,
	ElfLoaderReturnElfNull = -1,
	ElfLoaderReturnWritableMemoryNull = -2,
	ElfLoaderReturnExecutableMemoryNull = -3,
	ElfLoaderReturnNoElf = -4,
	ElfLoaderReturnUnknownRelocation = -5,
	ElfLoaderReturnAddressNotFound = -6,
	ElfLoaderReturnNoSectionsOrSegments = -7,
	ElfLoaderReturnIsNotLoadable = -8
}
ElfLoaderReturn;

typedef enum
{
	ElfSectionAttributeNone = 0,
	ElfSectionAttributeName,
	ElfSectionAttributeType,
	ElfSectionAttributeFlags,
	ElfSectionAttributeAddress,
	ElfSectionAttributeOffset,
	ElfSectionAttributeSize,
	ElfSectionAttributeLink,
	ElfSectionAttributeInfo,
	ElfSectionAttributeMemoryAlignment,
	ElfSectionAttributeEntrySize,
}
ElfSectionAttribute;

typedef enum
{
	ElfSegmentAttributeNone = 0,
	ElfSegmentAttributeType,
	ElfSegmentAttributeFlags,
	ElfSegmentAttributeOffset,
	ElfSegmentAttributeVirtualAddress,
	ElfSegmentAttributePhysicalAddress,
	ElfSegmentAttributeFileSize,
	ElfSegmentAttributeMemorySize,
	ElfSegmentAttributeAlignment,
}
ElfSegmentAttribute;

typedef enum
{
	ElfDynamicAttributeNone = 0,
	ElfDynamicAttributeTag,
	ElfDynamicAttributeValue,
	ElfDynamicAttributePointer
}
ElfDynamicAttribute;

typedef enum
{
	ElfRelocationAttributeNone = 0,
	ElfRelocationAttributeOffset,
	ElfRelocationAttributeInfo
}
ElfRelocationAttribute;

typedef enum
{
	ElfAddendRelocationAttributeNone = 0,
	ElfAddendRelocationAttributeOffset,
	ElfAddendRelocationAttributeInfo,
	ElfAddendRelocationAttributeAddend
}
ElfAddendRelocationAttribute;

typedef enum
{
	ElfSymbolAttributeNone = 0,
	ElfSymbolAttributeName,
	ElfSymbolAttributeInfo,
	ElfSymbolAttributeUnused,
	ElfSymbolAttributeSectionIndex,
	ElfSymbolAttributeValue,
	ElfSymbolAttributeSize,
}
ElfSymbolAttribute;

/* --- inlined getters FIXME: Hint inline */

ElfHeader *elfHeader(Elf *elf);

uint8_t elfIsElf(Elf *elf);
uint8_t elfClass(Elf *elf);
uint8_t elfEncoding(Elf *elf);
uint8_t elfVersion(Elf *elf);
uint8_t elfABI(Elf *elf);
uint64_t elfEntry(Elf *elf);

uint64_t elfLargestAlignment(Elf *elf);
uint64_t elfMemorySize(Elf *elf);

uint64_t elfSectionAttribute(ElfSection *elfSection, ElfSectionAttribute attribute);
ElfSection *elfSections(Elf *elf, uint16_t *size, uint16_t *length);
ElfSection *elfSection(Elf *elf, uint16_t *index, ElfSectionAttribute attribute, uint64_t value);

uint64_t elfSegmentAttribute(ElfSegment *elfSegment, ElfSegmentAttribute attribute);
ElfSegment *elfSegments(Elf *elf, uint16_t *size, uint16_t *length);
ElfSegment *elfSegment(Elf *elf, uint16_t *index, ElfSegmentAttribute attribute, uint64_t value);

/* dynamic (if exists) */

uint64_t elfDynamicAttribute(ElfDynamic *elfDynamic, ElfDynamicAttribute attribute);
ElfDynamic *elfDynamics(Elf *elf, uint16_t *size, uint16_t *length);
ElfDynamic *elfDynamic(Elf *elf, uint16_t *index, ElfDynamicAttribute attribute, uint64_t value);
ElfDynamic *elfLoadedDynamics(Elf *elf, uint16_t *size, uint16_t *length);
ElfDynamic *elfLoadedDynamic(Elf *elf, uint16_t *index, ElfDynamicAttribute attribute, uint64_t value);

/* specifics (if exists) */

char *elfSectionStrings(Elf *elf, uint64_t *size);
char *elfStringFromIndex(char *mem, uint64_t size, uint32_t index);
uint32_t elfStringToIndex(char *mem, uint64_t size, char *str);
char *elfStringFromOffset(char *mem, uint64_t size, uint32_t index);
uint32_t elfStringToOffset(char *mem, uint64_t size, char *str);

ElfSection *elfSectionByName(Elf *elf, char *name);

uint64_t elfAddendRelocationAttribute(ElfAddendRelocation *elfAddendRelocation, ElfAddendRelocationAttribute attribute);
ElfAddendRelocation *elfAddendRelocations(Elf *elf, char *name, uint16_t *size, uint16_t *length);
//ElfAddendRelocation *elfAddendRelocation(Elf *elf, char *name, uint16_t *index, ElfAddendRelocationAttribute attribute, uint64_t value);

uint32_t elfRelocationSymbol(uint64_t info);
uint32_t elfRelocationType(uint64_t info);

/*relocs here*/

uint64_t elfSymbolAttribute(ElfSymbol *elfSymbol, ElfSymbolAttribute attribute);
ElfSymbol *elfSymbols(Elf *elf, char *name, uint16_t *size, uint16_t *length);
//ElfSymbol *elfSymbol(Elf *elf, char *name, uint16_t *index, ElfSymbolAttribute attribute, uint64_t value);

uint8_t elfSymbolType(uint8_t info);
uint8_t elfSymbolBind(uint8_t info);

/* --- actions */

Elf *elfCreate(void *data, uint64_t size);
Elf *elfCreateLocal(void *elfl, void *data, size_t size);
Elf *elfCreateLocalUnchecked(void *elfl, void *data, size_t size);
void *elfDestroy(Elf *elf);
void elfDestroyAndFree(Elf *elf);

/* --- loader --- */

int elfLoaderIsLoadable(Elf *elf);
int elfLoaderInstantiate(Elf *elf, void *memory);
int elfLoaderRelocate(Elf *elf, void *writable, void *executable);
int elfLoaderLoad(Elf *elf, void *writable, void *executable);

#endif
