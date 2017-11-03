/*
CAPE - Config And Payload Extraction
Copyright(C) 2015-2017 Context Information Security. (kevin.oreilly@contextis.com)

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
*/
//  STATUS_SUCCESS
#define STATUS_SUCCESS                  ((NTSTATUS)0x00000000L)

// STATUS_BAD_COMPRESSION_BUFFER    "The specified buffer contains ill-formed data."
#define STATUS_BAD_COMPRESSION_BUFFER   ((NTSTATUS)0xC0000242L)

#define	PE_HEADER_LIMIT		0x200	// Range to look for PE header within candidate buffer

#define SIZE_OF_LARGEST_IMAGE ((ULONG)0x77000000)

#pragma comment(lib, "Wininet.lib")

#define	DATA				0
#define	EXECUTABLE			1
#define	DLL			        2

#define	PE_HEADER_LIMIT		0x200	// Range to look for PE header within candidate buffer

typedef struct CapeMetadata 
{
	char*	ProcessPath;
	char*	ModulePath;
    DWORD   Pid;
    DWORD   DumpType;
    char*	TargetProcess;  // For injection
    DWORD	TargetPid;      // "
    PVOID   Address;        // For shellcode/modules
	SIZE_T  Size;           // "
} CAPEMETADATA, *PCAPEMETADATA;

typedef struct InjectionSectionView
{
    HANDLE                          SectionHandle;
    PVOID                           LocalView;
    SIZE_T                          ViewSize;
	int                             TargetProcessId;
    struct InjectionSectionView     *NextSectionView;
} INJECTIONSECTIONVIEW, *PINJECTIONSECTIONVIEW;

typedef struct InjectionInfo
{
    int                         ProcessId;
	HANDLE	                    ProcessHandle;
    DWORD_PTR                   ImageBase;
    DWORD_PTR                   EntryPoint;
    BOOL                        WriteDetected;
    BOOL                        ImageDumped;
    LPVOID                     BufferBase;
    unsigned int                BufferSizeOfImage;
    HANDLE                      SectionHandle;
//    struct InjectionSectionView *SectionViewList;
    struct InjectionInfo        *NextInjectionInfo;
} INJECTIONINFO, *PINJECTIONINFO;

// To allow access to a subset of current hook info
// (without including hooking.h)
#ifndef __HOOKING_H
typedef struct _hook_info_t {
	int disable_count;
	ULONG_PTR last_hook;    // These were hook_t* but we
	ULONG_PTR current_hook; // don't need to include this.
	ULONG_PTR return_address;
	ULONG_PTR stack_pointer;
	ULONG_PTR frame_pointer;
	ULONG_PTR main_caller_retaddr;
	ULONG_PTR parent_caller_retaddr;
} hook_info_t;
#endif

PINJECTIONSECTIONVIEW AddSectionView(HANDLE SectionHandle, PVOID LocalView, SIZE_T ViewSize);
PINJECTIONSECTIONVIEW GetSectionView(HANDLE SectionHandle);
BOOL DropSectionView(PINJECTIONSECTIONVIEW SectionView);

struct InjectionInfo *InjectionInfoList;

PINJECTIONINFO GetInjectionInfo(DWORD ProcessId);
PINJECTIONINFO CreateInjectionInfo(DWORD ProcessId);

struct InjectionSectionView *SectionViewList;


extern HMODULE s_hInst;
extern WCHAR s_wzDllPath[MAX_PATH];
extern CHAR s_szDllPath[MAX_PATH];

//Global debugger switch
#define DEBUGGER_ENABLED 1

PVOID GetPageAddress(PVOID Address);
BOOL TranslatePathFromDeviceToLetter(__in TCHAR *DeviceFilePath, __out TCHAR* DriveLetterFilePath, __inout LPDWORD lpdwBufferSize);
BOOL DumpPEsInRange(LPVOID Buffer, SIZE_T Size);
int DumpMemory(LPVOID Buffer, unsigned int Size);
int DumpCurrentProcessNewEP(LPVOID NewEP);
int DumpCurrentProcess();
int DumpProcess(HANDLE hProcess, LPVOID ImageBase);
int DumpPE(LPVOID Buffer);
int ScanForNonZero(LPVOID Buffer, unsigned int Size);
int ScanPageForNonZero(LPVOID Address);
int ScanForPE(LPVOID Buffer, unsigned int Size, LPVOID* Offset);
int ScanForDisguisedPE(LPVOID Buffer, unsigned int Size, LPVOID* Offset);
int IsDisguisedPEHeader(LPVOID Buffer);
int DumpImageInCurrentProcess(LPVOID ImageBase);
void DumpSectionViewsForPid(DWORD Pid);

unsigned int DumpSize;
SYSTEM_INFO SystemInfo;



struct CapeMetadata *CapeMetaData;

BOOL SetCapeMetaData(DWORD DumpType, DWORD TargetPid, HANDLE hTargetProcess, PVOID Address);

enum {
    PROCDUMP                = 0,
    
    COMPRESSION             = 1,
    
    INJECTION_PE            = 3,
    INJECTION_SHELLCODE     = 4,
    //INJECTION_RUNPE         = 5,

    EXTRACTION_PE           = 8,
    EXTRACTION_SHELLCODE    = 9,
    
    PLUGX_PAYLOAD           = 0x10,
    PLUGX_CONFIG            = 0x11,
    
    EVILGRAB_PAYLOAD        = 0x14,
    EVILGRAB_DATA           = 0x15,
    
    SEDRECO_DATA            = 0x20,
	
    CERBER_CONFIG           = 0x30,
    CERBER_PAYLOAD          = 0x31
};

HANDLE EvilGrabRegHandle;
