/**
 * @file   XDiskGui.h
 * @brief  
 *
 * Detailed description if necessary.
 *
 * @author 31667
 * @date   2024-11-13
 */

#ifndef XDISKGUI_H
#define XDISKGUI_H

#include <QtWidgets/QMainWindow>

#include <memory>

class XDiskGui : public QMainWindow
{
public:
    explicit XDiskGui(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    ~XDiskGui() override;

public:
    void initUI();
protected slots:
    void slotRefresh();
    void slotUpload();
    void slotUpdateDir(std::string dirs);

private:
    class PImpl;
    std::shared_ptr<PImpl> impl_;
};

#endif // XDISKGUI_H
