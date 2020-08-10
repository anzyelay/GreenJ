/****************************************************************************
**
** Copyright (C) 2012 Lorem Ipsum Mediengesellschaft m.b.H.
**
** GNU General Public License
** This file may be used under the terms of the GNU General Public License
** version 3 as published by the Free Software Foundation and
** appearing in the file LICENSE.GPL included in the packaging of this file.
**
****************************************************************************/

#include <QApplication>
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QWebChannel>
#include "Config.h"
#include "LogHandler.h"
#include "phone/Phone.h"
#include "WebPage.h"
#include "Gui.h"

//-----------------------------------------------------------------------------
Gui::Gui(phone::Phone &phone, QWidget *parent, Qt::WindowFlags flags) :
    QMainWindow(parent, flags),
    phone_(phone),
    print_handler_(*this)
{
    qRegisterMetaType<LogInfo>("LogInfo");
    ui_.setupUi(this);

    js_handler_ = new JavascriptHandler(ui_.webview, phone_);
    phone_.setJavascriptHandler(js_handler_);

    ui_.webview->setPage(new WebPage());
    //ui_.webview->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    ui_.webview->settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
//    ui_.webview->settings()->setLocalStoragePath(QDir::homePath() + "/.greenj/");


#ifdef DEBUG
    // Enable webkit Debugger
#else
    // Deactivate right-click context menu
    ui_.webview->setContextMenuPolicy(Qt::NoContextMenu);
#endif

    connect(ui_.webview->page(), SIGNAL(loadStarted()),
            this,                             SLOT(slotCreateJavascriptWindowObject()));
    connect(ui_.webview,                      SIGNAL(urlChanged(QUrl)),
            this,                             SLOT(slotLinkClicked(const QUrl&)));

    connect(&LogHandler::getInstance(), SIGNAL(signalLogMessage(const LogInfo&)),
            js_handler_,                SLOT(slotLogMessage(const LogInfo&)));
    connect(js_handler_, SIGNAL(signalWebPageChanged()),
            this,        SLOT(slotUpdateWebPage()));
    connect(js_handler_, SIGNAL(signalPrintPage(const QUrl&)),
            this,        SLOT(slotPrintPage(const QUrl&)));

    connect(&phone_, SIGNAL(signalIncomingCall(QString,QVariantMap)),
            this,    SLOT(slotIncomingCall(const QString&)));

    QUrl url(Config::getInstance().getBrowserUrl());
    if (url.isRelative()) {
        QFileInfo fileinfo = QFileInfo(url.toString());
        if (fileinfo.exists()) {
            if (fileinfo.isRelative()) {
                url = QUrl::fromLocalFile(fileinfo.absoluteFilePath());
            } else {
                url = QUrl::fromLocalFile(fileinfo.fileName());
            }
        }
    }
    if (!url.isEmpty() && url.isValid()) {
        ui_.webview->setUrl(url);
    }

    createSystemTray();
    createShortcuts();

    readSettings();
}

//-----------------------------------------------------------------------------
Gui::~Gui()
{
    delete js_handler_;
}

//-----------------------------------------------------------------------------
void Gui::closeEvent(QCloseEvent *event)
{
    phone_.unregister();

    writeSettings();

    event->accept();
}

//-----------------------------------------------------------------------------
Ui::MainWindow &Gui::getWindow()
{
    return ui_;
}

//-----------------------------------------------------------------------------
void Gui::slotLinkClicked(const QUrl &url)
{
    ui_.webview->load(url);
}

//-----------------------------------------------------------------------------
void Gui::slotToggleFullScreen()
{
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}

//-----------------------------------------------------------------------------
void Gui::slotPrintKeyPressed()
{
    QUrl url = js_handler_->getPrintUrl();
    if (url != QUrl("about:blank")) {
        print_handler_.loadPrintPage(url);
    }
}

//-----------------------------------------------------------------------------
void Gui::slotPrintPage(const QUrl &url)
{
    print_handler_.loadPrintPage(url);
}

//-----------------------------------------------------------------------------
void Gui::slotCreateJavascriptWindowObject()
{
    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject(JavascriptHandler::OBJECT_NAME, js_handler_);
    ui_.webview->page()->setWebChannel(channel);
}

//-----------------------------------------------------------------------------
void Gui::slotIncomingCall(const QString &url)
{
    QApplication::alert(this);
    if (!QApplication::focusWidget()) {
        system_tray_icon_->showMessage("GreenJ", "Incoming call from " + url);
    }
}

//-----------------------------------------------------------------------------
void Gui::slotUpdateWebPage()
{
    const QUrl &url = Config::getInstance().getBrowserUrl();
    if (!url.isEmpty()) {
        ui_.webview->setUrl(url);
    }
}

//-----------------------------------------------------------------------------
void Gui::createSystemTray()
{
    QAction *minimize_action =   new QAction(tr("Mi&nimize"), this);
    QAction *maximize_action =   new QAction(tr("Ma&ximize"), this);
    QAction *restore_action =    new QAction(tr("&Restore"), this);
    QAction *fullscreen_action = new QAction(tr("&FullScreen"), this);
    QAction *quit_action =       new QAction(tr("&Quit"), this);

    connect(minimize_action,   SIGNAL(triggered()), this, SLOT(hide()));
    connect(maximize_action,   SIGNAL(triggered()), this, SLOT(showMaximized()));
    connect(restore_action,    SIGNAL(triggered()), this, SLOT(showNormal()));
    connect(fullscreen_action, SIGNAL(triggered()), this, SLOT(slotToggleFullScreen()));
    connect(quit_action,       SIGNAL(triggered()), qApp, SLOT(quit()));

    QMenu *tray_menu = new QMenu(this);
    tray_menu->addAction(minimize_action);
    tray_menu->addAction(maximize_action);
    tray_menu->addAction(restore_action);
    tray_menu->addAction(fullscreen_action);
    tray_menu->addSeparator();
    tray_menu->addAction(quit_action);

    system_tray_icon_ = new QSystemTrayIcon(this);
    system_tray_icon_->setContextMenu(tray_menu);
    system_tray_icon_->setIcon(QIcon(":images/icon.xpm"));
    system_tray_icon_->show();
}

//-----------------------------------------------------------------------------
void Gui::createShortcuts()
{
    new QShortcut(Qt::Key_F11,          this, SLOT(slotToggleFullScreen()));
    new QShortcut(Qt::CTRL + Qt::Key_P, this, SLOT(slotPrintKeyPressed()));
}

//-----------------------------------------------------------------------------
void Gui::readSettings()
{
    Config &config = Config::getInstance();
    QSettings settings(config.getApplicationDeveloper(), config.getApplicationName());

    settings.beginGroup("GuiMainWindow");
    resize(settings.value("size", QSize(800, 600)).toSize());
    move(settings.value("pos", QPoint(100, 50)).toPoint());
    setWindowState((Qt::WindowStates)settings.value("state", QVariant(Qt::WindowMaximized)).toInt());
    settings.endGroup();

    setMinimumSize(config.getWindowMinimumWidth(), config.getWindowMinimumHeight());
    setWindowTitle(config.getApplicationName());
}

//-----------------------------------------------------------------------------
void Gui::writeSettings() const
{
    Config &config = Config::getInstance();
    QSettings settings(config.getApplicationDeveloper(), config.getApplicationName());
    
    settings.beginGroup("GuiMainWindow");
    if (!isFullScreen()) {
        settings.setValue("size", size());
        settings.setValue("pos", pos());
    }
    settings.setValue("state", QVariant(windowState()));
    settings.endGroup();
}
