#include "StdAfx.h"
#include "LogWrite.h"

CLogWrite::CLogWrite(void)
{
  InitializeCriticalSection(&m_cs);
  
  CString cstrFileName;
  SYSTEMTIME sysTime;
  ::GetSystemTime(&sysTime);
  cstrFileName.Format(_T("Log%02d%02d%04d.txt"), sysTime.wDay, sysTime.wMonth, sysTime.wYear);

  m_pLogFile = new ofstream((LPCTSTR)cstrFileName, ios::out|ios::app);
  *m_pLogFile<<"******************** Application started ********************"<<endl;
}

CLogWrite::~CLogWrite(void)
{
  *m_pLogFile<<"******************** Application terminated ********************"<<endl;

  DeleteCriticalSection(&m_cs);
  
  if(m_pLogFile != NULL)
  {
    delete m_pLogFile;
    m_pLogFile = NULL;
  }
}

bool CLogWrite::CheckIn()
{
	return TryEnterCriticalSection(&m_cs) > 0 ? true : false;
}

void CLogWrite::CheckOut()
{
	return LeaveCriticalSection(&m_cs);
}

void CLogWrite::LogEvent(CString cstrEvent, int iErrCodes)
{
  if(CheckIn() == false)
    return;

  if(m_pLogFile != NULL)
  {
    CString cstrTime;
    SYSTEMTIME sysTime;
    ::GetSystemTime(&sysTime);
    cstrTime.Format(_T("%02d:%02d:%02d"), sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
    
    CString csErrorCode;
    if(iErrCodes != -999)
    {
      csErrorCode.Format(_T("%d"), iErrCodes);
      cstrEvent = cstrTime + _T(" ") + cstrEvent + csErrorCode;
    }
    else
    {
      cstrEvent = cstrTime + _T(" ") + cstrEvent;
    }

    int iSiz = cstrEvent.GetLength();
    char *pBuffer = new char[iSiz+1];

    for(int i=0; i<=iSiz; i++)
    {
      pBuffer[i] = (char)cstrEvent.GetAt(i);
    }

    //pBuffer[iSiz] = '\n';

    *m_pLogFile<<pBuffer<<endl;
    m_pLogFile->flush();

    delete []pBuffer;
  }

  CheckOut();
}