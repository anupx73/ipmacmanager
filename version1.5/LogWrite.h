#pragma once
#include <iostream>
#include <fstream>
using namespace std;

class CLogWrite
{
public:
  CLogWrite(void);
  ~CLogWrite(void);

private:
	CRITICAL_SECTION m_cs;
  ofstream *m_pLogFile;

	bool CheckIn();
	void CheckOut();

public:
  void LogEvent(CString cstrEvent, int iErrCodes = -999);
};
