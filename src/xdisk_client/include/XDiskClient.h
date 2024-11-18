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

#include <QObject>

#include <memory>
#include <string>

class XDiskClient : public QObject
{
    Q_OBJECT
public:
    static XDiskClient* get()
    {
        static XDiskClient instance;
        return &instance;
    }

public:
    /// \brief 获取目录下的文件列表， 只是请求消息到服务器
    void getDir();

    /// \brief 上传文件
    /// \param filePath
    void uploadFile(const std::string& filePath);

    /// \brief 下载文件
    /// \param serverPath
    /// \param localPath
    void downloadFile(const std::string& serverPath, const std::string& localPath);

    void setServerIp(const std::string ip);

    void setServerPort(int port);

    void setServerRoot(const std::string path);

    bool init();
signals:
    void signalUpdateDir(std::string dirs);

    void signalUploadComplete();

    void signalDownloadComplete();

private:
    XDiskClient();
    virtual ~XDiskClient();

private:
    class PImpl;
    std::shared_ptr<PImpl> impl_;
};

#endif // XDISKCLIENT_H
