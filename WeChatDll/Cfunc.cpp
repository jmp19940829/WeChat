#include "WeBianLiang.h"
#include "stdafx.h"
#include "WeChatDll.h"

//CList<>
DWORD g_JmpRecvMsg;
DWORD g_JmpErWeiMaPng;
DWORD g_dwJmpKey8;//获取到进群的完整链接后JMP回原来地址
BOOL UTF8_TO_GBK(CString &strGBK, LPCSTR lpstring)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, lpstring, -1, NULL, 0);
	wchar_t *wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, (len + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, lpstring, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	strGBK = wszGBK;
	delete[]szGBK;
	delete[]wszGBK;
	return TRUE;
}
//获取微信消息的裸函数
__declspec(naked)void HookRecvMsgWx()
{
	__asm
	{
		pushad
		pushfd
		mov eax, [esp + 0x28]
		mov ecx, [eax]
		add ecx, 0x114
		push[ecx]
		mov eax, [esp + 0x2C]
		mov ecx, [eax]
		add ecx, 0x68
		push[ecx]
		mov eax, [esp + 0x30]
		mov ecx, [eax]
		add ecx, 0x40
		push[ecx]
		call  CWeChatDll::RecvWxMsg
		add esp, 0xC
		popfd
		popad
		push ebp
		mov ebp, esp
		push  0x1
		jmp g_JmpRecvMsg
	}
}
//获取微信二维码的裸函数
__declspec(naked)void HookEeWeiMa()
{
	__asm
	{
		pushad
		push [esp+0x28]
		push [esp + 0x28]
		call CWeChatDll::GetErWeiMa
		add esp,0x8
		popad
		push ebp
		mov ebp, esp
		cmp dword ptr ss : [ebp + 0x8], 0x0
		jmp g_JmpErWeiMaPng
	}
}
//获取KEY8
__declspec(naked)void  GetKey8url()
{
	__asm
	{
		pushad
		push [eax]
		call CWeChatDll::PostAddRoom
		add esp,0x4
		popad
		lea eax, dword ptr ss : [ebp - 0x24]
		push eax
		lea ecx, dword ptr ds : [esi + 0x24]
		jmp g_dwJmpKey8
	}
}
const CStringW GetstrXml(const CStringW& strZ, const CStringW& strY, CStringW& strXML)
{
	int len1 = strXML.Find(strZ);
	if (len1 != -1)
	{
		len1 += strZ.GetLength();
		int len2 = strXML.Find(strY, len1);
		if (len2 != -1)
		{
			return strXML.Mid(len1, len2 - len1);
		}

	}
	return L"提取数据失败";
}
CString UTF8ToGB(const char* str)
{
	CString result;
	WCHAR *strSrc;
	LPSTR szRes;

	//获取临时变量的大小
	int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	strSrc = new WCHAR[i + 1];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);

	//获取临时变量的大小
	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new CHAR[i + 1];
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);
	result = szRes;
	DWORD nLen;
	delete[]strSrc;
	delete[]szRes;
	return result;
}
CStringW strArr[520];
CStringW* StrParTition(LPWSTR strTxt, LPWSTR strByte, int& nNum)
{

	int nVAL = 0;
	LPWSTR ptr = wcstok(strTxt, strByte);
	while (ptr != NULL) 
	{
		strArr[nVAL] = ptr;
		ptr = wcstok(NULL, strByte);
		nVAL++;
	}
	nNum = nVAL;
	return strArr;
}
int MyOutputDebugString(LPCTSTR pstrFormat, ...)
{
	TCHAR szBuffer[1024] = { 0 };
	va_list argList;
	va_start(argList, pstrFormat);
	int iRet = ::_vstprintf_s(szBuffer, pstrFormat, argList);
	va_end(argList);
	OutputDebugString(szBuffer);
	return iRet;
}

#define BURSIZE 2048
int hex2dec(char c)
{
	if ('0' <= c && c <= '9')
	{
		return c - '0';
	}
	else if ('a' <= c && c <= 'f')
	{
		return c - 'a' + 10;
	}
	else if ('A' <= c && c <= 'F')
	{
		return c - 'A' + 10;
	}
	else
	{
		return -1;
	}
}
void urldecode(char url[])
{
	int i = 0;
	int len = strlen(url);
	int res_len = 0;
	char res[BURSIZE];
	for (i = 0; i < len; ++i)
	{
		char c = url[i];
		if (c != '%')
		{
			res[res_len++] = c;
		}
		else
		{
			char c1 = url[++i];
			char c0 = url[++i];
			int num = 0;
			num = hex2dec(c1) * 16 + hex2dec(c0);
			res[res_len++] = num;
		}
	}
	res[res_len] = '\0';
	strcpy(url, res);
}