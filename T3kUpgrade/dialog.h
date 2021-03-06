#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QFWDPacket.h>
#include <QList>

#include "../common/T3kCamNameDef.h"

namespace Ui {
class Dialog;
}

struct SensorInfo {
    unsigned short  nMode;
    unsigned short  nModelNumber;
    unsigned short  nIapRevision;
    unsigned short  nVersionMajor;
    unsigned short  nVersionMinor;
    unsigned short  nWhich;
    char            szVersion[256];
    char            szDateTime[256];
    char            szModel[256];
    bool            bUpgradeTarget;
};

enum eFIRMWARE_TYPE { TYPE_MM=0, TYPE_CM=1 };
struct FirmwareInfo {
    char*           pFirmwareBinary;
    unsigned long   dwFirmwareSize;
    unsigned long   dwFirmwareVersion;
    unsigned short  nModelNumber;
    char            szVersion[256];
    char            szModel[256];
    eFIRMWARE_TYPE   type;
};

class Dialog : public QDialog
{
    Q_OBJECT
    
private:
    QFWDPacket  m_Packet;
    int         m_TimerConnectDevice;
    int         m_TimerRequestTimeout;
    int         m_TimerRequestInformation;
    int         m_TimerWaitModeChange;
#ifdef Q_OS_WIN
    int         m_TimerCheckRunning;
#endif
    int         m_nCountDownTimeout;
    int         m_TimerCheckAdmin;

    QString     m_strDropFileName;

    enum QueryInfoStep {
        SUB_QUERY_MODE = 0,
        SUB_QUERY_VERSION,
        SUB_QUERY_IAP_VERSION,
        SUB_QUERY_IAP_REVISION,
        SUB_QUERY_WRITE_PROGRESS,
        SUB_QUERY_FINISH
    };

    enum JobItemType {
        JOBF_NONE = 0,
        JOBF_QUERY_INFO,
        JOBF_MARK_IAP,
        JOBF_MARK_APP,
        JOBF_RESET,
        JOBF_ERASE,
        JOBF_WRITE,
        JOBF_WAIT_IAP_ALL,
        JOBF_WAIT_APP_ALL
    };

    struct JobItem {
        JobItemType     type;
        QueryInfoStep   subStep;
        unsigned long   firmwareBinaryPos;
        unsigned short  which;
    };

    QList<JobItem>  m_JobListForRequestInformation;
    QList<JobItem>  m_JobListForFirmwareDownload;
    bool            m_bIsExecuteJob;
    bool            m_bIsStartRequestInformation;
    bool            m_bIsStartFirmwareDownload;
    JobItem         m_CurrentJob;
    unsigned short  m_nPacketId;

    bool            m_bIsInformationUpdated;

    bool            m_bWaitIAP;
    bool            m_bWaitIAPCheckOK;

    bool            m_bWaitAPP;
    bool            m_bWaitAPPCheckOK;

    int             m_nStableCheck;

    SensorInfo      m_SensorInfo[IDX_MAX];
    SensorInfo      m_TempSensorInfo[IDX_MAX];
    QList<FirmwareInfo*> m_FirmwareInfo;

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private:
    Ui::Dialog *ui;

    QString m_strSensorInformation;
    QString m_strDownloadProgress;

    bool    m_bAdministrator;

protected:
    virtual void timerEvent(QTimerEvent *evt);
    virtual void showEvent(QShowEvent *evt);
    virtual void closeEvent(QCloseEvent *evt);
    virtual void paintEvent(QPaintEvent *);
    virtual void dragEnterEvent(QDragEnterEvent *);
    virtual void dropEvent(QDropEvent *);

    void connectDevice();

    void startRequestTimeoutTimer(int nTimeout);
    void killRequestTimeoutTimer();

    void startWaitModeChangeTimer();
    void killWaitModeChangeTimer();

    void startQueryInformation( bool bDelay );
    void stopQueryInformation();

    void startFirmwareDownload();
    void stopFirmwareDownload();

    void queryInformation();
    void firmwareDownload();
//    void firmwareDownload(ushort nIDX);
    void stopAllQueryInformationJobs();
    void stopAllFirmwareDownloadJobs();

    void executeNextJob( bool bRetry=false );

    void onFinishAllRequestInformationJobs();
    void onFinishAllFirmwareDownloadJobs();

    void onFirmwareUpdateFailed();

    void updateSensorInformation();
    void updateFirmwareInformation();
    void displayInformation(const QString &strText );

    bool loadFirmwareFile(QString strPath = "");
    void loadFirmwarePartFile(QString strPath);

    bool analysisFirmwareBinary( const char* ver_info, FirmwareInfo* pFI );

    bool verifyFirmware(QString& strMsg);
    bool checkFWVersion(QString& strMsg);

    FirmwareInfo* findFirmware( eFIRMWARE_TYPE type, unsigned short nModelNumber );

    enum TextMode{ TM_NORMAL, TM_NG, TM_OK };
    void addProgressText(QString& strMessage, TextMode tm);
    
private slots:
    void on_pushButtonUpgrade_clicked();
    void on_pushButtonCancel_clicked();
    void on_toolButtonLicense_clicked();

    void onDisconnected();
    void onResponseFromSensor(unsigned short nPacketId);

    void onToggledPart(QString strPart, bool bChecked);

public slots:
    void onHandleMessage(const QString &msg);
};

#endif // DIALOG_H
