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
    MSG_NONE     = 0, ///<<< ��Ч��Ϣ
    MSG_GETDIR   = 1, ///<<< ����Ŀ¼
    MSG_DIRLIST  = 2, ///<<< ����Ŀ¼�б�
    MSG_MAX_TYPE = 3, ///<<< ������֤������ȷ
};

/// ��Ϣ��󳤶�
#define MSG_MAX_SIZE 1000000

/// ��Ϣͷ
struct XCOM_EXPORT XMsgHead
{
    MsgType type = MSG_NONE;
    int     size = 0;
};

struct XCOM_EXPORT XMsg : public XMsgHead
{
    char* data   = 0; /// �洢��Ϣ����
    int   recved = 0; /// �Ѿ����յ������ݳ���
};

#endif // XMSG_H
