#pragma once
#include"WeBianLiang.h"
#ifdef WECHATDLL_EXPORTS
#define WECHATDLL_API __declspec(dllexport)
#else
#define WECHATDLL_API __declspec(dllimport)
#endif

void  GetKey8url();
class MYQUNLIST
{
public:
	LPWSTR szQid;
	LPWSTR szQName;
	LPWSTR szQzWxid;
	LPWSTR szQChengYuan;
};
//保存着好友信息
class MYWEHAO
{
public:
	LPWSTR szSex;
	LPWSTR szName;
	LPWSTR szWxid;
	LPWSTR szWxHao;
};
class MYWEQUN
{
public:
	LPWSTR szName;
	LPWSTR szQId;
};
typedef CArray<LPWSTR, LPWSTR&> ARRWXID;

typedef CList <MYWEQUN, MYWEQUN&> LISTQUN;
typedef CList <MYWEHAO, MYWEHAO&> LISTHAOYOU;
/*
声明1：本源码是本人利用业余时间所写，《免费》 仅供逆向编程爱好者学习使用！本源码禁止用于商业非法用途！
声明2：本源码无任何恶意代码！但可能存在些许BUG 因此所造成的损失等概与本人无关
声明3：使用编译本源码即默认同意上述声明+
*/
/*
Statement 1: the source code is written in my spare time, "free"
only for reverse programming enthusiasts to learn!This source code is prohibited for commercial illegal use!

Statement 2: this source code without any malicious code!But there may be a little BUG so the
loss caused by the loss has nothing to do with me
*/
/*******************************************************/
/*********支持的微信版本:2.7.1.82***************************/
/*********作者：亿万少女的梦***************************/
/*********时间：20191101***************************/
/*********QQ：2865298812***************************/
/*********邮箱：670088124@qq.com***************************/
/*********QQ交流群：635433739***************************/
/*******************************************************/

class WECHATDLL_API CWeChatDll {
public:
/*******************************************************/
/*********功能：数据初始化 必须优先调用一次************/
/*******************************************************/
	BOOL InitData();
/*******************************************************/
/*********功能：获取个人信息****************************/
/*********返回值：返回一个保存着个人信息的引用结构MYDATANAME***/
/*******************************************************/
	MYDATANAME& GetWeChatGerenXinxi();
/*******************************************************/
/*********功能：发送消息****************************/
/*********参数1：对方的WXID*************************/
/*********参数2：要发送的内容************************/
/*********返回值：无*********************/

/*******************************************************/
	VOID SendMsg(LPWSTR pWxid, LPWSTR pwMsg);
/*******************************************************/
/*********功能：接收消息****************************/
/*********返回值：成功返回TRUE 基本不会失败*********************/
	BOOL HookGetRegiXinxi();
/*******************************************************/
/*********功能：被动接收消息****************************/
/*********参数1：如何参数3 为NULL 这里存放着发送着的WXID 否则为群ID*****/
/*********参数2：接收到的内容************************/
/*********参数3：如果是群消息存放的是发送者的WXID 否则为NULL***********/
/*********返回值：无*********************/
/*********只用调用 HookGetRegiXinxi 后此函数才会被调用********/
	static VOID RecvWxMsg(LPWSTR szWxid, LPWSTR szMsg, LPWSTR szID);
/*******************************************************/
/*********功能：HOOK拦截微信二维码****************************/
/*********返回值：无*********************/
	VOID HookReWeiMaPng();
/***************************************************************************/
/*********功能：被动获取微信二维码 必须调用HookReWeiMaPng此函数才会生效*************/
/*********返回值：无*************************************************************/
	static VOID GetErWeiMa(BYTE* ByBuf, DWORD dwlen);

/*********功能：邀请好友进群***************************************/
/*********参数1：好友wxid********************************/
/*********参数2：群ID******************************************/
	BOOL SendYaoQingQunLiao(LPWSTR szWxid, LPWSTR szQid);

/*********功能：名片发送XML***************************************************/
/*********参数1:接受名片者的WXID**********************************************/
/*********参数2：被发送者的WXID**********************************************/
/*********参数3：被发送者的昵称 你i想让对方看到什么名字就填写什么*******************/
	VOID SendWxXML(CStringW strWxid_1, CStringW strWxid_2, CStringW strName);
/*********功能：微信退出登陆*********************************/
	DWORD  LogOut();
/*********功能：删除好友**********************************/
/*********参数：目标WXID**********************************/
	VOID DeleUser(LPWSTR szWxid);
/*********功能：判断是否完成登陆**********************************/
	BOOL IsWeChatInitOk();
/*********功能：退出群聊****************************************/
/*********参数1：目标群ID****************************************/
	VOID DeleQunLiao(LPWSTR szQid);
/*********功能：获取群列表****************************************/
/*********返回值：返回群列表****************************************/
	LISTQUN& GetRoomXinXi();
/*********功能：递归获取微信群列表****************************/
/*********参数：二叉树根节点****************************/
/*********返回值：返回微信群列表***********************/
	LISTQUN& GetAddRoomList(DWORD dwList);

/*********功能：双栈方式获取二叉树好友列表****************************/
/*********参数：二叉树头节点地址****************************/
/*********返回值：返回LISTHAOYOU的引用****************************/
LISTHAOYOU &GetAddHaoYouList(DWORD dwList);
/*********功能：获取微信好友信息****************************/
/*********参数：无****************************/
/*********返回值：返回LISTHAOYOU的引用****************************/
	LISTHAOYOU & GetHaoYouList();
/*********功能：自动同意添加好友****************************/
/*********参数1：V1数据****************************/
/*********参数2：V2数据****************************/
	BOOL ZiDongYesHaoYou(LPWSTR pV1,LPWSTR pV2);
/*********功能：自动收款****************************/
/*********参数1：转账人WXID****************************/
/*********参数2：订单号****************************/
	VOID ZiDongShouKuan(LPWSTR pWxid, LPWSTR pDingDan);
/*********功能：获取群成员WXID****************************/
/*********参数1：群ID****************************/
	ARRWXID& GetRoomWxid(LPWSTR szID);
	ARRWXID& CWeChatDll::GetRoomArrWxid(DWORD dwList, LPWSTR szID);
/*********功能：通过WXID获取对方详细信息****************************/
/*********参数1：目标WXID****************************/
/*********返回值：个人信息结构体****************************/
	XINXIJIEGOU& CWeChatDll::GetXiangxiXinxi(LPWSTR szID);
	char *CWeChatDll::GetXiangxiBase(LPWSTR szID);
/*********功能：获取群成员列表****************************/
/*********参数1：目标群ID****************************/
/*********返回值：群成员信息链表****************************/
	QUNCHENGYUANLIST& GetAddRoomXinxi(LPWSTR szID);
/*********功能：设置好友备注****************************/
/*********参数1：好友WXID****************************/
/*********参数2：要备注的昵称****************************/
	VOID SetBeiZhu(LPWSTR szWxid, LPWSTR szName);
/*********功能：自动进群****************************/
/*********参数1：群的URL****************************/
/*********参数2：邀请人的WXID****************************/
	VOID  SendUrlAddRoom(LPWSTR szURL, LPWSTR szWxid);
/*********功能：HOOK截取完整的URL****************************/
	VOID HookGetKey8();
/*********功能：POST进群****************************/
/*********参数1：群的URL****************************/
	static VOID PostAddRoom(LPWSTR szURL);


/*********功能：收藏发送****************************/
/*********参数1：对方的WXID****************************/
/*********参数2：收藏的索引从0开始****************************/
	VOID sendCollections(CString lpWxID, int Index);
	DWORD enumContainer(DWORD pParent, CString strText, int nType);
	DWORD getRoot();
	//void sendCollection(CList <CString, CString> &strArray, int Index);
	DWORD m_pUI;
	void updateData(DWORD p)
	{
		m_pUI = p;
	}
	wchar_t* getClass()
	{
		return (wchar_t*)_LPDWORD(m_pUI + 0xCC);
	}
	wchar_t* getText()
	{
		return (wchar_t*)(m_pUI + 0x1BC);
	}
	DWORD getSize()
	{
		DWORD nSize = _LPDWORD(m_pUI + 0x4B0);
		if (nSize > 0x100) nSize = 0;

		return nSize;
	}
	DWORD getChildStartAddr()
	{
		return _LPDWORD(m_pUI + 0x4AC);
	}
	DWORD getChild(DWORD pChildStartAddr, DWORD nIndex)
	{
		if (pChildStartAddr == 0xFFFFFFFF || pChildStartAddr < 0x10000) {
			return 0;
		}
		__try {
			return _LPDWORD(pChildStartAddr + nIndex * 4);
		}
		__except (1) {
			
		}
		return 0;
	}
/******************************************************************************************/
public:
	DWORD m_pContact_list;
	DWORD m_WeDllBase;
	MYDATANAME m_MyDataName;
	MYWEHAO m_MyHaoYou;
	MYWEQUN m_MyQun;
	XINXIJIEGOU m_MyData;
	BOOL m_IsAdd=FALSE;//是否自动同意添加好友
	BOOL m_IsShouKuan = FALSE; //自动收款
	BOOL m_IsAddRoom = FALSE;//自动进群
	int m_nVal;//收到进群邀请总次数
	int m_nNumOk;//成功进群次数
	int m_nNumOn;//失败进群次数

};




extern WECHATDLL_API int nWeChatDll;
WECHATDLL_API int fnWeChatDll(void);
