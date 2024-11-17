#include "XDiskGui.h"
#include "XDiskClient.h"
#include "XThreadPool.h"

#include <iostream>

#include <QtWidgets/QFrame>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QSpinBox>

class XDiskGui::PImpl
{
public:
    PImpl(XDiskGui *owenr);
    ~PImpl() = default;

public:
    void initUI();
    void initConnect();
    void updateServerInfo();

public:
    XDiskGui     *owenr_       = nullptr;
    QPushButton  *refreshBtn_  = nullptr;
    QPushButton  *uploadBtn_   = nullptr;
    QLineEdit    *ipEdit_      = nullptr;
    QLineEdit    *pathEdit_    = nullptr;
    QSpinBox     *portSBox_    = nullptr;
    QTableWidget *tableWidget_ = nullptr;
};

XDiskGui::PImpl::PImpl(XDiskGui *owenr) : owenr_(owenr)
{
    bool ret = XDiskClient::get()->init();
    connect(XDiskClient::get(), &XDiskClient::signalUpdateDir, owenr_, &XDiskGui::slotUpdateDir);
    connect(XDiskClient::get(), &XDiskClient::signalUploadComplete, owenr_, &XDiskGui::slotRefresh);
}

void XDiskGui::PImpl::initUI()
{
    /// TODO 后续如果复杂了话 就考虑封装成类 UITableWidget
    owenr_->setWindowTitle("XDisk");
    auto *mainFrame = new QFrame(owenr_);
    owenr_->setCentralWidget(mainFrame);
    auto *mainLayout = new QHBoxLayout(mainFrame);

    /// 列表展示
    tableWidget_ = new QTableWidget(mainFrame);
    mainLayout->addWidget(tableWidget_);
    tableWidget_->setColumnCount(2);
    QStringList header = { "FileName", "FileSize" };
    tableWidget_->setHorizontalHeaderLabels(header);
    tableWidget_->setMinimumWidth(500);

    /// 操作
    auto *optionFrame = new QFrame(mainFrame);
    mainLayout->addWidget(optionFrame);
    auto *optionLayout = new QVBoxLayout(optionFrame);

    auto *serverFrame = new QFrame(optionFrame);
    optionLayout->addWidget(serverFrame);
    auto *serverLayout = new QVBoxLayout(serverFrame);

    auto *optFrame = new QFrame(optionFrame);
    optionLayout->addWidget(optFrame);
    auto *optLayout = new QVBoxLayout(optFrame);

    auto *pathLayout = new QVBoxLayout;
    pathLayout->setContentsMargins(0, 0, 0, 0);
    auto *pathLabel = new QLabel("Server Path:", optionFrame);
    pathEdit_       = new QLineEdit(optionFrame);
    pathEdit_->setText("./server_root");
    pathLayout->addWidget(pathLabel);
    pathLayout->addWidget(pathEdit_);
    serverLayout->addLayout(pathLayout);

    auto *ipLayout = new QVBoxLayout;
    ipLayout->setContentsMargins(0, 0, 0, 0);
    auto *ipLabel = new QLabel("Server IP:", optionFrame);
    ipEdit_       = new QLineEdit(optionFrame);
    ipEdit_->setText("127.0.0.1");
    ipLayout->addWidget(ipLabel);
    ipLayout->addWidget(ipEdit_);
    serverLayout->addLayout(ipLayout);

    auto *portLayout = new QVBoxLayout;
    portLayout->setContentsMargins(0, 0, 0, 0);
    auto *portLabel = new QLabel("Server Port:", optionFrame);
    portSBox_       = new QSpinBox(optionFrame);
    portSBox_->setRange(0, 65535);
    portSBox_->setValue(8080);
    portLayout->addWidget(portLabel);
    portLayout->addWidget(portSBox_);
    serverLayout->addLayout(portLayout);

    refreshBtn_ = new QPushButton("Refresh", optFrame);
    refreshBtn_->setFixedSize(100, 80);
    uploadBtn_ = new QPushButton("Upload", optFrame);
    uploadBtn_->setFixedSize(100, 80);
    optLayout->addWidget(refreshBtn_);
    optLayout->addWidget(uploadBtn_);

    optionLayout->addStretch();
    mainLayout->setStretch(0, 3);
    mainLayout->setStretch(1, 1);
}

void XDiskGui::PImpl::initConnect()
{
    connect(refreshBtn_, &QPushButton::clicked, owenr_, &XDiskGui::slotRefresh);
    connect(uploadBtn_, &QPushButton::clicked, owenr_, &XDiskGui::slotUpload);
}

void XDiskGui::PImpl::updateServerInfo()
{
    /// 服务器IP
    const std::string ip = ipEdit_->text().toStdString();
    /// 服务器端口
    const int port = portSBox_->value();
    /// 服务器路径
    const std::string path = pathEdit_->text().toStdString();

    XDiskClient::get()->setServerIp(ip);
    XDiskClient::get()->setServerPort(port);
    XDiskClient::get()->setServerRoot(path);

    XDiskClient::get()->getDir();
}


XDiskGui::XDiskGui(QWidget *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags)
{
    impl_ = std::make_shared<PImpl>(this);
    initUI();

    /// 刷新目录
    slotRefresh();
}

XDiskGui::~XDiskGui() = default;

void XDiskGui::initUI()
{
    impl_->initUI();
    impl_->initConnect();
}

void XDiskGui::slotRefresh()
{
    std::cout << "Refresh" << std::endl;
    impl_->updateServerInfo();
}

void XDiskGui::slotUpload()
{
    const auto &filePath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("Please select upload file!"));
    if (filePath.isEmpty())
        return;
    std::cout << "Upload: " << filePath.toStdString() << std::endl;
    XDiskClient::get()->uploadFile(filePath.toStdString());
    impl_->updateServerInfo();
}

void XDiskGui::slotUpdateDir(std::string dirs)
{
    // std::cout << "Dirs: " << dirs << std::endl;
    QString str = QString::fromStdString(dirs);
    str         = str.trimmed();
    if (str.isEmpty())
        return;
    auto fileStr = str.split(";");
    impl_->tableWidget_->setRowCount(fileStr.size());
    for (int i = 0; i < fileStr.size(); ++i)
    {
        auto file = fileStr[i].split(",");
        if (file.size() != 2)
            continue;
        impl_->tableWidget_->setItem(i, 0, new QTableWidgetItem(file[0]));
        impl_->tableWidget_->setItem(i, 1, new QTableWidgetItem(file[1]));
    }
}
