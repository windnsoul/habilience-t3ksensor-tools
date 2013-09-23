#ifndef QMOUSESETTINGWIDGET_H
#define QMOUSESETTINGWIDGET_H

#include <QWidget>

#include "QProfileLabel.h"
#include "TPDPEventMultiCaster.h"
#include "QMouseMappingTable.h"
#include "QTouchSettingWidget.h"

#include "QCheckableButton.h"
#include "QLangManager.h"
#include "QRequestHIDManager.h"

namespace Ui {
    class QMouseSettingWidget;
}

class QMouseSettingWidget : public QWidget, public TPDPEventMultiCaster::ITPDPEventListener, public QLangManager::ILangChangeNotify
{
    Q_OBJECT

public:
    explicit QMouseSettingWidget(T3kHandle*& pHandle, QWidget *parent = 0);
    ~QMouseSettingWidget();

    void SetDefault();
    void Refresh();

    void ReplaceLabelName(QCheckableButton* pBtn);

    void RemoveChildRequest( const char* sCmd, QRequestHIDManager::eRequestPart ePart = QRequestHIDManager::MM );

protected:
    void RequestSensorData( bool bDefault );

    void ChangeProfile( QCheckableButton* pBtn, int nIndex );

    virtual void onChangeLanguage();

    virtual void showEvent(QShowEvent *evt);
    virtual void hideEvent(QHideEvent *evt);
    virtual void keyPressEvent(QKeyEvent *evt);

    virtual void OnRSP(ResponsePart Part, ushort nTickTime, const char *sPartId, long lId, bool bFinal, const char *sCmd);

protected:
    QProfileLabel           m_ProfileLabel;

    bool                    m_bSetDefault;
    int                     m_nMouseProfileIndex;
    int                     m_nSelectedProfileIndex;

    int                     m_nProfileIndexData;

    int                     m_nCurInputMode;

    QRequestHIDManager      m_RequestSensorData;

signals:
    void ByPassKeyPressEvent(QKeyEvent *evt);
    void SendInputModeState(int nCurInputMode);

private:
    Ui::QMouseSettingWidget *ui;
    T3kHandle*&            m_pT3kHandle;

private slots:
    void on_Profile5_clicked();
    void on_Profile4_clicked();
    void on_Profile3_clicked();
    void on_Profile2_clicked();
    void on_Profile1_clicked();
};

#endif // QMOUSESETTINGWIDGET_H
