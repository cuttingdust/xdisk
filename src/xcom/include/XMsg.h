/**
 * @file   XMsg.h
 * @brief  
 *
 * Detailed description if necessary.
 *
 * @author 31667
 * @date   2024-11-14
 */

#ifndef XMSG_H
#define XMSG_H

#include "XCom_Global.h"

enum MsgType
{
    MSG_NONE     = 0, ///<<< 无效消息
    MSG_GETDIR   = 1, ///<<< 请求目录
    MSG_DIRLIST  = 2, ///<<< 返回目录列表
    MSG_MAX_TYPE = 3, ///<<< 用来验证类型正确
};

/// 消息最大长度
#define MSG_MAX_SIZE 1000000

/// 消息头
struct XCOM_EXPORT XMsgHead
{
    MsgType type = MSG_NONE;
    int     size = 0;
};

struct XCOM_EXPORT XMsg : public XMsgHead
{
    char* data   = 0; /// 存储消息内容
    int   recved = 0; /// 已经接收到的数据长度
};

#endif // XMSG_H
