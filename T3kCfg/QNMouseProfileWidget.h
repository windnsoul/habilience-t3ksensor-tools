#ifndef QNMOUSEPROFILEWIDGET_H
#define QNMOUSEPROFILEWIDGET_H

#include <QWidget>

#include "QT3kDeviceEventHandler.h"
#include "ui/QGestureMappingTable.h"
#include "ui/QEditActionWnd.h"
#include "ui/QEditActionEDWnd.h"
#include "ui/QEditAction2WDWnd.h"
#include "ui/QEditAction4WDWnd.h"
#include "QLangManager.h"

#include "QRequestHIDManager.h"

#include <QKeyEvent>
#include <QComboBox>

namespace Ui {
class QNMouseProfileWidget;
}


class QColorTabWidget;
class QNMouseProfileWidget : public QWidget, public QT3kDeviceEventHandler::IListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit QNMouseProfileWidget(QT3kDevice*& pT3kHandle, QWidget *parent = 0);
    ~QNMouseProfileWidget();

    void setDefault();
    void refresh();

protected:
    // QT3kDeviceEventHandler::IListener
    virtual void TPDP_OnRSP(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);
    virtual void TPDP_OnRSE(T3K_DEVICE_INFO devInfo, ResponsePart Part, unsigned short ticktime, const char *partid, int id, bool bFinal, const char *cmd);

    // QLangManager::ILangChangeNotify
    virtual void onChangeLanguage();

    // QWidget
    virtual void showEvent(QShowEvent *);
    virtual void hideEvent(QHideEvent *);
    virtual void keyPressEvent(QKeyEvent *);

    void requestSensorData( bool bDefault );
    void sensorRefresh( bool bTabOnly/*=false*/ );
    void loadPredefProfiles(int nTabIndex);

protected:
    QGestureMappingTable        m_MouseProfileTableWidget;
    QEditActionWnd*             m_pEditActionWnd;
    QEditActionEDWnd*           m_pEditActionEWnd;
    QEditAction2WDWnd*          m_pEditAction2WDWnd;
    QEditAction4WDWnd*          m_pEditAction4WDWnd;

    QRequestHIDManager          m_RequestCmdManager;

    int                         m_nInputMode;
    int                         m_nChkUsbCfgMode;

    QComboBox                   m_cbPredefinedProfile;
    bool                        m_bDefault;
    QString                     m_strPrevZommValue;
    bool                        m_bCheckMacOSXZoom;

private:
    Ui::QNMouseProfileWidget *ui;
    QT3kDevice*&               m_pT3kHandle;

signals:
    void ByPassKeyPressEvent(QKeyEvent *evt);
    void SendInputModeState();

protected slots:
    void onTabSelChanged(QColorTabWidget* /*pTabWidget*/, int tabIndex);
    void onSendCommand(QString strCmd, bool bAsync = false, unsigned short nTimeout = 1000);
    void onUpdateProfile(int nProfileIndex, const QGestureMappingTable::CellInfo& ci, ushort nProfileFlags);
    void onCBPredefinedProfileActivated(int index);
    void onModifiedProfile();

public slots:
    void onEnableMacOSXGesture(bool bEnable);
};

#endif // QNMOUSEPROFILEWIDGET_H
