#include "QTouchSettingWidget.h"
#include "ui_QTouchSettingWidget.h"

#include "stdInclude.h"
#include "Common/nv.h"
#include "QT3kUserData.h"

#include <QShowEvent>
#include <QTimer>
#include <QMessageBox>

QTouchSettingWidget::QTouchSettingWidget(QT3kDevice*& pHandle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QTouchSettingWidget), m_pT3kHandle(pHandle)
{
    ui->setupUi(this);

    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );
    setWindowModality( Qt::WindowModal );
    setModal( true );

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    genAdjustButtonWidgetForWinAndX11( this );
#endif

    setFont( parent->font() );

    setFixedSize( width(), height() );

    connect( ui->BtnClose, SIGNAL(clicked()), this, SLOT(accept()) );

    ui->SldTap->setRange( (NV_DEF_TIME_A_RANGE_START)/100, NV_DEF_TIME_A_RANGE_END/100 );
    ui->SldTap->setValue( NV_DEF_TIME_A/100 );
    ui->SldTap->setTickInterval( 1 );
    ui->SldLongTap->setRange( (NV_DEF_TIME_L_RANGE_START)/100, NV_DEF_TIME_L_RANGE_END/100 );
    ui->SldLongTap->setValue( NV_DEF_TIME_L/100 );
    ui->SldLongTap->setTickInterval( 1 );
    ui->SldWheel->setRange( NV_DEF_WHEEL_SENSITIVITY_RANGE_START, NV_DEF_WHEEL_SENSITIVITY_RANGE_END );
    ui->SldWheel->setValue( NV_DEF_WHEEL_SENSITIVITY );
    ui->SldWheel->setTickInterval( NV_DEF_WHEEL_SENSITIVITY_RANGE_END );
    ui->SldZoom->setRange( NV_DEF_ZOOM_SENSITIVITY_RANGE_START, NV_DEF_ZOOM_SENSITIVITY_RANGE_END );
    ui->SldZoom->setValue( NV_DEF_ZOOM_SENSITIVITY );
    ui->SldZoom->setTickInterval( NV_DEF_ZOOM_SENSITIVITY_RANGE_END );

    m_pTimerSendData = new QTimer( this );
    connect( m_pTimerSendData, SIGNAL(timeout()), this, SLOT(OnTimer()) );
    m_pTimerSendData->setSingleShot( true );

    onChangeLanguage();

    ui->BtnClose->setFocus();
}

QTouchSettingWidget::~QTouchSettingWidget()
{
    if( m_pTimerSendData )
    {
        m_pTimerSendData->stop();
        delete m_pTimerSendData;
        m_pTimerSendData = NULL;
    }

    m_RequestSensorData.Stop();

    delete ui;
}

void QTouchSettingWidget::onChangeLanguage()
{
    if( !isWindow() ) return;

    QLangRes& Res = QLangManager::instance()->getResource();

    setWindowTitle( Res.getResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("WINDOW_CAPTION")) );
    ui->LBSlow1->setText( Res.getResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("TEXT_SLOW")) );
    ui->LBSlow2->setText( Res.getResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("TEXT_SLOW")) );
    ui->LBFast1->setText( Res.getResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("TEXT_FAST")) );
    ui->LBFast2->setText( Res.getResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("TEXT_FAST")) );
    ui->BtnDefault->setText( Res.getResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("BTN_CAPTION_DEFAULT")) );
    ui->BtnClose->setText( Res.getResString(QString::fromUtf8("COMMON"), QString::fromUtf8("TEXT_CLOSE")) );

    ui->TitleTimeSetting->setText( Res.getResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("TITLE_TIME_SETTING")) );
    ui->TitleWheelZoomSetting->setText( Res.getResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("TITLE_WHEEL_SENSITIVITY_SETTING")) );

    ui->LBTap->setText( Res.getResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("TEXT_TAP")) );
    ui->LBLongTap->setText( Res.getResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("TEXT_LONG_TAP")) );
    ui->LBWheel->setText( Res.getResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("TEXT_WHEEL")) );
    ui->LBZoom->setText( Res.getResString(QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("TEXT_ZOOM")) );

    ui->TitleTouchSensitivity->setText( Res.getResString(QString::fromUtf8("CALIBRATION SETTING"), QString::fromUtf8("TITLE_TOUCH_SENSITIVITY")) );
    ui->LBSensitivity->setText( Res.getResString(QString::fromUtf8("CALIBRATION SETTING"), QString::fromUtf8("TEXT_SENSITIVITY")) );

    Qt::AlignmentFlag eFlag = Res.isR2L() ? Qt::AlignLeft : Qt::AlignRight;
    ui->EditTap->setAlignment( eFlag );
    ui->EditLongTap->setAlignment( eFlag );
    ui->EditWheel->setAlignment( eFlag );
    ui->EditZoom->setAlignment( eFlag );
}

void QTouchSettingWidget::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart Part, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *cmd)
{
    if( !isVisible() ) return;

    if ( strstr(cmd, cstrTimeA) == cmd )
    {
        m_RequestSensorData.RemoveItem( cstrTimeA );

        int nTime = atoi(cmd + sizeof(cstrTimeA) - 1);
        if( nTime < NV_DEF_TIME_A_RANGE_START )
            nTime = NV_DEF_TIME_A_RANGE_START;
        if( nTime > NV_DEF_TIME_A_RANGE_END )
            nTime = NV_DEF_TIME_A_RANGE_END;

        QString strV( QString("%1").arg(nTime) );
        ui->EditTap->setText( strV );
        ui->SldTap->setValue( NV_DEF_TIME_A_RANGE_END/100 - nTime/100 + (NV_DEF_TIME_A_RANGE_START)/100 );
    }
    else if ( strstr(cmd, cstrTimeL) == cmd )
    {
        m_RequestSensorData.RemoveItem( cstrTimeL );

        int nTime = atoi(cmd + sizeof(cstrTimeL) - 1);
        if( nTime < NV_DEF_TIME_L_RANGE_START )
            nTime = NV_DEF_TIME_L_RANGE_START;
        if( nTime > NV_DEF_TIME_L_RANGE_END )
            nTime = NV_DEF_TIME_L_RANGE_END;

        QString strV( QString("%1").arg(nTime) );
        ui->EditLongTap->setText( strV );
        ui->SldLongTap->setValue( NV_DEF_TIME_L_RANGE_END/100 - nTime/100 + (NV_DEF_TIME_L_RANGE_START)/100 );
    }
    else if ( strstr(cmd, cstrWheelSensitivity) == cmd )
    {
        m_RequestSensorData.RemoveItem( cstrWheelSensitivity );

        int nSens = atoi(cmd + sizeof(cstrWheelSensitivity) - 1);
        if( !(nSens >= NV_DEF_WHEEL_SENSITIVITY_RANGE_START && nSens <= NV_DEF_WHEEL_SENSITIVITY_RANGE_END) )
        {
            nSens = NV_DEF_WHEEL_SENSITIVITY;
        }
        QString strV( QString("%1").arg(nSens) );
        ui->EditWheel->setText( strV );
        ui->SldWheel->setValue( nSens );
    }
    else if ( strstr(cmd, cstrZoomSensitivity) == cmd )
    {
        m_RequestSensorData.RemoveItem( cstrZoomSensitivity );

        int nSens = atoi(cmd + sizeof(cstrZoomSensitivity) - 1);
        if( !(nSens >= NV_DEF_ZOOM_SENSITIVITY_RANGE_START && nSens <= NV_DEF_ZOOM_SENSITIVITY_RANGE_END) )
        {
            nSens = NV_DEF_ZOOM_SENSITIVITY;
        }
        QString strV( QString("%1").arg(nSens) );
        ui->EditZoom->setText( strV );
        ui->SldZoom->setValue( nSens );
    }
    else if( strstr(cmd, cstrDetectionThreshold) == cmd )
    {
        if( Part != MM )
        {
            m_RequestSensorData.RemoveItem( cstrDetectionThreshold, Part );

            int nSensitivity = strtol(cmd + sizeof(cstrDetectionThreshold) - 1, NULL, 10);
            m_mapTouchSensitivity.insert( Part, nSensitivity );

            nSensitivity = NV_DEF_CAM_TRIGGER_THRESHOLD_RANGE_END;
            foreach( int nV, m_mapTouchSensitivity.values() )
            {
                if( nSensitivity > nV )
                    nSensitivity = nV;
            }

            ui->EditSensitivity->setText( QString::number(nSensitivity) );
        }
    }
}

void QTouchSettingWidget::TPDP_OnRSE(T3K_DEVICE_INFO /*devInfo*/, ResponsePart Part, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *cmd)
{
    if( strstr(cmd, cstrNoCam) == cmd )
    {
        m_mapTouchSensitivity.remove( Part );
    }
}

void QTouchSettingWidget::RequestSensorData( bool bDefault )
{
    if( !m_pT3kHandle ) return;

    m_RequestSensorData.Stop();

    char cQ = bDefault ? '*' : '?';

    QString str( cQ );
    m_RequestSensorData.AddItem( cstrTimeA, str );
    m_RequestSensorData.AddItem( cstrTimeL, str );
    m_RequestSensorData.AddItem( cstrWheelSensitivity, str );
    m_RequestSensorData.AddItem( cstrZoomSensitivity, str );

    m_pT3kHandle->sendCommand( QString("%1%2").arg(cstrTimeA).arg(cQ), true );
    m_pT3kHandle->sendCommand( QString("%1%2").arg(cstrTimeL).arg(cQ), true );

    m_pT3kHandle->sendCommand( QString("%1%2").arg(cstrWheelSensitivity).arg(cQ), true );
    m_pT3kHandle->sendCommand( QString("%1%2").arg(cstrZoomSensitivity).arg(cQ), true );

    // threshold
    m_pT3kHandle->sendCommand( QString("%1%2%3").arg(sCam1).arg(cstrDetectionThreshold).arg(cQ), true );
    m_pT3kHandle->sendCommand( QString("%1%2%3").arg(sCam2).arg(cstrDetectionThreshold).arg(cQ), true );

    if( QT3kUserData::GetInstance()->isSubCameraExist() )
    {
        m_pT3kHandle->sendCommand( QString("%1%2%3").arg(sCam1_1).arg(cstrDetectionThreshold).arg(cQ), true );
        m_pT3kHandle->sendCommand( QString("%1%2%3").arg(sCam2_1).arg(cstrDetectionThreshold).arg(cQ), true );
    }

    m_RequestSensorData.Start( m_pT3kHandle );
}

void QTouchSettingWidget::showEvent(QShowEvent *evt)
{
    QDialog::showEvent(evt);

    if( evt->type() == QEvent::Show )
    {
        ui->BtnDefault->setEnabled( false );
        RequestSensorData( false );
        ui->BtnDefault->setEnabled( true );
    }
}

//void QTouchSettingWidget::on_BtnRefresh_clicked()
//{
//    ui->BtnDefault->setEnabled( false );
//    RequestSensorData( false );
//    ui->BtnDefault->setEnabled( true );
//}

void QTouchSettingWidget::on_BtnDefault_clicked()
{
    QLangRes& Res = QLangManager::instance()->getResource();
    QString strMessage = Res.getResString( QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("MSGBOX_TEXT_DEFAULT") );
    QString strMsgTitle = Res.getResString( QString::fromUtf8("TOUCH SETTING"), QString::fromUtf8("MSGBOX_TITLE_DEFAULT") );

    QMessageBox msgBox( this );
    msgBox.setWindowTitle( strMsgTitle );
    msgBox.setText( strMessage );
    msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
    msgBox.setIcon( QMessageBox::Question );
    msgBox.setButtonText( QMessageBox::Yes, Res.getResString( QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_YES") ) );
    msgBox.setButtonText( QMessageBox::No, Res.getResString( QString::fromUtf8("MESSAGEBOX"), QString::fromUtf8("BTN_CAPTION_NO") ) );
    msgBox.setFont( font() );

    if( msgBox.exec() != QMessageBox::Yes ) return;

    ui->BtnDefault->setEnabled( false );
    RequestSensorData( true );
    ui->BtnDefault->setEnabled( true );
}

void QTouchSettingWidget::SendTimeTap( int nTime )
{
    if( !m_pT3kHandle ) return;

    if( nTime < NV_DEF_TIME_A_RANGE_START )
        nTime = NV_DEF_TIME_A_RANGE_START;
    if( nTime > NV_DEF_TIME_A_RANGE_END )
        nTime = NV_DEF_TIME_A_RANGE_END;

    m_pT3kHandle->sendCommand( QString("%1%2").arg(cstrTimeA).arg(nTime), false );
}

void QTouchSettingWidget::SendTimeLongTap( int nTime )
{
    if( !m_pT3kHandle ) return;

    if( nTime < NV_DEF_TIME_L_RANGE_START )
        nTime = NV_DEF_TIME_L_RANGE_START;
    if( nTime > NV_DEF_TIME_L_RANGE_END )
        nTime = NV_DEF_TIME_L_RANGE_END;

    m_pT3kHandle->sendCommand( QString("%1%2").arg(cstrTimeL).arg(nTime), false );
}

void QTouchSettingWidget::SendSensWheel( int nSensitivity )
{
    if( !m_pT3kHandle ) return;

    if( !(nSensitivity >= NV_DEF_WHEEL_SENSITIVITY_RANGE_START && nSensitivity <= NV_DEF_WHEEL_SENSITIVITY_RANGE_END) )
    {
        nSensitivity = NV_DEF_WHEEL_SENSITIVITY;
    }

    m_pT3kHandle->sendCommand( QString("%1%2").arg(cstrWheelSensitivity).arg(nSensitivity), false );
}

void QTouchSettingWidget::SendSensZoom( int nSensitivity )
{
    if( !m_pT3kHandle ) return;

    if( !(nSensitivity >= NV_DEF_ZOOM_SENSITIVITY_RANGE_START && nSensitivity <= NV_DEF_ZOOM_SENSITIVITY_RANGE_END) )
    {
        nSensitivity = NV_DEF_ZOOM_SENSITIVITY;
    }

    m_pT3kHandle->sendCommand( QString("%1%2").arg(cstrZoomSensitivity).arg(nSensitivity), false );
}

void QTouchSettingWidget::on_SldTap_valueChanged(int /*value*/)
{
    if( !ui->SldTap->isVisible() ) return;

    int nMinPos = ui->SldTap->minimum();
    int nMaxPos = ui->SldTap->maximum();
    int nCurPos = ui->SldTap->value();

    if( m_pTimerSendData->isActive() )
        m_pTimerSendData->stop();

    QString strV( QString("%1").arg((nMaxPos-(nCurPos-nMinPos))*100) );
    ui->EditTap->setText( strV );

    m_pTimerSendData->start( 500 );
}

void QTouchSettingWidget::on_SldLongTap_valueChanged(int /*value*/)
{
    if( !ui->SldLongTap->isVisible() ) return;

    int nMinPos = ui->SldLongTap->minimum();
    int nMaxPos = ui->SldLongTap->maximum();
    int nCurPos = ui->SldLongTap->value();

    if( m_pTimerSendData->isActive() )
        m_pTimerSendData->stop();

    QString strV( QString("%1").arg((nMaxPos-(nCurPos-nMinPos))*100) );
    ui->EditLongTap->setText( strV );

    m_pTimerSendData->start( 500 );
}

void QTouchSettingWidget::on_SldWheel_valueChanged(int /*value*/)
{
    if( !ui->SldWheel->isVisible() ) return;

//    int nMinPos = ui->SldWheel->minimum();
//    int nMaxPos = ui->SldWheel->maximum();
    int nCurPos = ui->SldWheel->value();

    if( m_pTimerSendData->isActive() )
        m_pTimerSendData->stop();

    QString strV( QString("%1").arg(nCurPos) );
    ui->EditWheel->setText( strV );


    m_pTimerSendData->start( 500 );
}

void QTouchSettingWidget::on_SldZoom_valueChanged(int /*value*/)
{
    if( !ui->SldZoom->isVisible() ) return;

//    int nMinPos = ui->SldZoom->minimum();
//    int nMaxPos = ui->SldZoom->maximum();
    int nCurPos = ui->SldZoom->value();

    if( m_pTimerSendData->isActive() )
        m_pTimerSendData->stop();

    QString strV( QString("%1").arg(nCurPos) );
    ui->EditZoom->setText( strV );

    m_pTimerSendData->start( 500 );
}

void QTouchSettingWidget::on_BtnSensitivityDec_clicked()
{
    if( !m_pT3kHandle ) return;

    QString strV( ui->EditSensitivity->text() );
    int nV = (float)strtod( strV.toUtf8().data(), NULL );
    nV -= 5;

    if( nV < NV_DEF_CAM_TRIGGER_THRESHOLD_RANGE_START || nV > NV_DEF_CAM_TRIGGER_THRESHOLD_RANGE_END )
        nV = NV_DEF_CAM_TRIGGER_THRESHOLD_RANGE_START;

    m_pT3kHandle->sendCommand( strV.sprintf( "%s%s%d", sCam1.toUtf8().data(), cstrDetectionThreshold, nV ), true );
    m_pT3kHandle->sendCommand( strV.sprintf( "%s%s%d", sCam2.toUtf8().data(), cstrDetectionThreshold, nV ), true );

    if( QT3kUserData::GetInstance()->isSubCameraExist() )
    {
        m_pT3kHandle->sendCommand( strV.sprintf( "%s%s%d", sCam1_1.toUtf8().data(), cstrDetectionThreshold, nV ), true );
        m_pT3kHandle->sendCommand( strV.sprintf( "%s%s%d", sCam2_1.toUtf8().data(), cstrDetectionThreshold, nV ), true );
    }
}

void QTouchSettingWidget::on_BtnSensitivityInc_clicked()
{
    if( !m_pT3kHandle ) return;

    QString strV( ui->EditSensitivity->text() );
    int nV = (float)strtod( strV.toUtf8().data(), NULL );
    nV += 5;

    if( nV < NV_DEF_CAM_TRIGGER_THRESHOLD_RANGE_START || nV > NV_DEF_CAM_TRIGGER_THRESHOLD_RANGE_END )
        nV = NV_DEF_CAM_TRIGGER_THRESHOLD_RANGE_END;

    m_pT3kHandle->sendCommand( strV.sprintf( "%s%s%d", sCam1.toUtf8().data(), cstrDetectionThreshold, nV ), true );
    m_pT3kHandle->sendCommand( strV.sprintf( "%s%s%d", sCam2.toUtf8().data(), cstrDetectionThreshold, nV ), true );

    if( QT3kUserData::GetInstance()->isSubCameraExist() )
    {
        m_pT3kHandle->sendCommand( strV.sprintf( "%s%s%d", sCam1_1.toUtf8().data(), cstrDetectionThreshold, nV ), true );
        m_pT3kHandle->sendCommand( strV.sprintf( "%s%s%d", sCam2_1.toUtf8().data(), cstrDetectionThreshold, nV ), true );
    }
}

void QTouchSettingWidget::OnTimer()
{
    int nSens, nTime;
    QString strV( ui->EditWheel->text() );
    nSens = (int)strtol( strV.toStdString().c_str(), NULL, 10 );
    SendSensWheel( nSens );
    strV = QString(ui->EditZoom->text() );
    nSens = (int)strtol( strV.toStdString().c_str(), NULL, 10 );
    SendSensZoom( nSens );
    strV = QString(ui->EditTap->text() );
    nTime = (int)strtol( strV.toStdString().c_str(), NULL, 10 );
    SendTimeTap( nTime );
    strV = QString(ui->EditLongTap->text() );
    nTime = (int)strtol( strV.toStdString().c_str(), NULL, 10 );
    SendTimeLongTap( nTime );
}
