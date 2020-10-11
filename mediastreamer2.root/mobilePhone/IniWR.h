#pragma once

#define DIM(a)		sizeof(a)/sizeof(a[0])

class CIniWR
{
public:
	CIniWR(void);
	~CIniWR(void);
	int ReadIniFile(LPCWSTR lpFileName);
	DWORD GetLine(LPWSTR pLine, DWORD dwOffset, DWORD dwSize);
	bool IsComment(LPCWSTR pLine);
	bool IsSection(LPCWSTR pLine);
	bool IsSectionName(LPCWSTR pLine, LPCWSTR pSection);
	bool IsKey(LPCWSTR pLine, LPCWSTR pKeyName, LPWSTR* pValue , DWORD* dwValLen);
	void WriteValue(HANDLE m_hOutput, LPCWSTR pAppName, LPCWSTR pKeyName, LPCWSTR pString);
	DWORD GetString(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPWSTR lpReturnedString, DWORD nSize, LPCWSTR lpFileName);
	DWORD GetPrivateProfileString(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPWSTR lpReturnedString, DWORD Size, LPCWSTR lpFileName);
	UINT GetPrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int nDefault, LPCTSTR lpFileName);
	bool WritePrivateProfileString(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString, LPCTSTR lpFileName);
	void WriteLine(HANDLE hOutput, LPCWSTR pLine);
	bool WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int Value, LPCTSTR lpFileName);
};
