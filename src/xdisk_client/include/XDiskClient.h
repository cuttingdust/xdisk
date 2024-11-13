/**
 * @file   XDiskClient.h
 * @brief  
 *
 * Detailed description if necessary.
 *
 * @author 31667
 * @date   2024-11-13
 */

#ifndef XDISKCLIENT_H
#define XDISKCLIENT_H

#include <memory>
#include <string>

class XDiskClient
{
public:
    static XDiskClient* get()
    {
        static XDiskClient instance;
        return &instance;
    }

public:
    /// \brief ��ȡĿ¼�µ��ļ��б� ֻ��������Ϣ��������
    void getDir();

    void setServerIp(const std::string ip);
    void setServerPort(int port);
    void setServerRoot(const std::string path);
    bool init();

private:
    XDiskClient();
    virtual ~XDiskClient();

private:
    class PImpl;
    std::shared_ptr<PImpl> impl_;
};

#endif // XDISKCLIENT_H
