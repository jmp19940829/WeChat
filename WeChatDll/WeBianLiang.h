#pragma once
#include "stdafx.h"
//整数转指针
#define  _LPBASE(X) (DWORD*)(X)
#define _PSTRBASE(X)((LPWSTR)*(DWORD*)(X))
const CStringW GetstrXml(const CStringW& strZ, const CStringW& strY, CStringW& strXML);
CString UTF8ToGB(const char* str);
/*文本分割 如11，11，11
参数1：要分割的文本
参数2：作为分割的目标 如11，11，11要分割为3个11 此参数就填写 “，”
参数3：此参数是个引用 不是指针 输出返回 分割了多少组
返回值：返回一个CString的数组
*/
CStringW* StrParTition(LPWSTR strTxt, LPWSTR strByte, int& nNum);
void urldecode(char url[]);
typedef struct _BinaryTree
{
	//左
	_BinaryTree* s_tree_pleft;
	//中
	_BinaryTree* s_tree_pcentre;
	//右
	_BinaryTree* s_tree_pright;
	//标志
	byte                s_by_flags;
	//结束标志
	byte                s_by_endflags;

	_BinaryTree()
	{
		s_tree_pleft = nullptr;
		s_tree_pcentre = nullptr;
		s_tree_pright = nullptr;
		s_by_flags = 0;
		s_by_endflags = 0;
	}

} BinaryTree, *pBinaryTree;
#define _LPDWORD *(LPDWORD)
#define UI_CLASS 1
#define UI_NAME 2
#define UI_TEXT 3
int MyOutputDebugString(LPCTSTR pstrFormat, ...);
#define LOG MyOutputDebugString
typedef struct
{
	wchar_t* lpwString;
	DWORD Len;
	DWORD Len2;
	DWORD Zero0;
	DWORD Zero1;
}WXSTRING, *LPWXSTRING;
typedef struct
{
	LPWXSTRING pHead;
	LPWXSTRING pEnd1;
	LPWXSTRING pEnd2;
	DWORD Zero0;
	DWORD Zero1;
}WXSTRINGARRAY, *LPWXSTRINGARRAY;
//编码转换
BOOL UTF8_TO_GBK(CString &strGBK, LPCSTR lpstring);
//微信退出登陆
typedef DWORD(__stdcall *LOGOUT)(DWORD);
//接受消息的裸函数
void HookRecvMsgWx();
//拦截微信二维码
void HookEeWeiMa();
//保存接收消息回跳的地址
extern DWORD g_JmpRecvMsg;
extern DWORD g_JmpErWeiMaPng;
extern DWORD g_dwJmpKey8;//获取到进群的完整链接后JMP回原来地址
//保存微信个人信息的结构体
typedef struct DATANAME
{
	CString strWxID;
	CString strWxName;
	CString strWxHao;
	CString strURL;
}MYDATANAME, *PMYDATANAME;
//发送文件消息的结构
typedef struct SENDMSG
{
	LPWSTR lpMsg;
	int nlen;
	int nVal;
}MYSENDMSG,*PMYSENDMSG;
//发送时WXID的结构
typedef struct SENWXID
{
	LPWSTR lpWxid;
	int nlen;
	int nVal;
}MYSENDWXID, *PMYSENDWXID;
typedef struct WXIDEX
{
	LPWSTR lpWxid;
	int nlen;
	int nVal;
	char arr[72];
}MYWXIDEX, *PMYWXIDEX;
typedef struct WXID
{
	LPWSTR lpWxid;
	int nlen;
	int nVal;
	int nEnd;
}MYWXID, *PMYWXID;
typedef struct V2DATA
{
	char Memy[0x24c] = {};
	DWORD dwVal = 0x25;
	char Memy2[0x40] = {};
	wchar_t* v2Data;
	int v2Len;
	int v2len2;
	char  Memy3[8] = {};
} WECHATV2DATA, *PWECHATV2DATA;
typedef struct V1DATA
{
	int fill = 0;
	wchar_t* v1;
	int nlen;
	int nlen2;
	char fill2[0x41c] = {};
	DWORD v2 = 0;
} WECHATV1DATA, *PWECHATV1DATA;
//收款结构体
typedef struct strShouKuan
{
	LPWSTR szDanHao;
	int len;
	int lenMax;
	int null0 = 0;
	int null1 = 0;
	LPWSTR szWxid;
	int len1;
	int lenMax1;
	int end0 = 0;
	int end1 = 0;
	int end3 = 0;
}ShouKuan,*PShouKuan;
typedef struct HAOYOUXINXI
{
	LPWSTR szWxid;
	LPWSTR szWxHao;
	LPWSTR szV1;
	LPWSTR szName;
}XINXIJIEGOU,*PXINXIJIEGOU;
typedef struct STRWXID
{
	int nVal1;
	int nVal2;
	LPWSTR lpWxid;
	int nlen;
	int nlenMax;
	int end;
}STRWXIDEX;
typedef CList<XINXIJIEGOU, XINXIJIEGOU&> QUNCHENGYUANLIST;



//微信个人名称的偏移
#define WENAMEOFFSET 0x13972FC
//个人头像的URL偏移            
#define  WETOUXIANGURLOFFSET 0x13975C4
//个人微信ID偏移 
#define WEWXIDOFFSET 0x1397700
//存放微信号偏移
#define  WECHATHAO 0x13976E8
//存放微信是否登陆完成标志
#define ISWECHATOK 0x13A986C


//接收消息的CALL的偏移 
/*特征码：C70000000000C7400400000000C7400800000000A3????????50A1????????B9????????FF50*/
#define  WERECVMSGOFFSET 0x264F20
//登陆二维码拦截
//特征码：83 C4 08 84 C0 0F 85 ?? ?? ?? ?? FF 76 34 8D 4D 0C FF 76 30 E8 ?? ?? ?? ?? 8D 45 0C C7 45 FC 0000 00 00
#define WEERWEIMAPNGOFFSET 0x4A2AD0
//发送消息CALL的偏移
/**CALL特征码：8B???? 8D???? 6A01?? ??8D8D????????E8????????83C40C50 **/
#define WESENDTEXTOFFSET 0x2FA780
/************************下面是邀请进群用到的偏移*****************************************/
//生成wxid结构体的call
#define WESHENGCHENGWXIDOFFSET 0x59390

//必须调用 不调用进群会崩溃 这个是自己的微信ID
#define WECEXOFFSET 0x1397284
#define WEBIXUZHIXINGOFFSET 0x4A4560

//将群ID压入堆栈
#define WECHATQUNOFFSET 0x6A7A0
//进群邀请 CALL
//特征码：83 C4 04 C6 45 FC ?? ?? B9 ?? ?? ?? ?? C6 45 FC ?? E8 ?? ?? ?? ??
#define WEJINQUNECXOFFSET 0x13977B0
#define WEJINQUNYAOQINGOFFSET 0x2712F0

//群人数超过40需要执行这个CALL
//特征码：8D 87 ?? ?? ?? ?? C6 45 ?? ?? 50 E8 ?? ?? ?? ?? 8B 87 ?? ?? ?? ?? 8D 8F ?? ?? ?? ?? 6A 01 FF ?? 8D 8D ?? ?? ?? ?? E8 ?? ?? ?? ?? 8D 8D ?? ?? ?? ?? E8 ?? ?? ?? ?? E9 ?? ?? ?? ?? 85 F6
#define WEJINQUNYAOQING2OFFSET 0x271150
/******************************************************************************************/
//发送名片XML
//特征码：83 C4 04 C6 45 FC 02 6A 2A 8D 45 ? ? 8B D6 6A 00 50 8D 8D ? ? ? ? ? ? ? ? E8 ? ? ? ? ? ? ? ? 83 C4 0C
#define WESENDMXLOFFSET 0x2FA780
//微信退出当前登陆的函数偏移
//中文搜索：Kicked by Srv Logout onLogout(0,0,0) 上面CALL就是 
#define WECHATEXITOFFSET 0x40B330
//删除好友的CALL偏移
//中文搜索 menuCmdDeletePublicUser上面2个参数的就是
#define WEDELETEHAOYOUOFFSET 0x2809D0
//删除退出群
//特征码：53 E8 ?? ?? ?? ?? 8D 87 ?? ?? ?? ?? 8B CB 50 E8 
#define WEDELEQUNLIAOOFFSET 0x276600
//遍历群 遍历好友二叉树基址偏移
//特征码：FF 15 ?? ?? ?? ?? A1 ?? ?? ?? ?? 83 C0 24 5E C3
#define  WEBIANLIQUNOFFSET 0x1397AAC
//遍历群成员WXID
//特征码：FF 15 ?? ?? ?? ?? A1 ?? ?? ?? ?? 05 80 06 00 00 5E C3
#define WECHATROOMINF 0x126E0A0  
//自动同意添加好友
//特征码：8B D7 8D 8F ?? ?? ?? ?? F7 DA 50 1B D2 23 D1 8D 8F ?? ?? ?? ?? 52 56 E8 
#define WEYESADDHAOYOU 0x18EBC0
#define WEYESADDHAOYOU2 0x527A0
#define WEYESADDHAOYOU3 0x1397A04
#define  WEYESADDHAOYOU4 0xD37C0
#define  WEYESADDHAOYOU5 0x173ED0

//自动收款
//特征码：8B 45 34 83 EC ? ? 89 45 D0 8B CC 8D 45 A8 C7 45 D4 00 00 00 00 50 E8 ? ? ? ? ? ? ? ? E8
#define  WEYESSHOUKUAN1 0x757450  //CALL1
#define  WEYESSHOUKUAN2 0x7574D0  //CALL2

//通过WXID获取用户详细
//特征码8D BE ?? ?? ?? ?? 57 83 EC ?? 8D 45 08 8B CC 89 65 D4 50 E8 ?? ?? ?? ?? C6 45 FC 01 E8 ?? ?? ?? ?? C6 45 FC 00 E8
#define WEXIANGXIXINXICALL1 0x4A4450
#define WEXIANGXIXINXICALL2 0x53400
#define WEXIANGXIXINXICALL3 0x27F4F0

//通过WXID设置备注
//EB ?? E8 ?? ?? ?? ?? 51 56 E8
#define WECHATBEIZHU1 0x43FDE0
#define WECHATBEIZHU2 0x53400
#define WECHATBEIZHU3 0x2808A0

//收藏发送
// WECHATSHOUCANG4特征码
/*55 8B EC 6A FF 68 ? ? ? ? ? ? ? ? 64 A1 00 00 00 00 50 51 56 A1 ? ? ? ? ? ? ? ? 33 C5 50 8D 45 F4 64 A3
00 00 00 00 8B F1 83 BE 54 0A 00 00 00 0F 85 F6 00 00 00 0F 10 05*/
#define WECHATSHOUCANG4 0x620900
//WECHATSHOUCANG2特征码
//55 8B EC 83 E4 F8 83 EC 2C A1 ? ? ? ? ? ? ? ? 33 C4 89 44 24 28 53 56 8B 75 08 8B D9 8B 4D 10 89 7424 10 89 4C 24 08 57
#define WECHATSHOUCANG2 0x7CA980
//WECHATSHOUCANG3特征码
//55 8B EC F6 45 10 01 56 57 8B F1 74 ?? 8B 06 FF 90 ?? ?? ?? ?? 84 C0 75 ?? 33 C0 EB ?? F6 45 1002 74
#define WECHATSHOUCANG3 0x7C2E48

#define WECHATSHOUCANG1 0x1397A44


//自动进群获取KEY8
//83 EC 08 8D 45 E4 8B CC 89 65 1C 50 E8 ?? ?? ?? ?? C6 45 FC 04 E8 ?? ?? ?? ?? 8B C8 C6 45 FC 03E8 ? ? ? ? ? ? ? ? 8AD8
#define WECHATGETKEY1 0xB34C58
#define WECHATGETKEY2 0x4A4710
#define WECHATGETKEY3 0x11004F4
#define WECHATGETKEY4 0x4A4710
#define WECHATGETKEY5 0x38E8D0
#define WECHATGETKEY6 0x2F1360
#define WECHATGETKEY7 0x6D1D0
#define WECHATGETKEY8 0x6D4E0
#define WECHATGETKEY9 0x358E80
//HOOK得到KEY8
//6A 00 FF 70 08 E8 ?? ?? ?? ?? 8D 45 DC 50 8D 4E 24
#define WECHATHOOKGETKEY8 0x38F559

