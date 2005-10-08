/*
 * exchndl.c
 *
 * Author:
 *   José Fonseca <j_r_fonseca@yahoo.co.uk>
 *
 * Originally based on Matt Pietrek's MSJEXHND.CPP in Microsoft Systems
 * Journal, April 1997.
 */

#include <assert.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#define bfd_get_section_size_before_reloc bfd_get_section_size
#endif

#define HAVE_BFD    1

static TCHAR szBuff[16384];

static int __cdecl rprintf(const TCHAR * format, ...)
{
    va_list argptr;

    va_start(argptr, format);
    wvsprintf(szBuff + strlen(szBuff), format, argptr);
    va_end(argptr);

    return 0;
}

// The GetModuleBase function retrieves the base address of the module that contains the specified address. 
static 
DWORD GetModuleBase(DWORD dwAddress)
{
    MEMORY_BASIC_INFORMATION Buffer;
    
    return VirtualQuery((LPCVOID) dwAddress, &Buffer, sizeof(Buffer)) ? (DWORD) Buffer.AllocationBase : 0;
}


#ifdef HAVE_BFD

#include <bfd.h>
#include "internal.h"
#include "libcoff.h"

// Read in the symbol table.
static bfd_boolean
slurp_symtab (bfd *abfd, asymbol ***syms, long *symcount)
{
    long storage;

    if ((bfd_get_file_flags (abfd) & HAS_SYMS) == 0)
        return FALSE;

    storage = bfd_get_symtab_upper_bound (abfd);
    if (storage < 0)
        return FALSE;

    if((*syms = (asymbol **) malloc (storage)) == NULL)
        return FALSE;

    if((*symcount = bfd_canonicalize_symtab (abfd, *syms)) < 0)
        return FALSE;
    
    return TRUE;
}

// This stucture is used to pass information between translate_addresses and find_address_in_section.
struct find_handle
{
    asymbol **syms;
    bfd_vma pc;
    const char *filename;
    const char *functionname;
    unsigned int line;
    long symcount;
    bfd_boolean found;
};

// Look for an address in a section.  This is called via  bfd_map_over_sections. 
static void find_address_in_section (bfd *abfd, asection *section, PTR data)
{
    long i;
    struct find_handle *info = (struct find_handle *) data;
    bfd_vma vma;
    bfd_size_type size;

    if (info->found)
        return;

    if ((bfd_get_section_flags (abfd, section) & SEC_ALLOC) == 0)
        return;

    vma = bfd_get_section_vma (abfd, section);
    size = bfd_get_section_size_before_reloc (section);
    
    if (info->pc < (vma = bfd_get_section_vma (abfd, section)))
        return;

    if (info->pc >= vma + (size = bfd_get_section_size_before_reloc (section)))
        return;

    long nearest_index = -1;

    for (i = 0; i < info->symcount; i++)
    {
        if (section == info->syms[i]->section && info->pc - vma >= info->syms[i]->value)
        {
            if (nearest_index == -1 || info->syms[i]->value > info->syms[nearest_index]->value)
                nearest_index = i;
        }
    }

    info->found = 0;
    if (nearest_index != -1)
    {
        info->line = 1;
        info->found = 1;
        info->functionname = info->syms[nearest_index]->name;
        info->filename = 0;
    }
}

static
BOOL BfdGetSymFromAddr(bfd *abfd, asymbol **syms, long symcount, DWORD dwAddress, LPTSTR lpSymName, DWORD nSize)
{
    HMODULE hModule;
    struct find_handle info;
    
    if(!(hModule = (HMODULE) GetModuleBase(dwAddress)))
        return FALSE;
    
    info.pc = dwAddress;

    if(!(bfd_get_file_flags (abfd) & HAS_SYMS) || !symcount)
        return FALSE;
    info.syms = syms;
    info.symcount = symcount;

    info.found = FALSE;
    bfd_map_over_sections (abfd, find_address_in_section, (PTR) &info);
    if (info.found == FALSE)
        return FALSE;

    assert(lpSymName);
    
    if(info.functionname == NULL)
        return FALSE;       
    
    lstrcpyn(lpSymName, info.functionname, nSize);

    return TRUE;
}

#endif /* HAVE_BFD */

#include <imagehlp.h>

static
BOOL PEGetSymFromAddr(HANDLE hProcess, DWORD dwAddress, LPTSTR lpSymName, DWORD nSize)
{
    HMODULE hModule;
    PIMAGE_NT_HEADERS pNtHdr;
    IMAGE_NT_HEADERS NtHdr;
    PIMAGE_SECTION_HEADER pSection;
    DWORD dwNearestAddress = 0, dwNearestName;
    int i;

    if(!(hModule = (HMODULE) GetModuleBase(dwAddress)))
        return FALSE;
    
    {
        PIMAGE_DOS_HEADER pDosHdr;
        LONG e_lfanew;
        
        // Point to the DOS header in memory
        pDosHdr = (PIMAGE_DOS_HEADER)hModule;
        
        // From the DOS header, find the NT (PE) header
        if(!ReadProcessMemory(hProcess, &pDosHdr->e_lfanew, &e_lfanew, sizeof(e_lfanew), NULL))
            return FALSE;
        
        pNtHdr = (PIMAGE_NT_HEADERS)((DWORD)hModule + (DWORD)e_lfanew);
    
        if(!ReadProcessMemory(hProcess, pNtHdr, &NtHdr, sizeof(IMAGE_NT_HEADERS), NULL))
            return FALSE;
    }
    
    pSection = (PIMAGE_SECTION_HEADER) ((DWORD)pNtHdr + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + NtHdr.FileHeader.SizeOfOptionalHeader);

    // Look for export section
    for (i = 0; i < NtHdr.FileHeader.NumberOfSections; i++, pSection++)
    {
        IMAGE_SECTION_HEADER Section;
        PIMAGE_EXPORT_DIRECTORY pExportDir = NULL;
        BYTE ExportSectionName[IMAGE_SIZEOF_SHORT_NAME] = {'.', 'e', 'd', 'a', 't', 'a', '\0', '\0'};
        
        if(!ReadProcessMemory(hProcess, pSection, &Section, sizeof(IMAGE_SECTION_HEADER), NULL))
            return FALSE;
        
        if(memcmp(Section.Name, ExportSectionName, IMAGE_SIZEOF_SHORT_NAME) == 0)
            pExportDir = (PIMAGE_EXPORT_DIRECTORY) Section.VirtualAddress;
        else if ((NtHdr.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress >= Section.VirtualAddress) && (NtHdr.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress < (Section.VirtualAddress + Section.SizeOfRawData)))
            pExportDir = (PIMAGE_EXPORT_DIRECTORY) NtHdr.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

        if(pExportDir)
        {
            IMAGE_EXPORT_DIRECTORY ExportDir;
            
            if(!ReadProcessMemory(hProcess, (PVOID)((DWORD)hModule + (DWORD)pExportDir), &ExportDir, sizeof(IMAGE_EXPORT_DIRECTORY), NULL))
                return FALSE;
            
            {
                PDWORD *AddressOfFunctions = alloca(ExportDir.NumberOfFunctions*sizeof(PDWORD));
                int j;
    
                if(!ReadProcessMemory(hProcess, (PVOID)((DWORD)hModule + (DWORD)ExportDir.AddressOfFunctions), AddressOfFunctions, ExportDir.NumberOfFunctions*sizeof(PDWORD), NULL))
                        return FALSE;
                
                for(j = 0; j < ExportDir.NumberOfNames; ++j)
                {
                    DWORD pFunction = (DWORD)hModule + (DWORD)AddressOfFunctions[j];
                    
                    if(pFunction <= dwAddress && pFunction > dwNearestAddress)
                    {
                        dwNearestAddress = pFunction;
                        
                        if(!ReadProcessMemory(hProcess, (PVOID)((DWORD)hModule + (DWORD)(ExportDir.AddressOfNames + j * 4)), &dwNearestName, sizeof(dwNearestName), NULL))
                            return FALSE;
                            
                        dwNearestName = (DWORD)hModule + dwNearestName;
                    }
                }
            }
        }       
    }

    if(!dwNearestAddress)
        return FALSE;
        
    if(!ReadProcessMemory(hProcess, (PVOID)dwNearestName, lpSymName, nSize, NULL))
        return FALSE;
    lpSymName[nSize - 1] = 0;

    return TRUE;
}

static
BOOL WINAPI IntelStackWalk(
    DWORD MachineType, 
    HANDLE hProcess, 
    HANDLE hThread, 
    LPSTACKFRAME StackFrame, 
    PCONTEXT ContextRecord, 
    PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine,  
    PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine,
    PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine, 
    PTRANSLATE_ADDRESS_ROUTINE TranslateAddress 
)
{
    assert(MachineType == IMAGE_FILE_MACHINE_I386);
    
    if(ReadMemoryRoutine == NULL)
        ReadMemoryRoutine = ReadProcessMemory;
    
    if(!StackFrame->Reserved[0])
    {
        StackFrame->Reserved[0] = 1;
        
        StackFrame->AddrPC.Mode = AddrModeFlat;
        StackFrame->AddrPC.Offset = ContextRecord->Eip;
        StackFrame->AddrStack.Mode = AddrModeFlat;
        StackFrame->AddrStack.Offset = ContextRecord->Esp;
        StackFrame->AddrFrame.Mode = AddrModeFlat;
        StackFrame->AddrFrame.Offset = ContextRecord->Ebp;

        StackFrame->AddrReturn.Mode = AddrModeFlat;
        if(!ReadMemoryRoutine(hProcess, (LPCVOID) (StackFrame->AddrFrame.Offset + sizeof(DWORD)), &StackFrame->AddrReturn.Offset, sizeof(DWORD), NULL))
            return FALSE;
    }
    else
    {
        StackFrame->AddrPC.Offset = StackFrame->AddrReturn.Offset;
        //AddrStack = AddrFrame + 2*sizeof(DWORD);
        if(!ReadMemoryRoutine(hProcess, (LPCVOID) StackFrame->AddrFrame.Offset, &StackFrame->AddrFrame.Offset, sizeof(DWORD), NULL))
            return FALSE;
        if(!ReadMemoryRoutine(hProcess, (LPCVOID) (StackFrame->AddrFrame.Offset + sizeof(DWORD)), &StackFrame->AddrReturn.Offset, sizeof(DWORD), NULL))
            return FALSE;
    }

    ReadMemoryRoutine(hProcess, (LPCVOID) (StackFrame->AddrFrame.Offset + 2*sizeof(DWORD)), StackFrame->Params, sizeof(StackFrame->Params), NULL);
    
    return TRUE;    
}

static
BOOL StackBackTrace(HANDLE hProcess, HANDLE hThread, PCONTEXT pContext)
{
    STACKFRAME StackFrame;

    HMODULE hModule = NULL;
    TCHAR szModule[MAX_PATH]; 

#ifdef HAVE_BFD
    bfd *abfd = NULL;
    asymbol **syms = NULL;  // The symbol table.
    long symcount = 0;  // Number of symbols in `syms'.
#endif /* HAVE_BFD */

    memset( &StackFrame, 0, sizeof(StackFrame) );

    // Initialize the STACKFRAME structure for the first call.  This is only
    // necessary for Intel CPUs, and isn't mentioned in the documentation.
    StackFrame.AddrPC.Offset = pContext->Eip;
    StackFrame.AddrPC.Mode = AddrModeFlat;
    StackFrame.AddrStack.Offset = pContext->Esp;
    StackFrame.AddrStack.Mode = AddrModeFlat;
    StackFrame.AddrFrame.Offset = pContext->Ebp;
    StackFrame.AddrFrame.Mode = AddrModeFlat;

    rprintf( _T("Call stack:<br>") );

    while ( 1 )
    {
        BOOL bSuccess = FALSE;
        HMODULE hPrevModule = hModule;
        TCHAR szSymName[512] = _T("");
//      TCHAR szFileName[MAX_PATH] = _T("");
//      DWORD LineNumber = 0;

        if(!IntelStackWalk(
                    IMAGE_FILE_MACHINE_I386,
                    hProcess,
                    hThread,
                    &StackFrame,
                    pContext,
                    NULL,
                    NULL,
                    NULL,
                    NULL
                )
            ) break;
        
        // Basic sanity check to make sure  the frame is OK.  Bail if not.
        if ( 0 == StackFrame.AddrFrame.Offset ) 
            break;
        
        if((hModule = (HMODULE) GetModuleBase(StackFrame.AddrPC.Offset)) && GetModuleFileName(hModule, szModule, sizeof(szModule)))
        {
            rprintf(_T("%s:%08lX"), szModule, StackFrame.AddrPC.Offset);
        
            if(hModule != hPrevModule)
            {
                if(syms)
                {
                    free(syms);
                    syms = NULL;
                    symcount = 0;
                }
                
                if(abfd)
                    bfd_close(abfd);
                
                if((abfd = bfd_openr (szModule, NULL)))
                    if(bfd_check_format(abfd, bfd_object))
                    {
                        bfd_vma adjust_section_vma = 0;
            
                        /* If we are adjusting section VMA's, change them all now.  Changing
                        the BFD information is a hack.  However, we must do it, or
                        bfd_find_nearest_line will not do the right thing.  */
                        if ((adjust_section_vma = (bfd_vma) hModule - pe_data(abfd)->pe_opthdr.ImageBase))
                        {
                            asection *s;
                        
                            for (s = abfd->sections; s != NULL; s = s->next)
                            {
                                s->vma += adjust_section_vma;
                                s->lma += adjust_section_vma;
                            }
                        }
                        
                        if(bfd_get_file_flags(abfd) & HAS_SYMS)
                        {
                            /* Read in the symbol table.  */
                            slurp_symtab(abfd, &syms, &symcount);
//                          printf("symbol table %d %d\n\n", syms, symcount);
                        }
                    }
            }
            
            if(!bSuccess && abfd && syms && symcount)
                if((bSuccess = BfdGetSymFromAddr(abfd, syms, symcount, StackFrame.AddrPC.Offset, szSymName, 512)))
                {
                    rprintf( _T(" %s"), szSymName);
                }
            
            if(!bSuccess)
                if((bSuccess = PEGetSymFromAddr(hProcess, StackFrame.AddrPC.Offset, szSymName, 512)))
                    rprintf( _T(" %s"), szSymName);
        }

        rprintf(_T("<br>"));
    }

#ifdef HAVE_BFD
    if(syms)
    {
        free(syms);
        syms = NULL;
        symcount = 0;
    }
    
    if(abfd)
        bfd_close(abfd);
#endif /* HAVE_BFD */
    
    return TRUE;
}

char *GenerateExceptionReport(PEXCEPTION_POINTERS pExceptionInfo)
{
    PEXCEPTION_RECORD pExceptionRecord = pExceptionInfo->ExceptionRecord;
    TCHAR szModule[MAX_PATH];
    HMODULE hModule;
    PCONTEXT pContext;

    *szBuff = '\0';
    
    switch(pExceptionRecord->ExceptionCode)
    {
        case EXCEPTION_ACCESS_VIOLATION:
            rprintf(_T("Access Violation"));
            break;
    
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
            rprintf(_T("Array Bound Exceeded"));
            break;
    
        case EXCEPTION_BREAKPOINT:
            rprintf(_T("Breakpoint"));
            break;
    
        case EXCEPTION_DATATYPE_MISALIGNMENT:
            rprintf(_T("Datatype Misalignment"));
            break;
    
        case EXCEPTION_FLT_DENORMAL_OPERAND:
            rprintf(_T("Float Denormal Operand"));
            break;
    
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            rprintf(_T("Float Divide By Zero"));
            break;
    
        case EXCEPTION_FLT_INEXACT_RESULT:
            rprintf(_T("Float Inexact Result"));
            break;
    
        case EXCEPTION_FLT_INVALID_OPERATION:
            rprintf(_T("Float Invalid Operation"));
            break;
    
        case EXCEPTION_FLT_OVERFLOW:
            rprintf(_T("Float Overflow"));
            break;
    
        case EXCEPTION_FLT_STACK_CHECK:
            rprintf(_T("Float Stack Check"));
            break;
    
        case EXCEPTION_FLT_UNDERFLOW:
            rprintf(_T("Float Underflow"));
            break;
    
        case EXCEPTION_GUARD_PAGE:
            rprintf(_T("Guard Page"));
            break;

        case EXCEPTION_ILLEGAL_INSTRUCTION:
            rprintf(_T("Illegal Instruction"));
            break;
    
        case EXCEPTION_IN_PAGE_ERROR:
            rprintf(_T("In Page Error"));
            break;
    
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            rprintf(_T("Integer Divide By Zero"));
            break;
    
        case EXCEPTION_INT_OVERFLOW:
            rprintf(_T("Integer Overflow"));
            break;
    
        case EXCEPTION_INVALID_DISPOSITION:
            rprintf(_T("Invalid Disposition"));
            break;
    
        case EXCEPTION_INVALID_HANDLE:
            rprintf(_T("Invalid Handle"));
            break;

        case EXCEPTION_NONCONTINUABLE_EXCEPTION:
            rprintf(_T("Noncontinuable Exception"));
            break;

        case EXCEPTION_PRIV_INSTRUCTION:
            rprintf(_T("Privileged Instruction"));
            break;
    
        case EXCEPTION_SINGLE_STEP:
            rprintf(_T("Single Step"));
            break;
    
        case EXCEPTION_STACK_OVERFLOW:
            rprintf(_T("Stack Overflow"));
            break;

        case DBG_CONTROL_C:
            rprintf(_T("Control+C"));
            break;
    
        case DBG_CONTROL_BREAK:
            rprintf(_T("Control+Break"));
            break;
    
        case DBG_TERMINATE_THREAD:
            rprintf(_T("Terminate Thread"));
            break;
    
        case DBG_TERMINATE_PROCESS:
            rprintf(_T("Terminate Process"));
            break;
    
        case RPC_S_UNKNOWN_IF:
            rprintf(_T("Unknown Interface"));
            break;
    
        case RPC_S_SERVER_UNAVAILABLE:
            rprintf(_T("Server Unavailable"));
            break;
    
        default:
            rprintf(_T("Unknown [0x%lX] Exception"), pExceptionRecord->ExceptionCode);
            break;
    }

    // Now print information about where the fault occured
    rprintf(_T(" at %08x"), (DWORD) pExceptionRecord->ExceptionAddress);
    if((hModule = (HMODULE) GetModuleBase((DWORD) pExceptionRecord->ExceptionAddress)) && GetModuleFileName(hModule, szModule, sizeof(szModule)))
        rprintf(_T(" in %s"), szModule);

    rprintf(_T("<br>"));
    
    // If the exception was an access violation, print out some additional information, to the error log and the debugger.
    if(pExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && pExceptionRecord->NumberParameters >= 2)
        rprintf("%s location %08x", pExceptionRecord->ExceptionInformation[0] ? "Writing to" : "Reading from", pExceptionRecord->ExceptionInformation[1]);

    rprintf(".<br><br>");       
    
    pContext = pExceptionInfo->ContextRecord;

    #ifdef _M_IX86  // Intel Only!

    // Show the registers
    rprintf(_T("Registers:<br>"));
    if(pContext->ContextFlags & CONTEXT_INTEGER)
        rprintf(
            _T("eax=%08lx ebx=%08lx ecx=%08lx edx=%08lx esi=%08lx edi=%08lx<br>"),
            pContext->Eax,
            pContext->Ebx,
            pContext->Ecx,
            pContext->Edx,
            pContext->Esi,
            pContext->Edi
        );
    if(pContext->ContextFlags & CONTEXT_CONTROL)
        rprintf(
            _T("eip=%08lx esp=%08lx ebp=%08lx iopl=%1lx %s %s %s %s %s %s %s %s %s %s<br>"),
            pContext->Eip,
            pContext->Esp,
            pContext->Ebp,
            (pContext->EFlags >> 12) & 3,   //  IOPL level value
            pContext->EFlags & 0x00100000 ? "vip" : "   ",  //  VIP (virtual interrupt pending)
            pContext->EFlags & 0x00080000 ? "vif" : "   ",  //  VIF (virtual interrupt flag)
            pContext->EFlags & 0x00000800 ? "ov" : "nv",    //  VIF (virtual interrupt flag)
            pContext->EFlags & 0x00000400 ? "dn" : "up",    //  OF (overflow flag)
            pContext->EFlags & 0x00000200 ? "ei" : "di",    //  IF (interrupt enable flag)
            pContext->EFlags & 0x00000080 ? "ng" : "pl",    //  SF (sign flag)
            pContext->EFlags & 0x00000040 ? "zr" : "nz",    //  ZF (zero flag)
            pContext->EFlags & 0x00000010 ? "ac" : "na",    //  AF (aux carry flag)
            pContext->EFlags & 0x00000004 ? "po" : "pe",    //  PF (parity flag)
            pContext->EFlags & 0x00000001 ? "cy" : "nc" //  CF (carry flag)
        );
    if(pContext->ContextFlags & CONTEXT_SEGMENTS)
    {
        rprintf(
            _T("cs=%04lx  ss=%04lx  ds=%04lx  es=%04lx  fs=%04lx  gs=%04lx"),
            pContext->SegCs,
            pContext->SegSs,
            pContext->SegDs,
            pContext->SegEs,
            pContext->SegFs,
            pContext->SegGs,
            pContext->EFlags
        );
        if(pContext->ContextFlags & CONTEXT_CONTROL)
            rprintf(
                _T("             efl=%08lx"),
                pContext->EFlags
            );
    }
    else
        if(pContext->ContextFlags & CONTEXT_CONTROL)
            rprintf(
                _T("                                                                       efl=%08lx"),
                pContext->EFlags
            );
    rprintf(_T("<br><br>"));

    #endif

    StackBackTrace(GetCurrentProcess(), GetCurrentThread(), pContext);

    return szBuff;
}
