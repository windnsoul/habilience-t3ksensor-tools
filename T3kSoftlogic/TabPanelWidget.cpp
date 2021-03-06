#include "TabPanelWidget.h"
#include "ui_TabPanelWidget.h"

#include "T3kConstStr.h"

#include "T3kCommonData.h"
#include "T3kSoftlogicDlg.h"

#include <QMessageBox>


TabPanelWidget::TabPanelWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::TabPanelWidget)
{
    ui->setupUi(this);
    setFont( parent->font() );

    m_bLoadFromSensor = false;

    ui->BtnLoad->setEnabled( false );

    updateUIFromData();
}

TabPanelWidget::~TabPanelWidget()
{
    delete ui;
}

void TabPanelWidget::updateUIFromData()
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    ui->EditInfoName->setText( Keys.getPanelName() );
    double dSX = 0.0, dSY = 0.0;
    Keys.getScreenDimension( dSX, dSY );
    ui->EditScreenWidth->setText( QString("%1").arg(dSX) );
    ui->EditScreenHeight->setText( QString("%1").arg(dSY) );

    ui->LBPath->setText( T3kCommonData::instance()->getLoadedModelPathName() );
}

void TabPanelWidget::updateDataFromUI()
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

    Keys.setPanelName( ui->EditInfoName->text() );
    Keys.setScreenDimension( ui->EditScreenWidth->text().toDouble(), ui->EditScreenHeight->text().toDouble() );
    T3kCommonData::instance()->setLoadedModelPathName( ui->LBPath->text() );
}

void TabPanelWidget::setFocusPaname()
{
    ui->EditInfoName->setFocus();
}

void TabPanelWidget::onConnectedT3kDevice()
{
    if( !isVisible() ) return;

    ui->BtnLoad->setEnabled( true );
}

void TabPanelWidget::TPDP_OnDisconnected(T3K_DEVICE_INFO /*devInfo*/)
{
    if( !isVisible() ) return;

    ui->BtnLoad->setEnabled( false );
}

void TabPanelWidget::TPDP_OnRSP(T3K_DEVICE_INFO /*devInfo*/, ResponsePart /*Part*/, unsigned short /*ticktime*/, const char */*partid*/, int /*id*/, bool /*bFinal*/, const char *cmd)
{
    if( !isVisible() ) return;

    const char * buf;

    if ( strstr(cmd, cstrFactorialSoftkey) == cmd )
    {
        buf = cmd + sizeof(cstrFactorialSoftkey) - 1;

        CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

        if( m_bLoadFromSensor )
        {
            m_bLoadFromSensor = false;
            QString strData( buf );
            Keys.load(strData, "", NULL);

            T3kCommonData::instance()->getGroupKeys().clear();

            emit updatePreview();
        }
    }
    else if ( strstr(cmd, cstrFactorialSoftkeyBind) == cmd )
    {
        buf = cmd + sizeof(cstrFactorialSoftkeyBind) - 1;

        CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

        if( m_bLoadFromSensor )
        {
            m_bLoadFromSensor = false;
            QString strData( buf );

            Keys.loadBindInfo( strData );

            emit updatePreview();
        }
    }
    else if ( strstr(cmd, cstrFactorialSoftlogic) == cmd )
    {
        buf = cmd + sizeof(cstrFactorialSoftlogic) - 1;

        CSoftlogicArray& Logics = T3kCommonData::instance()->getLogics();

        if( m_bLoadFromSensor )
        {
            m_bLoadFromSensor = false;
            QString strData( buf );
            Logics.load(strData, NULL);
        }
    }
    else if ( strstr(cmd, cstrFactorialGPIO) == cmd )
    {
        buf = cmd + sizeof(cstrFactorialGPIO) - 1;

        CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

        if ( m_bLoadFromSensor )
        {
            m_bLoadFromSensor = false;
            QString strData( buf );
            Keys.loadGPIOInfo( strData );
        }
    }
}

void TabPanelWidget::showEvent(QShowEvent *)
{
    if( isValidT3kSensorState() )// pDlg->isConnected() && !pDlg->isInvalidFirmware() && !pDlg->isFirmwareDownload() )
        ui->BtnLoad->setEnabled( true );
    else
        ui->BtnLoad->setEnabled( false );
}

void TabPanelWidget::on_EditInfoName_editingFinished()
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();
    Keys.setPanelName( ui->EditInfoName->text() );
}

void TabPanelWidget::on_EditScreenWidth_editingFinished()
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();
    Keys.setScreenDimension( ui->EditScreenWidth->text().toDouble(), ui->EditScreenHeight->text().toDouble() );

    ui->EditScreenWidth->setText( QString("%1").arg(ui->EditScreenWidth->text().toDouble(), 0, 'f', 1 ) );
}

void TabPanelWidget::on_EditScreenHeight_editingFinished()
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();
    Keys.setScreenDimension( ui->EditScreenWidth->text().toDouble(), ui->EditScreenHeight->text().toDouble() );

    ui->EditScreenHeight->setText( QString("%1").arg(ui->EditScreenHeight->text().toDouble(), 0, 'f', 1 ) );
}

void TabPanelWidget::on_EditInfoName_textChanged(const QString&)
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();
    if( Keys.getPanelName() ==  ui->EditInfoName->text() ) return;


}

void TabPanelWidget::on_EditScreenWidth_textChanged(const QString&)
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();
    if( Keys.getScreenDimensionW()  ==  ui->EditScreenWidth->text().toDouble() ) return;


}

void TabPanelWidget::on_EditScreenHeight_textChanged(const QString&)
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();
    if( Keys.getScreenDimensionH() ==  ui->EditScreenHeight->text().toDouble() ) return;

    //ui->EditScreenHeight->setStyleSheet();
}

void TabPanelWidget::on_BtnLoad_clicked()
{
    QMessageBox msg( QMessageBox::Question, "Warning", "Existing data will be overwritten by the data from the sensor. Continue?", QMessageBox::Yes|QMessageBox::No, this );
    if( msg.exec() == QMessageBox::No )
        return;

    QT3kDevice* pT3kHandle = getT3kHandle();

    char szCmd[256];
    if( pT3kHandle->isOpen() )
    {
        m_bLoadFromSensor = true;
        sprintf( szCmd, "%s?", cstrFactorialSoftkey );
        if( !pT3kHandle->sendCommand( szCmd, false ) )
        {
            QMessageBox::critical( this, "Error", "Cannot retrieve the Sofkey data from the sensor.", QMessageBox::Ok );
            m_bLoadFromSensor = false;
            return;
        }
        emit notifyTab( 1 );

        m_bLoadFromSensor = true;
        sprintf( szCmd, "%s?", cstrFactorialSoftkeyBind );
        if( !pT3kHandle->sendCommand( szCmd, false ) )
        {
            m_bLoadFromSensor = false;
        }

        m_bLoadFromSensor = true;
        sprintf( szCmd, "%s?", cstrFactorialSoftlogic );
        if( !pT3kHandle->sendCommand( szCmd, false ) )
        {
            QMessageBox::critical( this, "Error", "Cannot retrieve the Softlogic data from the sensor.", QMessageBox::Ok );
            m_bLoadFromSensor = false;
            return;
        }

        m_bLoadFromSensor = true;
        sprintf( szCmd, "%s?", cstrFactorialGPIO );
        if ( !pT3kHandle->sendCommand( szCmd, false ) )
        {
            m_bLoadFromSensor = false;
            return;
        }

        emit notifyTab( 2 );
    }

    ui->EditInfoName->setText("");
    T3kCommonData::instance()->setLoadedModelPathName("");
    T3kCommonData::instance()->getKeys().setPanelName("");

    updateUIFromData();
}
