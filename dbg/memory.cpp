#include <Windows.h>
#include <ComLib/ComUtil.h>
#include "memory.h"
#include "TitanEngine/TitanEngine.h"

#define PAGE_SIZE 0x1000

CMemoryProc::CMemoryProc(HANDLE hProcess)
{
    m_hProcess = hProcess;
}

CMemoryProc::~CMemoryProc()
{}

bool CMemoryProc::MemoryReadPageSafe(DWORD64 dwAddr, char *szBuffer, DWORD dwBufferSize, IN OUT SIZE_T *pReadSize) const
{
    if (dwBufferSize > (PAGE_SIZE - (dwAddr & (PAGE_SIZE - 1))))
    {
        ErrMessage(L"Read Page Memory Error");
        return false;
    }

    return ::MemoryReadSafe(m_hProcess, (LPVOID)dwAddr, szBuffer, dwBufferSize, pReadSize);
}

bool CMemoryProc::MemoryReadSafe(DWORD64 dwAddr, char *szBuffer, DWORD dwBufferSize, IN OUT DWORD *pReadSize) const
{
    if (!szBuffer || !dwBufferSize)
    {
        return false;
    }

    DWORD dwOffset = 0;
    DWORD dwRequest = dwBufferSize;
    DWORD dwLeftInFirstPage = (PAGE_SIZE - (dwAddr & (PAGE_SIZE - 1)));
    DWORD dwReadSize = min(dwLeftInFirstPage, dwRequest);

    pReadSize[0] = 0;
    while (dwReadSize)
    {
        SIZE_T dw = 0;
        MemoryReadPageSafe(dwAddr + dwOffset, szBuffer + dwOffset, dwReadSize, &dw);

        pReadSize[0] += (DWORD)dw;
        if (dw != dwReadSize)
        {
            break;
        }

        dwOffset += (DWORD)dw;
        dwRequest -= (DWORD)dw;
        dwReadSize = min(PAGE_SIZE, dwRequest);
    }
    return true;
}