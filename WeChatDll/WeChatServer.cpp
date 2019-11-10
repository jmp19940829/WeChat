#include "stdafx.h"
#include "WeChatDll.h"
extern CWeChatDll g_WeChatDll;
#include "stdafx.h"
#include "WeChatDll.h"  
#include "Chttp.h"


BOOL CWeChatDll::InitData()
{
	if (MH_Initialize() != MH_OK)
		AfxMessageBox("核心库初始化失败 请联系开发人员！");
	m_WeDllBase=(DWORD)LoadLibrary("WeChatWin.dll");
	if (!m_WeDllBase)
	{
		AfxMessageBox("load 重要初始化失败！");
		return FALSE;
	}

	HookGetRegiXinxi();
	HookReWeiMaPng();
	HookGetKey8();
	return TRUE;
}
MYDATANAME& CWeChatDll::GetWeChatGerenXinxi()
{
	char arrWxID[124] = {};
	CString strWxID;
	CString strWeName;
	DWORD dwName=m_WeDllBase + WENAMEOFFSET;
	DWORD dwUrl = m_WeDllBase + WETOUXIANGURLOFFSET;
	DWORD dwWxId = m_WeDllBase + WEWXIDOFFSET;
	DWORD dwWxHao = m_WeDllBase + WECHATHAO;
	DWORD* dWx =(DWORD*)(dwWxId+0x14);
	DWORD* dwNa=(DWORD*)(dwName + 0x14);
	if (*dWx == 0xF)
	{
		LPCSTR szWxID = (LPCSTR)dwWxId;
		sprintf_s(arrWxID, "%s", szWxID);
	}
	else
	{
		LPCSTR szWxID = (LPCSTR)*(DWORD*)dwWxId;
		sprintf_s(arrWxID, "%s", szWxID);
	}
	
	if (*dwNa==0xF)
	{
		UTF8_TO_GBK(strWeName, (LPCSTR)dwName);
	}
	else
	{
		UTF8_TO_GBK(strWeName, (LPCSTR)*(DWORD*)dwName);
	}
	//判断是否是存放着正确的微信号
	if (*(BYTE*)dwWxHao!=0)
	{
		m_MyDataName.strWxHao=(LPSTR)dwWxHao;
	}
	else
	{
		m_MyDataName.strWxHao = arrWxID;
	}
	m_MyDataName.strURL = (LPCSTR)*(DWORD*)dwUrl;
	m_MyDataName.strWxID = arrWxID;
	m_MyDataName.strWxName = strWeName;
#ifdef _DEBUG
	CString strLog;
	strLog.Format("WXID：%s\n微信号：%s\n微信昵称：%s\n微信头像URL：%s\n", m_MyDataName.strWxID, \
		m_MyDataName.strWxHao, m_MyDataName.strWxName, m_MyDataName.strURL);
	OutputDebugString(strLog);
#endif 
	return m_MyDataName;
}
VOID CWeChatDll::SendMsg(LPWSTR pWxid, LPWSTR pwMsg)
{
	DWORD dwCall=m_WeDllBase + WESENDTEXTOFFSET;
	PMYSENDMSG pMySendMsg = new MYSENDMSG{ pwMsg,(int)wcslen(pwMsg),(int)wcslen(pwMsg) * 2 };
	PMYSENDWXID pMySnedWxid = new MYSENDWXID{pWxid,(int)wcslen(pWxid),(int)wcslen(pWxid)*2 };
	BYTE arr[0x880] = {};
	__asm
	{
		mov edx, pMySnedWxid
		push 1
		push 0
		push pMySendMsg
		lea ecx, arr
		call dwCall
		add esp,0xC
	}
	delete pMySendMsg;
	pMySendMsg = NULL;
	delete pMySnedWxid;
	pMySnedWxid = NULL;
}
BOOL CWeChatDll::HookGetRegiXinxi()
{
	DWORD dwRecvBase=m_WeDllBase + WERECVMSGOFFSET;
	g_JmpRecvMsg=dwRecvBase + 0x5;
	MH_CreateHook((LPVOID)dwRecvBase, HookRecvMsgWx, NULL);
	MH_EnableHook((LPVOID)dwRecvBase);
	return TRUE;
}
VOID CWeChatDll::RecvWxMsg(LPWSTR szWxid, LPWSTR szMsg, LPWSTR szID)
{
	
	if (szID)
	{
		CStringW strLog;
		strLog.Format(L"群消息 接受到来自 群ID:%ls\n消息内容为：%ls\n WXID：%ls\n", szWxid, szMsg, szID);
		OutputDebugStringW(strLog);

	}
	else
	{
		CStringW strLog;
		strLog.Format(L"个人消息 接受到来自 wxid:%ls\n消息内容为：%ls\n", szWxid, szMsg);
		OutputDebugStringW(strLog);

		//判断是否自动同意加好友请求
		if (!g_WeChatDll.m_IsAdd)
		{
			CStringW strMsg = szMsg;
			if (strMsg.Find(L"encryptusername=") != -1 && strMsg.Find(L"ticket=") != -1 && strMsg.Find(L"fromusername=") != -1)
			{
				CStringW strV1 = GetstrXml(L"encryptusername=\"", L"\" fromnickname=", strMsg);
				CStringW  strV2 = GetstrXml(L"ticket=\"", L"\" opcode=", strMsg);
				g_WeChatDll.ZiDongYesHaoYou(strV1.GetBuffer(), strV2.GetBuffer());
				strV1.ReleaseBuffer();
				strV2.ReleaseBuffer();
			}
		}
		//判断是否自动收款
		if (!g_WeChatDll.m_IsShouKuan)
		{
			CStringW strMsg = szMsg;
			if (strMsg.Find(L"<transferid><![CDATA[") != -1 && strMsg.Find(L"<title><![CDATA[微信转账]]></title>") != -1)
			{
				CStringW strDingDan = GetstrXml(L"<transferid><![CDATA[", L"]]></transferid>", strMsg);
				//MessageBoxW(0, strDingDan, 0, MB_OK);
				g_WeChatDll.ZiDongShouKuan(szWxid, strDingDan.GetBuffer());
				strDingDan.ReleaseBuffer();
			}
		}
		if (!g_WeChatDll.m_IsAddRoom)
		{
			CStringW strMsg = szMsg;
			if (strMsg.Find(L"<![CDATA[邀请你加入群聊]]></title>")!=-1&& strMsg.Find(L"<url><![CDATA[") != -1)
			{
				CStringW strUrl = GetstrXml(L"<url><![CDATA[", L"]]></url>", strMsg);
				g_WeChatDll.SendUrlAddRoom(strUrl.GetBuffer(), szWxid);
				strUrl.ReleaseBuffer();
			}
		}
	}
}
VOID CWeChatDll::HookReWeiMaPng()
{
	DWORD dwPng = m_WeDllBase + WEERWEIMAPNGOFFSET;
	g_JmpErWeiMaPng = dwPng + 0x7;
	MH_CreateHook((LPVOID)dwPng, HookEeWeiMa, NULL);
	MH_EnableHook((LPVOID)dwPng);
}
VOID CWeChatDll::GetErWeiMa(BYTE* ByBuf, DWORD dwlen)
{
#ifdef DEBUG
	CFile file;
	if (!file.Open("C:\\WeChat.png", CFile::modeReadWrite | CFile::shareDenyNone | CFile::modeCreate))
	{
		AfxMessageBox("二维码写出失败");
		return;
	}
	file.Write(ByBuf, dwlen);
	file.Close();
#endif
}
BOOL CWeChatDll::SendYaoQingQunLiao(LPWSTR szWxid, LPWSTR szQid)
{
	struct MyQunid
	{
		char arr[8];
		LPWSTR Qid;
		int nlen1;
		int nlen2;
	};
	MyQunid My;
	My.Qid = szQid;
	My.nlen1 = (int)wcslen(szQid) * 2;
	My.nlen2 = (int)wcslen(szQid) * 2;
	LPWSTR *asmWxid = &szWxid;
	DWORD dwCallWxid = m_WeDllBase + WESHENGCHENGWXIDOFFSET;
	DWORD dwCalladd = m_WeDllBase + WEJINQUNYAOQINGOFFSET;
	DWORD dwZiJiWxid = m_WeDllBase + WECEXOFFSET;
	DWORD dwBixuDiaoyong = m_WeDllBase + WEBIXUZHIXINGOFFSET;
	DWORD dwCallQun = m_WeDllBase + WECHATQUNOFFSET;
	DWORD dwBase = m_WeDllBase + WEJINQUNECXOFFSET;
	DWORD dwCallAddRoom=m_WeDllBase + WEJINQUNYAOQING2OFFSET;
	char arr[0x14] = {};
	char TmpEcx[0x14] = {};
	char TmpEcx2[0x14] = {};
	__asm
	{
	pushad
		lea esi,arr
		mov ecx,esi
		mov eax, asmWxid
		push eax
		call dwCallWxid

		push 0
		push dwZiJiWxid
		lea ecx, TmpEcx
		call dwBixuDiaoyong

		sub esp, 0x14
		lea ecx, My
		mov eax,esp
		push eax
		call dwCallQun
		lea esi, arr
		push esi
		mov ecx, dwBase
		call dwCalladd
	/*******下面是超过40人再次邀请************/
		push 0
		push dwZiJiWxid
		lea ecx, TmpEcx
		call dwBixuDiaoyong
		sub esp, 0x14
		lea ecx, My
		mov eax, esp
		push eax
		call dwCallQun
		lea eax,arr
		push eax
		call dwCallAddRoom
	popad
	}
}
VOID CWeChatDll::SendWxXML(CStringW strWxid_1, CStringW strWxid_2, CStringW strName)
{
	CStringW strXML;
	strXML.Format(L"<?xml version=\"1.0\"?><msg bigheadimgurl=\"http://wx.qlogo.cn/mmhead/ver_1/7IiaGRVxyprWcBA9v2IA1NLRa1K5YbEX5dBzmcEKw4OupNxsYuYSBt1zG91O6p07XlIOQIFhPCC3hU1icJMk3z28Ygh6IhfZrV4oYtXZXEU5A/0\" smallheadimgurl=\"http://wx.qlogo.cn/mmhead/ver_1/7IiaGRVxyprWcBA9v2IA1NLRa1K5YbEX5dBzmcEKw4OupNxsYuYSBt1zG91O6p07XlIOQIFhPCC3hU1icJMk3z28Ygh6IhfZrV4oYtXZXEU5A/132\" username=\"%s\" nickname=\"%s\" fullpy=\"?\" shortpy=\"\" alias=\"%s\" imagestatus=\"3\" scene=\"17\" province=\"湖北\" city=\"中国\" sign=\"\" sex=\"2\" certflag=\"0\" certinfo=\"\" brandIconUrl=\"\" brandHomeUrl=\"\" brandSubscriptConfigUrl= \"\" brandFlags=\"0\" regionCode=\"CN_Hubei_Wuhan\" />", strWxid_2, strName, strWxid_2);
#ifdef DEBUG
	MessageBoxW(NULL, strXML, L"", MB_OK);
#endif
	
	typedef struct WXXML
	{
		wchar_t* szXML;
		int dlen1;
		int dlen2;
		int dlen3;
	} WEXML;
	typedef struct WXID
	{
		wchar_t* szWXID;
		int dlen1;
		int dlen2;
		int dlen3;
	} WEWXID;
	WEXML* xml = new WEXML{ strXML.GetBuffer(),strXML.GetLength(),strXML.GetLength() * 2 ,0 };
	WEWXID* wxid = new WEWXID{ strWxid_1.GetBuffer(), strWxid_1.GetLength(), strWxid_1.GetLength() * 2 ,0 };
	char arr[0x218] = { 0 };
	DWORD dBase = m_WeDllBase + WESENDMXLOFFSET;
	_asm
	{
		push 0x2A
		mov eax, xml
		mov edx, wxid
		push 0x0
		push eax
		lea ecx, arr
		call dBase
		add esp, 0xC
	}
	strXML.ReleaseBuffer();
	strWxid_1.ReleaseBuffer();
	delete xml;
	delete wxid;
}
DWORD CWeChatDll::LogOut()
{
	DWORD dwLogOut=m_WeDllBase + WECHATEXITOFFSET;
	return ((LOGOUT)dwLogOut)(0);
}
VOID CWeChatDll::DeleUser(LPWSTR szWxid)
{
	DWORD dwCall=m_WeDllBase + WEDELETEHAOYOUOFFSET;
	MYWXID MyWxid;
	MYWXID* pMyWxid;
	MyWxid.lpWxid = szWxid;
	MyWxid.nlen = (int)wcslen(szWxid);
	MyWxid.nVal = (int)wcslen(szWxid) * 2;
	pMyWxid = &MyWxid;
	_asm
	{
		pushad
		push 0
		push pMyWxid
		call dwCall
		popad
	}
}
BOOL CWeChatDll::IsWeChatInitOk()
{
	DWORD dwBase=m_WeDllBase+ ISWECHATOK;
	return *(DWORD*)dwBase;
}
BOOL CWeChatDll::ZiDongYesHaoYou(LPWSTR pV1, LPWSTR pV2)
{
	DWORD dwV2Call=m_WeDllBase + WEYESADDHAOYOU;
	DWORD dwCall = m_WeDllBase + WEYESADDHAOYOU2;
	DWORD dwEcx=m_WeDllBase+ WEYESADDHAOYOU3;
	DWORD dwCallAddess= m_WeDllBase + WEYESADDHAOYOU4;
	DWORD dwCallend= m_WeDllBase + WEYESADDHAOYOU5;
	WECHATV2DATA v2 = {};
	WECHATV1DATA v1 = {};
	v1.v2 = (DWORD)&v2.Memy;
	v1.v1 = pV1;
	v1.nlen= wcslen(pV1);
	v1.nlen2 = wcslen(pV1)*2;
	v2.v2Data = pV2;
	v2.v2Len = wcslen(pV2);
	v2.v2len2 = wcslen(pV2)*2;
	char* asmUser = (char*)&v1.fill;
	char buff[0x14] = {0};
	char buff2[0x48] = {0};
	char* arr = buff2;
	__asm
	{
		mov ecx, asmUser
		push 6
		sub esp,0x14
		push esp
		call dwV2Call
		mov ecx,asmUser
		lea eax,buff
		push eax
		call dwCall
		mov esi, eax
		sub esp, 0x8
		mov ecx, dwEcx
		call dwCallAddess
		mov ecx,arr
		mov edx,ecx
		push edx
		push eax
		push esi
		call dwCallend
	}
}
VOID CWeChatDll::DeleQunLiao(LPWSTR szQid)
{
		DWORD dwCallDele = m_WeDllBase + WEDELEQUNLIAOOFFSET;
		MYWXIDEX My, *pMy;
		My.lpWxid = szQid;
		My.nlen = (int)wcslen(szQid) * 2;
		My.nVal = (int)wcslen(szQid) * 2;
		memset(My.arr, 0, sizeof(My.arr));
		pMy = &My;
		_asm
		{
			pushad
			push pMy
			call dwCallDele
			popad
		}
}
LISTHAOYOU & CWeChatDll::GetHaoYouList()
{
	DWORD* dwpTou = (DWORD*)(m_WeDllBase + WEBIANLIQUNOFFSET);
	DWORD dwTou = ((*dwpTou) + 0x68 + 0x24);//获取二叉树 指针
	DWORD* dwBase = _LPBASE(dwTou);
	DWORD* dwP = _LPBASE(*dwBase);
	DWORD dwGenJieDian = *(DWORD*)((DWORD)dwP + 0x4);
	return GetAddHaoYouList(dwGenJieDian);
}
LISTHAOYOU listHaoYou;
LISTHAOYOU &CWeChatDll::GetAddHaoYouList(DWORD dwGenJieDian)
{
	//二叉树结构
	//获取二叉树头
	listHaoYou.RemoveAll();
	DWORD dwHeader = dwGenJieDian;
	if (0 == dwHeader)
	{
	return listHaoYou;
	}
	pBinaryTree pTree = reinterpret_cast<pBinaryTree>(dwHeader);
	stack<pBinaryTree> pStack1, pStack2;
	pStack1.push(pTree);
	while (!pStack1.empty())
	{
		pBinaryTree pTempTree = pStack1.top();
		pStack1.pop();
		if (pTempTree->s_by_endflags == 0)
		{
			pStack2.push(pTempTree);
			if ((pTempTree->s_tree_pleft) && (pTree != pTempTree->s_tree_pleft))
				pStack1.push(pTempTree->s_tree_pleft);
			if ((pTempTree->s_tree_pright) && (pTree != pTempTree->s_tree_pright))
				pStack1.push(pTempTree->s_tree_pright);
		}
	}
	while (!pStack2.empty())
	{
		pBinaryTree pTreeItem =pStack2.top();
		//过滤群
		CStringW strWxid = (LPWSTR)*(DWORD*)((DWORD)pTreeItem + 0x10);
		if (strWxid.Find(L"@chatroom")==-1)
		{
			m_MyHaoYou.szName = (LPWSTR)*(DWORD*)(((DWORD)pTreeItem)+0x8C);//好友昵称
			m_MyHaoYou.szWxid = (LPWSTR)*(DWORD*)(((DWORD)pTreeItem)+ 0x10);//获取WXID
			m_MyHaoYou.szWxHao = (LPWSTR)*(DWORD*)(((DWORD)pTreeItem) + 0x44);//获取WX号
			listHaoYou.AddHead(m_MyHaoYou);
		}
		pStack2.pop();
	}
	return listHaoYou;
}
LISTQUN listQun;
LISTQUN& CWeChatDll::GetRoomXinXi()
{
	DWORD* dwpTou = (DWORD*)(m_WeDllBase + WEBIANLIQUNOFFSET);
	DWORD dwTou = ((*dwpTou) + 0x68 + 0x24);//获取二叉树 指针
	DWORD* dwBase = _LPBASE(dwTou);
	DWORD* dwP = _LPBASE(*dwBase);
	DWORD dwGenJieDian = *(DWORD*)((DWORD)dwP + 0x4);
	return GetAddRoomList(dwGenJieDian);
}
LISTQUN& CWeChatDll::GetAddRoomList(DWORD dwList)
{
	//二叉树结构
	//获取二叉树头
	listQun.RemoveAll();
	DWORD dwHeader = dwList;
	if (0 == dwHeader)
	{
		return listQun;
	}
	pBinaryTree pTree = reinterpret_cast<pBinaryTree>(dwHeader);
	stack<pBinaryTree> pStack1, pStack2;
	pStack1.push(pTree);
	while (!pStack1.empty())
	{
		pBinaryTree pTempTree = pStack1.top();
		pStack1.pop();
		if (pTempTree->s_by_endflags == 0)
		{
			pStack2.push(pTempTree);
			if ((pTempTree->s_tree_pleft) && (pTree != pTempTree->s_tree_pleft))
				pStack1.push(pTempTree->s_tree_pleft);
			if ((pTempTree->s_tree_pright) && (pTree != pTempTree->s_tree_pright))
				pStack1.push(pTempTree->s_tree_pright);
		}
	}
	while (!pStack2.empty())
	{
		pBinaryTree pTreeItem = pStack2.top();
		//过滤群
		CStringW strWxid = (LPWSTR)*(DWORD*)((DWORD)pTreeItem + 0x10);
		if (strWxid.Find(L"@chatroom") != -1)
		{
			m_MyQun.szName = (LPWSTR)*(DWORD*)(((DWORD)pTreeItem) + 0x8C);//好友昵称
			m_MyQun.szQId = (LPWSTR)*(DWORD*)(((DWORD)pTreeItem) + 0x10);//获取WXID
			listQun.AddHead(m_MyQun);
		}
		pStack2.pop();
	}
	return listQun;
}

ARRWXID arrWxid;
ARRWXID& CWeChatDll::GetRoomWxid(LPWSTR szID)
{
	DWORD* dwpTou = (DWORD*)(m_WeDllBase + WECHATROOMINF);
	DWORD dwTou = ((*dwpTou) + 0x680 + 0x64);//获取二叉树 指针
	DWORD* dwBase = _LPBASE(dwTou);
	DWORD* dwP = _LPBASE(*dwBase);
	DWORD dwGenJieDian = *(DWORD*)((DWORD)dwP + 0x4);
	return GetRoomArrWxid(dwGenJieDian, szID);
}
ARRWXID& CWeChatDll::GetRoomArrWxid(DWORD dwList,LPWSTR szID)
{
	//二叉树结构
	//获取二叉树头
	arrWxid.RemoveAll();
	DWORD dwHeader = dwList;
	if (0 == dwHeader)
	{
		return arrWxid;
	}
	pBinaryTree pTree = reinterpret_cast<pBinaryTree>(dwHeader);
	stack<pBinaryTree> pStack1, pStack2;
	pStack1.push(pTree);
	while (!pStack1.empty())
	{
		pBinaryTree pTempTree = pStack1.top();
		pStack1.pop();
		if (pTempTree->s_by_endflags == 0)
		{
			pStack2.push(pTempTree);
			if ((pTempTree->s_tree_pleft) && (pTree != pTempTree->s_tree_pleft))
				pStack1.push(pTempTree->s_tree_pleft);
			if ((pTempTree->s_tree_pright) && (pTree != pTempTree->s_tree_pright))
				pStack1.push(pTempTree->s_tree_pright);
		}
	}
	while (!pStack2.empty())
	{
		pBinaryTree pTreeItem = pStack2.top();
		//过滤群
		CStringW strWxid = (LPWSTR)*(DWORD*)((DWORD)pTreeItem + 0x10);

		if (strWxid.CompareNoCase(szID)==0)
		{
			CStringW strWxidBase= (LPWSTR)*(DWORD*)(((DWORD)pTreeItem) + 0x3C);//待分割群成员WXID
			int nNum = 0;
			CStringW* strParrWxid=StrParTition(strWxidBase.GetBuffer(),L"^G", nNum);
			strWxidBase.ReleaseBuffer();
			for (int i=0;i<nNum;i++)
			{
				LPWSTR szWxid=strParrWxid[i].GetBuffer();
				arrWxid.Add(szWxid);
				strParrWxid[i].ReleaseBuffer();
			}
			break; //不再继续遍历群直接返回
		}
		pStack2.pop();
	}
	return arrWxid;
}
VOID CWeChatDll::ZiDongShouKuan(LPWSTR pWxid, LPWSTR pDingDan)
{
	struct Wxid
	{
		LPWSTR swDingdan;
		int nlen;
		int nlenMax;
		int mem1 = 0;
		int mem2 = 0;
	};
	char arrWxid[0x20] = { 0 };
	char arr[500] = { 0 };
	DWORD dwCall1 = m_WeDllBase + WEYESSHOUKUAN1;
	DWORD dwCall2 = m_WeDllBase + WEYESSHOUKUAN2;
	ShouKuan rmb = {};
	rmb.szDanHao = pDingDan;
	rmb.len = wcslen(pDingDan);
	rmb.lenMax = wcslen(pDingDan)*2;
	rmb.szWxid = pWxid;
	rmb.len1 = wcslen(pWxid);
	rmb.lenMax1 = wcslen(pWxid) * 2;
	//PShouKuan pRmb = &rmb;
	PShouKuan pRmb = new ShouKuan(rmb);
	__asm
	{
		pushad
		pushfd
		sub esp,0x30
		mov ecx, esp
		push pRmb
		call dwCall1
		call dwCall2
		add esp,0x30
		popfd
		popad
	}
	delete pRmb;
}
char *CWeChatDll::GetXiangxiBase(LPWSTR szID)
{
	DWORD dwCall1 = m_WeDllBase + WEXIANGXIXINXICALL1;
	DWORD dwCall2 = m_WeDllBase + WEXIANGXIXINXICALL2;
	DWORD dwCall3 = m_WeDllBase + WEXIANGXIXINXICALL3;
	char* arrBuf = new char[0x500];
	MYWXID MyWxid;
	MyWxid.lpWxid = szID;
	MyWxid.nlen = wcslen(szID);
	MyWxid.nVal = wcslen(szID) * 2;
	__asm
	{
		pushad
		lea edi, arrBuf
		push edi
		sub esp, 0x14
		lea eax, MyWxid
		mov ecx, esp
		push eax
		call dwCall1
		call dwCall2
		call dwCall3
		popad
	}
	return arrBuf;
}
XINXIJIEGOU& CWeChatDll::GetXiangxiXinxi(LPWSTR szID)
{
	//AfxMessageBox("222");
	DWORD dwCall1 = m_WeDllBase + WEXIANGXIXINXICALL1;
	DWORD dwCall2 = m_WeDllBase + WEXIANGXIXINXICALL2;
	DWORD dwCall3 = m_WeDllBase + WEXIANGXIXINXICALL3;
	char arrBuf[0x500] = {};
	MYWXID MyWxid;
	MyWxid.lpWxid = szID;
	MyWxid.nlen = wcslen(szID);
	MyWxid.nVal = wcslen(szID) * 2;
	__asm
	{
		pushad
		lea edi, arrBuf
		push edi
		sub esp,0x14
		lea eax,MyWxid
		mov ecx,esp
		push eax
		call dwCall1
		call dwCall2
		call dwCall3
		popad
	}
	m_MyData.szWxid = _PSTRBASE(arrBuf + 0x8);
	m_MyData.szWxHao = _PSTRBASE(arrBuf + 0x1C);
	m_MyData.szV1 = _PSTRBASE(arrBuf + 0x30);
	m_MyData.szName = _PSTRBASE(arrBuf + 0x64);
	return m_MyData;
}
QUNCHENGYUANLIST g_arrQun;
QUNCHENGYUANLIST& CWeChatDll::GetAddRoomXinxi(LPWSTR szID)
{
	g_arrQun.RemoveAll();
	ARRWXID& arrWxid=GetRoomWxid(szID);
	for (int i=0;i<arrWxid.GetSize();i++)
	{
		g_arrQun.AddHead(GetXiangxiXinxi(arrWxid[i]));
	}
	return g_arrQun;
}
VOID CWeChatDll::SetBeiZhu(LPWSTR szWxid, LPWSTR szName)
{
	MYWXID stName = {0};
	stName.lpWxid = szName;
	stName.nlen = wcslen(szName);
	stName.nVal= wcslen(szName) * 2;
	DWORD dwCall1=m_WeDllBase + WECHATBEIZHU1;
	DWORD dwCall2 = m_WeDllBase + WECHATBEIZHU2;
	DWORD dwCall3 = m_WeDllBase + WECHATBEIZHU3;
	char* arr = GetXiangxiBase(szWxid);

	__asm
	{
		pushad
		pushfd
		lea eax, stName
		mov esi, arr
		mov dword ptr ds:[esi+0x224],0xFFFFFFFF
		mov[esi + 0x228],3
		mov ecx,esi
		push eax
		call dwCall1
		call dwCall2
		push ecx
		push esi
		call dwCall3
		popfd
		popad
	}
	delete[]arr;
}


/************************************收藏函数的实现*****************************************************/
DWORD CWeChatDll::enumContainer(DWORD pParent, CString strText, int nType) {
	if (!pParent) return NULL;
	DWORD pRet = 0;
	try
	{
		DWORD dwpCall = _LPDWORD(_LPDWORD(pParent) - 0xF5CEC);
		//此处特征码55 8B EC 83 E4 F8 83 EC 2C A1 ???????? 33 C4 89 44 24 28 53 56 8B 75 08 8B D9 8B 4D 10 89 7424 10 89 4C 24 08 57
		DWORD dwEnumCallAddr1 = m_WeDllBase + WECHATSHOUCANG2;
		//此处特征码55 8B EC F6 45 10 01 56 57 8B F1 74 10 8B 06 FF 90 CC 00 00 00 84 C0 75 04 33 C0 EB 41 F6 45 1002 74 0E
		DWORD dwEnumCallAddr2 = m_WeDllBase + WECHATSHOUCANG3;
		if (dwpCall != dwEnumCallAddr1 && dwpCall != dwEnumCallAddr2) return NULL;

		updateData(pParent);
		wchar_t* lpcszClass = getClass();
		wchar_t* lpcszText = getText();
		LOG(_T("p=%x,Parent=%x,Class=%s,Text=%s,size=%d,19A=%d"), m_pUI, _LPDWORD(m_pUI + 0x44), lpcszClass, lpcszText, _LPDWORD(m_pUI + 0x4B0), *(LPBYTE)(m_pUI + 0x19A));
		if (lpcszClass[0]) {
			if (!wcscmp(lpcszClass, L"chatlist"))
			{
				m_pContact_list = m_pUI;
			}
		}
		if (UI_CLASS == nType) {
			if (strText == lpcszClass) return m_pUI;
		}
		else if (UI_TEXT == nType) {
			if (strText == lpcszText) {
				LOG(_T("p=%x,Class=%s,Text=%s,string=%s"), m_pUI, lpcszClass, lpcszText, strText.GetBuffer(0));
				return m_pUI;
			}
		}
		DWORD nSize = getSize();
		if (!nSize) return pRet;
		DWORD pChildStartAddr = getChildStartAddr();
		for (DWORD i = 0; i < nSize; i++)
		{
			DWORD pChild = getChild(pChildStartAddr, i);
			if (!pChild) break;

			pRet = enumContainer(pChild, strText, nType);
			if (pRet) break;
		}
	}
	catch (...) {
		OutputDebugString(_T("enumContainer..."));
	}
	return pRet;
}
DWORD CWeChatDll::getRoot()
{
	AfxMessageBox("11111");
	DWORD root = m_WeDllBase + WECHATSHOUCANG1;
	__try {
		root = _LPDWORD(root);
		root = _LPDWORD(root + 0x2C + 0x54);
	}
	__except (1) {
		OutputDebugString(_T("getRoot..."));
	}
	return root;
}
CString getAt(CList <CString, CString> &pList, int n)
{
	POSITION p = pList.FindIndex(n);
	return pList.GetAt(p);
}
LPWXSTRINGARRAY CreateWxStringArray(CList <CString, CString> &strArray)
{
	LPWXSTRINGARRAY pWxStringArray = (LPWXSTRINGARRAY)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WXSTRINGARRAY));
	pWxStringArray->pHead = (LPWXSTRING)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WXSTRING) * strArray.GetCount());
	pWxStringArray->pEnd1 = pWxStringArray->pHead + strArray.GetCount();
	pWxStringArray->pEnd2 = pWxStringArray->pHead + strArray.GetCount();
	for (int i = 0; i < strArray.GetCount(); i++)
	{
		CStringW str = (CStringW)getAt(strArray, i);
		pWxStringArray->pHead[i].lpwString = (wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, str.GetLength() * 2 + 2);
		wcscpy(pWxStringArray->pHead[i].lpwString, (wchar_t*)str.GetBuffer(0));
		pWxStringArray->pHead[i].Len = str.GetLength();
		pWxStringArray->pHead[i].Len2 = pWxStringArray->pHead[i].Len * 2;
	}
	return pWxStringArray;
}
void sendCollection(CList <CString, CString> &strArray, int Index)
{
	LPWXSTRINGARRAY pUsersString = CreateWxStringArray(strArray);
	//pUsersString->pHead->lpwString
	DWORD pS = (DWORD)pUsersString->pHead, pE = (DWORD)pUsersString->pEnd1;
	BSTR pHeapString = (BSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, wcslen(L"ForwardFav") * 2 + 2);
	wcscpy(pHeapString, L"ForwardFav");
	DWORD pUI = g_WeChatDll.enumContainer(g_WeChatDll.getRoot(), "all_fav_list", UI_CLASS);
	if (!pUI) return;
	DWORD pLoadUICall = g_WeChatDll.m_WeDllBase+ WECHATSHOUCANG4;
	if (!_LPDWORD(pUI + 0xA54))
	{
		__asm
		{
			pushad
			mov ecx, pUI
			call pLoadUICall
			popad
		}
	}
	DWORD dwEcx = pUI + 0xA50;
	CString strBase;
	strBase.Format("%X", dwEcx);
	MessageBox(0, strBase, 0, 0);
	_LPDWORD(dwEcx + 0x18) = Index;
	__asm
	{
		pushad
		mov esi, pUI
		push 0
		push pE
		push pE
		push pS
		push 0
		push 0
		push 0x10
		push 0xA
		push pHeapString
		mov     ecx, dwEcx//mov     ecx, dword ptr [esi+32C]//但是这里是A20
		mov     eax, dword ptr[ecx]
		mov     eax, dword ptr[eax]
		call    eax
		popad
	}
	HeapFree(GetProcessHeap(), 0, pUsersString);
}
VOID CWeChatDll::sendCollections(CString lpWxID, int Index)
{
	CList <CString, CString> strArray;
	strArray.AddHead(lpWxID);
	sendCollection(strArray, Index);
}
/**********************************************************************************************************/

VOID CWeChatDll::SendUrlAddRoom(LPWSTR szUrl,LPWSTR szWxid)
{
	DWORD dwCall1 = m_WeDllBase + WECHATGETKEY1;
	DWORD dwCall2 = m_WeDllBase + WECHATGETKEY2;
	DWORD dwEax = m_WeDllBase + WECHATGETKEY3;
	DWORD dwCall3 = m_WeDllBase + WECHATGETKEY4;
	DWORD dwCall4 = m_WeDllBase + WECHATGETKEY5;
	DWORD dw1 = m_WeDllBase +  WECHATGETKEY6;
	DWORD dw2 = m_WeDllBase +WECHATGETKEY7;
	DWORD dwEcx = m_WeDllBase + WECHATGETKEY8;
	DWORD dwCallend = m_WeDllBase + WECHATGETKEY9;
	DWORD *pdwExa = &dwEax;
	char buf[521] = { 0 };
	
	__asm {
		//pushad
		//pushfd
		push 0x1A0
		call dwCall1   // 1
		mov esi, eax
		sub esp, 0x14
		mov ecx, esp
		push - 0x1
		mov dword ptr ds : [ecx], 0x0
		mov dword ptr ds : [ecx + 0x4], 0x0
		mov dword ptr ds : [ecx + 0x8], 0x0
		mov dword ptr ds : [ecx + 0xC], 0x0
		mov dword ptr ds : [ecx + 0x10], 0x0
		push szWxid  //wxid 6D0A0000
		call dwCall2    ///2
		mov eax, pdwExa//3
		push 0x2
		push 0x1
		push 0x0
		push 0x1
		sub esp, 0x14
		mov ecx, esp
		push - 0x1
		mov dword ptr ds : [ecx], 0x0
		mov dword ptr ds : [ecx + 0x4], 0x0
		mov dword ptr ds : [ecx + 0x8], 0x0
		mov dword ptr ds : [ecx + 0xC], 0x0
		mov dword ptr ds : [ecx + 0x10], 0x0
		push szUrl  //
		call dwCall3 //4
		mov ecx, esi
		call dwCall4// 5
		push eax
		lea ecx, buf
		call dw1  //6
		sub esp, 0x8
		lea eax, buf
		mov ecx, esp
		push eax
		call dw2  //7
		call dwEcx //8
		mov ecx, eax
		call dwCallend //9
		//add esp,4
		//popfd
		//popad
	}
}

VOID CWeChatDll::HookGetKey8()
{
	DWORD dwHookBase=m_WeDllBase + WECHATHOOKGETKEY8;
	g_dwJmpKey8 = dwHookBase + 0x7;
	MH_CreateHook((LPVOID)dwHookBase, GetKey8url, NULL);
	MH_EnableHook((LPVOID)dwHookBase);
}
#import "C:\\Windows\\System32\\WinHttpcom.dll" no_namespace

VOID CWeChatDll::PostAddRoom(LPWSTR szURL)
{
	char                *pJsonPostData;
	CHttpClient m_HttpClient;
	CStringA strUrl = (CStringA)szURL;
	CString             strText;
	CoInitialize(NULL);//初始化POSTCOM组件
	IWinHttpRequestPtr pHttpReq = NULL;//组件后面加Ptr 是智能指针
	HRESULT hr = pHttpReq.CreateInstance(__uuidof(WinHttpRequest));//传递类名
	if (FAILED(hr)) return;
	pHttpReq->Open(L"GET", szURL);
	if (FAILED(hr))
	{
		CoUninitialize();
		return;
	}
	
	hr = pHttpReq->Send(NULL);
	_variant_t Body = pHttpReq->GetResponseBody();//接收传过来的内容
	ULONG dataLen = Body.parray->rgsabound[0].cElements;//获取接收到的字节数
	char* Test = (char*)Body.parray->pvData;//宽字节到载字节
	strText =UTF8ToGB(Test);
	
	g_WeChatDll.m_nVal++;
	
	//AfxMessageBox(strText);
	if (strText.Find("该邀请已过期") != -1)
	{
		g_WeChatDll.m_nNumOn++;
		return;
	}
	else if (strText.Find("你已接受邀请") != -1)
	{
		g_WeChatDll.m_nNumOn++;
		return;
	}
	else if (strText.Find("该邀请已发送") != -1)
	{
		g_WeChatDll.m_nNumOn++;
		return;
	}
	else if (strText.Find("此群聊是你邀请别人的") != -1)
	{
		g_WeChatDll.m_nNumOn++;
		return;
	}
	m_HttpClient.HttpPost(strUrl, pJsonPostData, strText);
	m_HttpClient.HttpPost(strUrl, pJsonPostData, strText);
	//AfxMessageBox(strText+"\n\n");
	g_WeChatDll.m_nNumOk++;
	CoUninitialize();

}