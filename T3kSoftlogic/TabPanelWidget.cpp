#include "TabPanelWidget.h"
#include "ui_TabPanelWidget.h"

#include "../common/T3kConstStr.h"

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

void TabPanelWidget::OnOpenT3kDevice(T3K_HANDLE /*hDevice*/)
{
    if( !isVisible() ) return;

    ui->BtnLoad->setEnabled( true );
}

void TabPanelWidget::OnCloseT3kDevice(T3K_HANDLE /*hDevice*/)
{
    if( !isVisible() ) return;

    ui->BtnLoad->setEnabled( false );
}

void TabPanelWidget::OnRSP(ResponsePart, ushort, const char *, long, bool, const char *szCmd)
{
    if( !isVisible() ) return;

    const char * buf;

    if ( strstr(szCmd, cstrFactorialSoftkey) == szCmd )
    {
        buf = szCmd + sizeof(cstrFactorialSoftkey) - 1;

        CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

        if( m_bLoadFromSensor )
        {
            m_bLoadFromSensor = false;
            QString strData( buf );
            Keys.load(strData, "", NULL);

            T3kCommonData::instance()->getGroupKeys().clear();
            ((T3kSoftlogicDlg*)parent())->updatePreviewCanvas();
        }
    }
    else if ( strstr(szCmd, cstrFactorialSoftkeyBind) == szCmd )
    {
        buf = szCmd + sizeof(cstrFactorialSoftkeyBind) - 1;

        CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();

        if( m_bLoadFromSensor )
        {
            m_bLoadFromSensor = false;
            QString strData( buf );

            Keys.loadBindInfo( strData );

            ((T3kSoftlogicDlg*)parent())->updatePreviewCanvas();
        }
    }
    else if ( strstr(szCmd, cstrFactorialSoftlogic) == szCmd )
    {
        buf = szCmd + sizeof(cstrFactorialSoftlogic) - 1;

        CSoftlogicArray& Logics = T3kCommonData::instance()->getLogics();

        if( m_bLoadFromSensor )
        {
            m_bLoadFromSensor = false;
            QString strData( buf );
            Logics.load(strData, NULL);
        }
    }
    else if ( strstr(szCmd, cstrFactorialGPIO) == szCmd )
    {
        buf = szCmd + sizeof(cstrFactorialGPIO) - 1;

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
    T3kSoftlogicDlg* pDlg = (T3kSoftlogicDlg*)findWantToParent( parent(), "T3kSoftlogicDlg" );
    if( pDlg->isConnected() && !pDlg->isInvalidFirmware() && !pDlg->isFirmwareDownload() )
        ui->BtnLoad->setEnabled( true );
    else
        ui->BtnLoad->setEnabled( false );
}

QObject* TabPanelWidget::findWantToParent(QObject *target, const char* strObjectName)
{
    Q_ASSERT( target );
    QObject* p = target;
    while( p )
    {
        if( p->inherits( strObjectName ) )
            break;

        p = p->parent();
    }

    Q_ASSERT( p );

    return p;
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
}

void TabPanelWidget::on_EditScreenHeight_editingFinished()
{
    CSoftkeyArray& Keys = T3kCommonData::instance()->getKeys();
    Keys.setScreenDimension( ui->EditScreenWidth->text().toDouble(), ui->EditScreenHeight->text().toDouble() );
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

    T3kSoftlogicDlg* pDlg = (T3kSoftlogicDlg*)findWantToParent( parent(), "T3kSoftlogicDlg" );
    //CGdipTabWnd* pTabWnd = (CGdipTabWnd*)GetParent();
    T3kHandle* pT3kHandle = pDlg->getT3kHandle();

    char szCmd[256];
    if( pT3kHandle->IsOpen() )
    {
        m_bLoadFromSensor = true;
        sprintf( szCmd, "%s?", cstrFactorialSoftkey );
        if( !pT3kHandle->SendCommand( szCmd, false ) )
        {
            QMessageBox::critical( this, "Error", "Cannot retrieve the Sofkey data from the sensor.", QMessageBox::Ok );
            m_bLoadFromSensor = false;
            return;
        }
        pDlg->notifyTab( 1 );

        m_bLoadFromSensor = true;
        sprintf( szCmd, "%s?", cstrFactorialSoftkeyBind );
        if( !pT3kHandle->SendCommand( szCmd, false ) )
        {
            m_bLoadFromSensor = false;
        }

        m_bLoadFromSensor = true;
        sprintf( szCmd, "%s?", cstrFactorialSoftlogic );
        if( !pT3kHandle->SendCommand( szCmd, false ) )
        {
            QMessageBox::critical( this, "Error", "Cannot retrieve the Softlogic data from the sensor.", QMessageBox::Ok );
            m_bLoadFromSensor = false;
            return;
        }

        m_bLoadFromSensor = true;
        sprintf( szCmd, "%s?", cstrFactorialGPIO );
        if ( !pT3kHandle->SendCommand( szCmd, false ) )
        {
            m_bLoadFromSensor = false;
            return;
        }

        pDlg->notifyTab( 2 );
    }

    ui->EditInfoName->setText("");
    T3kCommonData::instance()->setLoadedModelPathName("");
    T3kCommonData::instance()->getKeys().setPanelName("");

    updateUIFromData();
}