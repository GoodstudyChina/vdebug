#ifndef DBGBASE_VDEBUG_H_H_
#define DBGBASE_VDEBUG_H_H_
#include <Windows.h>
#include <DbgHelp.h>
#include <map>
#include <list>
#include "TitanEngine/TitanEngine.h"
#include <ComStatic/ComStatic.h>

using namespace std;

typedef DWORD (__stdcall *pfnReadMemoryProc)(IN DWORD64 dwAddr, IN DWORD dwReadLength, OUT char *pBuffer);
typedef DWORD (__stdcall *pfnWriteMemoryProc)(IN DWORD64 dwAddr, IN DWORD dwWriteLength, IN const char *pBuffer);

enum DebuggerStatus
{
    em_dbg_status_init,
    em_dbg_status_busy,
    em_dbg_status_free
};

struct DbgProcModuleProcInfo
{
    ustring m_wstrName;     //����
    DWORD64 m_dwBaseAddr;   //ģ���ַ
    DWORD64 m_dwProcAddr;   //��������
    DWORD64 m_dwType;       //����

    DbgProcModuleProcInfo()
    {
        m_dwBaseAddr = 0;
        m_dwProcAddr = 0;
        m_dwType = 0;
    }
};

struct DbgModuleInfo
{
    ustring m_wstrDllPath;      //dll·��
    ustring m_wstrDllName;      //dll����
    DWORD64 m_dwBaseOfImage;    //dll��ַ
    DWORD64 m_dwEndAddr;        //ģ�������ַ
    DWORD64 m_dwModuleSize;     //ģ���С
    HMODULE m_hModule;          //ģ����
    map<DWORD64, DbgProcModuleProcInfo> m_vProcInfo;   //������Ϣ

    DbgModuleInfo()
    {
        m_dwBaseOfImage = 0;
        m_dwEndAddr = 0;
        m_dwModuleSize = 0;
        m_hModule = NULL;
    }
};

class CDbgBase
{
public:
    CDbgBase() : m_bX64(FALSE)
    {}

    virtual ~CDbgBase()
    {}

    virtual BOOL Connect(LPCWSTR wszTarget, LPVOID pParam)
    {
        return FALSE;
    }

    virtual BOOL Connect(DWORD dwPid)
    {
        return FALSE;
    }

    virtual BOOL DisConnect()
    {
        return FALSE;
    }

    virtual BOOL IsConnect()
    {
        return m_bConnected;
    }

    DebuggerStatus GetStatus()
    {
        return m_eStatus;
    }

    void SetStatus(DebuggerStatus eStatus)
    {
        m_eStatus = eStatus;
    }

    virtual TITAN_ENGINE_CONTEXT_t GetCurrentContext()
    {
        return TITAN_ENGINE_CONTEXT_t();
    }

    virtual ustring GetSymFromAddr(DWORD64 dwAddr)
    {
        return L"";
    }

    virtual list<STACKFRAME64> GetStackFrame(const ustring &wstrParam)
    {
        return list<STACKFRAME64>();
    }

    BOOL IsDbgProcx64()
    {
        return m_bX64;
    }

    //�Ƿ��ǼĴ���
    bool IsRegister(const ustring &wstr) const;
    mstring GetPrintStr(const char *szBuffer, int iSize)
    {
        mstring strOut;
        for (int i = 0 ; i < iSize ;)
        {
            byte letter = szBuffer[i];
            //�ַ�
            if (letter >= 0x20 && letter <= 0x7e)
            {
                strOut += (char)letter;
                i++;
                continue;
            }
            //����
            else if (letter >= 0xb0 && letter <= 0xf7)
            {
                if (i < iSize)
                {
                    byte next = szBuffer[i + 1];
                    if (next >= 0xa1 && next <= 0xfe)
                    {
                        strOut += (char)letter;
                        strOut += (char)next;
                        i += 2;
                        continue;
                    }
                }
            }
            //���ɴ�ӡ
            strOut += '.';
            i++;
        }
        return strOut;
    }
protected:
    BOOL ReadMemory(DWORD64 dwAddr, IN OUT DWORD dwLength, LPSTR pBuffer);
    BOOL WriteMemory(DWORD64 dwAddr, LPCSTR pBuffer, DWORD dwLength);

protected:
    virtual INT_PTR OnDebugEvent(LPDEBUG_EVENT pDbgEvent)
    {
        return 0;
    }

protected:
    BOOL m_bX64;
    BOOL m_bConnected;
    DebuggerStatus m_eStatus;
};
#endif