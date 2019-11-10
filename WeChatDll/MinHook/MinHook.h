/*
 *  MinHook - The Minimalistic API Hooking Library for x64/x86
 *  Copyright (C) 2009-2017 Tsuda Kageyu.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 *  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#if !(defined _M_IX86) && !(defined _M_X64) && !(defined __i386__) && !(defined __x86_64__)
    #error MinHook supports only x86 and x64 systems.
#endif

//#include <windows.h>

// MinHook Error Codes.
typedef enum MH_STATUS
{
    // 未知的错误。不应该返回.
    MH_UNKNOWN = -1,

    // 成功的.
    MH_OK = 0,

    // MinHook已经初始化.
    MH_ERROR_ALREADY_INITIALIZED,

    // MinHook还没有被初始化，或者已经没有初始化了.
    MH_ERROR_NOT_INITIALIZED,

    // 指定目标函数的钩子已经创建好了.
    MH_ERROR_ALREADY_CREATED,

    // 指定目标函数的钩子还没有创建.
    MH_ERROR_NOT_CREATED,

    // 指定目标函数的钩子已经启用了.
    MH_ERROR_ENABLED,

    // 指定目标函数的钩子还没有启用，或者已经禁用了
    MH_ERROR_DISABLED,

    // 指定的指针无效。它指向非分配和/或不可执行区域的地址
    MH_ERROR_NOT_EXECUTABLE,

    // 指定的目标函数不能被钩住.
    MH_ERROR_UNSUPPORTED_FUNCTION,

    // 分配内存失败.
    MH_ERROR_MEMORY_ALLOC,

    // 无法改变记忆保护.
    MH_ERROR_MEMORY_PROTECT,

    // 指定的模块没有加载.
    MH_ERROR_MODULE_NOT_FOUND,

    // 指定的函数没有被找到.
    MH_ERROR_FUNCTION_NOT_FOUND
}
MH_STATUS;

// 可以作为一个参数传递给 MH_EnableHook, MH_DisableHook,
// MH_QueueEnableHook or MH_QueueDisableHook.
#define MH_ALL_HOOKS NULL

#ifdef __cplusplus
extern "C" {
#endif

	//初始化MinHook库。你必须调用这个函数一次
	//在你的程序开始的时候
    MH_STATUS WINAPI MH_Initialize(VOID);

	//取消MinHook库。你必须准确地调用这个函数
	//在你的程序结束时.
    MH_STATUS WINAPI MH_Uninitialize(VOID);

    // 方法1：为指定的目标函数创建一个钩子，在禁用状态下.
    // 参数:
    //   pTarget    [in]  一个指向Hook目标函数的指针（就是目标地址）
    //                    
    //   pDetour    [in]  指向自己回调函数的指针
    //              
    //   ppOriginal [out] A pointer to the trampoline function, which will be
    //                    used to call the original target function.
    //                    这个参数可以是 NULL.
    MH_STATUS WINAPI MH_CreateHook(LPVOID pTarget, LPVOID pDetour, LPVOID *ppOriginal);

    // 方法2：为指定的API函数创建一个钩子，在禁用状态下.
    // 参数:
	//   pszModule  [in]  在一个指向加载的模块名称的指针中，其中包含
	//                    目标函数.如（"user32"）
    //   pszTarget  [in]  指向被HOOK目标函数的地址
    //                    如：“MessageBoxW”.
    //   pDetour    [in]  指向自己回到函数的指针
    //                    用于调用原始目标函数.
    //   ppOriginal [out] 一个指向蹦床函数的指针，它将被用来调用原始的目标函数.
    //                    这个参数可以是 NULL.
    MH_STATUS WINAPI MH_CreateHookApi(
        LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour, LPVOID *ppOriginal);

    // 为指定的API函数创建一个钩子，在禁用状态下（强化版）.
    // 参数:
	//   pszModule  [in]  在一个指向加载的模块名称的指针中，其中包含
	//                    目标函数.如（"user32"）.
	//   pszTarget  [in]  指向被HOOK目标函数的地址
	//                    如：“MessageBoxW”.
	//   pDetour    [in]  指向自己回到函数的指针
	//                    用于调用原始目标函数.
    //   ppOriginal [out] A pointer to the trampoline function, which will be
    //                    used to call the original target function.
    //                    这个参数可以是 NULL.
    //   ppTarget   [out] 一个指向目标函数的指针，它将与其他函数一起使用.
    //                    这个参数可以是 NULL.
    MH_STATUS WINAPI MH_CreateHookApiEx(
        LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour, LPVOID *ppOriginal, LPVOID *ppTarget);

    // 删除一个已经创建的钩子.
    // 参数:
    // Target [in] 指向目标函数的指针（被Hook处的地址）.
    MH_STATUS WINAPI MH_RemoveHook(LPVOID pTarget);

    // 使用一个已经创建的钩子.
    // 参数:
    //   pTarget [in] 指向被目标函数的指针.
    //                如果这个参数是 MH_ALL_HOOKS, 所有创建的钩子都可以在一个go中启用    
    MH_STATUS WINAPI MH_EnableHook(LPVOID pTarget);
    // 禁用已创建的 hook.
    // 参数:
    //   pTarget [in] 指向目标函数的指针.
    //                如果这个参数是 MH_ALL_HOOKS, 所有创建的钩子都将在一个go中禁用。ooks在一个过程中是启用的
    MH_STATUS WINAPI MH_DisableHook(LPVOID pTarget);

    // Queues to enable an already created hook.
    // Parameters:
    //   pTarget [in] A pointer to the target function.
    //                If this parameter is MH_ALL_HOOKS, all created hooks are
    //                queued to be enabled.
    MH_STATUS WINAPI MH_QueueEnableHook(LPVOID pTarget);

    // 队列以启用已创建的挂钩.
    // 参数:
    //   pTarget [in] 指向目标函数的指针.
    //                如果这个参数是 MH_ALL_HOOKS, 所有创建的钩子都是排队等待禁用的
    MH_STATUS WINAPI MH_QueueDisableHook(LPVOID pTarget);

    // Applies all queued changes in one go.
	//在一个过程中应用所有队列更改
    MH_STATUS WINAPI MH_ApplyQueued(VOID);

    // Translates the MH_STATUS to its name as a string.
	//将 MH_STATUS 翻译成字符串
    const char * WINAPI MH_StatusToString(MH_STATUS status);

#ifdef __cplusplus
}
#endif

