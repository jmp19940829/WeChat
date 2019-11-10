// WeChatDll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "WeChatDll.h"
#ifdef _X86_
extern "C" { int _afxForceUSRDLL; }
#else
extern "C" { int __afxForceUSRDLL; }
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
CWinApp theApp;

using namespace std;
CWeChatDll g_WeChatDll;

BOOL Mymain()
{
	int nRetCode = 0;
	AfxMessageBox("注入");
	HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr)
	{
		// 初始化 MFC 并在失败时显示错误
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			// TODO: 更改错误代码以符合您的需要
			wprintf(L"错误: MFC 初始化失败\n");
			nRetCode = 1;
		}
		else
		{
			

			//0BCCEA10  0A6EADB8  UNICODE "wxid_udf87w5xrr5622"
			//0BDE3598  0BBC7EA0  UNICODE "wxid_o6qtpsjzi32922"
			//0A8CC578  0B75F388  UNICODE "wxid_qcf7tarw5i8y22"




			g_WeChatDll.InitData();
			//AfxMessageBox("开始进群");
			//g_WeChatDll.SendUrlAddRoom(L"https://support.weixin.qq.com/cgi-bin/mmsupport-bin/addchatroombyinvite?ticket=A6fff1ao1vmoNr6BrkjC0",L"wxid_o6qtpsjzi32922");
			//CString aa = "wxid_66hotg8nv37m22";
			//g_WeChatDll.sendCollections(aa,0);
			//g_WeChatDll.GetXiangxiBase(L"liuhaiqi000");
			//0AA93148  0BA44758  UNICODE "wxid_oeufy22czhxr12"
			//g_WeChatDll.SetBeiZhu(L"wxid_benwytlkyosx21", L"AA666备注成功喽");

			/*for (int k=0;k<5;k++)
			{
				AfxMessageBox("开始获取");
				QUNCHENGYUANLIST& listQunChengyuan = g_WeChatDll.GetAddRoomXinxi(L"105489409@chatroom");
				int nNum = listQunChengyuan.GetCount();
				CStringW strnNum;
				strnNum.Format(L"群成员人数：%d", nNum);
				OutputDebugStringW(strnNum);
				for (int i = 0; i < nNum; i++)
				{
				
					XINXIJIEGOU& Xinxi = listQunChengyuan.GetAt(listQunChengyuan.FindIndex(i));

					OutputDebugStringW(Xinxi.szName);
					OutputDebugStringW(Xinxi.szWxHao);
					OutputDebugStringW(Xinxi.szV1);
					OutputDebugStringW(Xinxi.szWxid);

				}
			}*/
			/*ARRWXID &listRet = g_WeChatDll.GetRoomWxid(L"105489409@chatroom");
			if (listRet.IsEmpty())
			{
				return 0;
			}
			CString str;
			str.Format("当前qun成员数量：%d", listRet.GetSize());
			AfxMessageBox(str);
			for (int i = 0; i < listRet.GetSize(); i++)
			{

				LPWSTR Myhao = listRet.GetAt(i);

				OutputDebugStringW(Myhao);
				//OutputDebugStringW(Myhao.szQId);
			}*/

		}
	}
	else
	{
		// TODO: 更改错误代码以符合您的需要
		wprintf(L"错误: GetModuleHandle 失败\n");
		nRetCode = 1;
	}

	return nRetCode;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
#pragma comment(linker, "/EXPORT:vSetDdrawflag=_AheadLib_vSetDdrawflag,@1")
#pragma comment(linker, "/EXPORT:AlphaBlend=_AheadLib_AlphaBlend,@2")
#pragma comment(linker, "/EXPORT:DllInitialize=_AheadLib_DllInitialize,@3")
#pragma comment(linker, "/EXPORT:GradientFill=_AheadLib_GradientFill,@4")
#pragma comment(linker, "/EXPORT:TransparentBlt=_AheadLib_TransparentBlt,@5")
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 宏定义
#define EXTERNC extern "C"
#define NAKED __declspec(naked)
#define EXPORT __declspec(dllexport)

#define ALCPP EXPORT NAKED
#define ALSTD EXTERNC EXPORT NAKED void __stdcall
#define ALCFAST EXTERNC EXPORT NAKED void __fastcall
#define ALCDECL EXTERNC NAKED void __cdecl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AheadLib 命名空间
namespace AheadLib
{
	HMODULE m_hModule = NULL;	// 原始模块句柄
	DWORD m_dwReturn[5] = { 0 };	// 原始函数返回地址


									// 加载原始模块
	inline BOOL WINAPI Load()
	{
		TCHAR tzPath[MAX_PATH];
		TCHAR tzTemp[MAX_PATH * 2];

		GetSystemDirectory(tzPath, MAX_PATH);
		lstrcat(tzPath, TEXT("\\msimg32"));
		m_hModule = LoadLibrary(tzPath);
		if (m_hModule == NULL)
		{
			wsprintf(tzTemp, TEXT("无法加载 %s，程序无法正常运行。"), tzPath);
			MessageBox(NULL, tzTemp, TEXT("AheadLib"), MB_ICONSTOP);
		}

		return (m_hModule != NULL);
	}

	// 释放原始模块
	inline VOID WINAPI Free()
	{
		if (m_hModule)
		{
			FreeLibrary(m_hModule);
		}
	}

	// 获取原始函数地址
	FARPROC WINAPI GetAddress(PCSTR pszProcName)
	{
		FARPROC fpAddress;
		CHAR szProcName[16];
		TCHAR tzTemp[MAX_PATH];

		fpAddress = GetProcAddress(m_hModule, pszProcName);
		if (fpAddress == NULL)
		{
			if (HIWORD(pszProcName) == 0)
			{
				wsprintf(szProcName, "%d", pszProcName);
				pszProcName = szProcName;
			}

			wsprintf(tzTemp, TEXT("无法找到函数 %hs，程序无法正常运行。"), pszProcName);
			MessageBox(NULL, tzTemp, TEXT("AheadLib"), MB_ICONSTOP);
			ExitProcess(-2);
		}

		return fpAddress;
	}
}
using namespace AheadLib;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 入口函数
BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, PVOID pvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		Mymain();
		return Load();
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		Free();
	}

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_vSetDdrawflag(void)
{
	GetAddress("vSetDdrawflag");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_AlphaBlend(void)
{
	GetAddress("AlphaBlend");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_DllInitialize(void)
{
	GetAddress("DllInitialize");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_GradientFill(void)
{
	GetAddress("GradientFill");
	__asm JMP EAX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_TransparentBlt(void)
{
	GetAddress("TransparentBlt");
	__asm JMP EAX;
}


