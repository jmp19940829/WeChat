#pragma once
#include "afxinet.h"
#define		IE_AGENT              _T("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)")  

#define		NORMAL_CONNECT        INTERNET_FLAG_KEEP_CONNECTION  
#define		SECURE_CONNECT        NORMAL_CONNECT | INTERNET_FLAG_SECURE  
#define		NORMAL_REQUEST        INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE   
#define		SECURE_REQUEST        NORMAL_REQUEST | INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID  

#define		HTTP_SUCCESS          0		// 操作成功  
#define		HTTP_FAILURE          1		// 操作失败 
#define		HTTP_TIMEOUT          2		// 操作超时
class CHttpClient
{
public:
	CHttpClient(LPCTSTR strAgent = IE_AGENT);
	virtual ~CHttpClient(void);

	int                 HttpGet(LPCTSTR strUrl, LPCTSTR strPostData, CString &strResponse);
	int                 HttpPost(LPCTSTR strUrl, LPCTSTR strPostData, CString &strResponse);
	int                 HttpPut(LPCTSTR strUrl, LPCTSTR strPostData, CString &strResponse);

	CString             UTF8AndUnicode_Convert(CString &strSource, UINT nSourceCodePage, UINT nTargetCodePage);

private:
	int                 ExecuteRequest(int strMethod, LPCTSTR strUrl, LPCTSTR strPostData, CString &strResponse);
	void                Clear();

private:
	CInternetSession    *m_pSession;
	CHttpConnection     *m_pConnection;
	CHttpFile           *m_pFile;
};