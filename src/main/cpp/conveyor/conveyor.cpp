// vim:cindent:cino=\:0:et:fenc=utf-8:ff=unix:sw=4:ts=4:

#include "conveyor.h"

#include <QUuid>
#include <QTime>
namespace conveyor
{
    struct ConveyorPrivate
    {
    };

    struct JobPrivate
    {

        QString m_displayName;
        QString m_uniqueName;
        int m_Progress;
        JobStatus m_Status;
    };

    struct PrinterPrivate
    {
        QString m_displayName;
        QString m_uniqueName;
        QString m_printerType;
        QList<Job *> m_jobs;
        bool m_canPrint;
        bool m_canPrintToFile;
        bool m_hasPlatform;
        Conveyor * m_Conveyor;
        int m_numberOfToolheads;

        ConnectionStatus m_connectionStatus;
    };

    Conveyor::Conveyor (Address const & address __attribute__ ((unused)))
        : m_private (0)
    {
        qsrand(QTime::currentTime().msec());

    }

    QList<Job *>
    Conveyor::jobs ()
    {
        QList<Job *> list;
        return list;
    }

    QList<Printer *>
    Conveyor::printers ()
    {
        QList<Printer *> list;
        return list;
    }

    Job::Job
        ( Printer * printer __attribute__ ((unused))
        , QString const & id
        )
        :m_private(new JobPrivate())
    {
        m_private->m_Progress = 0;
        m_private->m_uniqueName = id;
        m_private->m_Status = PRINTING;
    }
    Job::Job
    (Printer * printer __attribute__ ((unused)),
	 QString const &name,
     int const &progress):m_private(new JobPrivate())
    {
        m_private->m_displayName = name;
        m_private->m_Progress = progress;
        m_private->m_uniqueName = QUuid::createUuid().toString();
        m_private->m_Status = PRINTING;
    }
    int Job::progress()
    {
        return m_private->m_Progress;
    }
    void Job::incrementProgress()
    {
        m_private->m_Progress++;
        emit JobPercentageChanged(m_private->m_Progress);
    }

    JobStatus Job::jobStatus() const
    {
        return m_private->m_Status;
    }


    Printer::Printer
        ( Conveyor  * conveyor __attribute__ ((unused))
        , QString const & name __attribute__ ((unused))
        )
        : m_private (new PrinterPrivate())
    {
        m_private->m_canPrint = true;
        m_private->m_canPrintToFile = true;
        m_private->m_connectionStatus = NOT_CONNECTED;
        m_private->m_displayName = "Dummy Printer";
        m_private->m_printerType = "Replicator";
        m_private->m_uniqueName = QUuid::createUuid().toString();
        m_private->m_numberOfToolheads = 2;
        m_private->m_hasPlatform = true;
        m_private->m_jobs = conveyor->jobs();
        m_private->m_Conveyor = conveyor;
    }
    Printer::Printer
		(Conveyor *convey
		, const QString &name
		, const bool &canPrint
		, const bool &canPrintToFile
		, const ConnectionStatus &cs
        , const QString &printerType
		, const int &numberOfExtruders
		, const bool &hasHeatedPlatform
		)
		: m_private(new PrinterPrivate())
    {
        m_private->m_canPrint = canPrint;
        m_private->m_canPrintToFile = canPrintToFile;
        m_private->m_connectionStatus = cs;
        m_private->m_displayName = name;
        m_private->m_printerType = printerType;
        m_private->m_uniqueName = QUuid::createUuid().toString();
        m_private->m_numberOfToolheads = numberOfExtruders;
        m_private->m_hasPlatform = hasHeatedPlatform;

        m_private->m_jobs = convey->jobs();
        m_private->m_Conveyor = convey;


    }

    Printer::~Printer ()
    {
        delete m_private;
    }

    QList<Job *>
    * Printer::jobs ()
    {
       return &m_private->m_jobs;
    }

    Job *
    Printer::currentJob ()
    {
        if(m_private->m_jobs.isEmpty())
            return 0;

        return m_private->m_jobs.first();
    }

    QString const &
    Printer::displayName () const
    {
        return m_private->m_displayName;
    }

    QString const &
    Printer::uniqueName () const
    {
        return m_private->m_uniqueName;
    }

    QString const &
    Printer::printerType () const
    {
        return m_private->m_printerType;
    }

    bool
    Printer::canPrint () const
    {
        return m_private->m_canPrint;
    }

    bool Printer::hasHeatedPlatform() const
    {
        return m_private->m_hasPlatform;
    }

    bool
    Printer::canPrintToFile () const
    {
        return m_private->m_canPrintToFile;
    }

    int Printer::numberOfExtruders() const
    {
        return m_private->m_numberOfToolheads;
    }

    ConnectionStatus
    Printer::connectionStatus () const
    {
        return m_private->m_connectionStatus;
    }

    QString
    Printer::connectionStatusString () const
    {
        QString status;

        switch(m_private->m_connectionStatus)
        {
        case CONNECTED:
            status = "Connected";
            break;
        case NOT_CONNECTED:
            status = "Not Connected";
            break;
        }

        return status;
    }
    Conveyor * Printer::conveyor()
    {
        return m_private->m_Conveyor;
    }

    Job *
    Printer::print
        ( QString const & inputFile __attribute__ ((unused))
        )
    {
        QString jobID("fakePrintID:" + QUuid::createUuid().toString());
        Job * job = new Job(this, jobID);
        m_private->m_jobs.append(job);
        return job;
    }

    Job *
    Printer::printToFile
        ( QString const & inputFile __attribute__ ((unused))
        , QString const & outputFile __attribute__ ((unused))
        )
    {
        QString jobID("fakePrintToFileID:" + QUuid::createUuid().toString());
        Job * job = new Job(this, jobID);
        m_private->m_jobs.append(job);
        return job;
    }

    Job *
    Printer::slice
        ( QString const & inputFile __attribute__ ((unused))
        , QString const & outputFile __attribute__ ((unused))
        )
    {
        QString jobID("fakeSliceID:" + QUuid::createUuid().toString());
        Job * job = new Job(this, jobID);
        m_private->m_jobs.append(job);
        return job;
    }

    void
    Printer::jog
		( float x
		, float y
		, float z
		, float a
        , float b
		, float f
		)
    {
        qDebug() << "jogging x"<<x<<" y"<<y<<" z"<<z<<" a"<<a<<" b"<<b<<" f"<<f;
        //Jogz
    }
    void Printer::togglePaused()
    {
        qDebug() << "1. jobstatus" << this->currentJob()->jobStatus();
        if(this->currentJob()->jobStatus() == PRINTING)
        {
            this->currentJob()->m_private->m_Status = PAUSED;
        }
        else if(this->currentJob()->jobStatus() == PAUSED)
        {
            this->currentJob()->m_private->m_Status = PRINTING;
        }
        qDebug() << "2. jobstatus" << this->currentJob()->jobStatus();

    }
    void Printer::cancelCurrentJob()
    {
        this->m_private->m_jobs.first()->m_private->m_Status = CANCELLED;
        emit m_private->m_Conveyor->jobRemoved();


    }

    FakePrinter::FakePrinter (Conveyor * convey, QString const & name) :Printer(convey, name)
        {
            //qDebug() << "y me no work :(";
            connect(&m_ConnectionTimer, SIGNAL(timeout()), this, SLOT(emitRandomConnectionStatus()));
        }
    FakePrinter::FakePrinter (Conveyor *convey, const QString &name, const bool &canPrint, const bool &canPrintToFile, const ConnectionStatus &cs,
             const QString &printerType, const int &numberOfExtruders, const bool &hasHeatedPlatform)
    :Printer(convey, name , canPrint, canPrintToFile, cs, printerType, numberOfExtruders, hasHeatedPlatform)
    {
        qDebug() << m_private->m_displayName;
        connect(&m_ConnectionTimer, SIGNAL(timeout()), this, SLOT(emitRandomConnectionStatus()));

    }
    void FakePrinter::startRandomConnectionStatus()
    {
        m_ConnectionTimer.start(1000);

    }
    void FakePrinter::stopRandomConnectionStatus()
    {
        m_ConnectionTimer.stop();
    }
    void FakePrinter::emitRandomConnectionStatus()
    {
        //qDebug() << "1. connection status: " << m_private->m_connectionStatus;
        int rnd = qrand();
        if(rnd % 4 == 0) //one fourth of the time actually swap stuff
        {
           m_private->m_connectionStatus = m_private->m_connectionStatus == CONNECTED ? NOT_CONNECTED : CONNECTED;
           qDebug() << "emitting change! for printer:" << m_private->m_displayName;
           emit connectionStatusChanged(m_private->m_connectionStatus);
        }
        //qDebug() << "2. connection status: " << m_private->m_connectionStatus;

    }

    void FakePrinter::startCurrentJob()
    {
        m_JobTimer.start(1000);
    }
    void FakePrinter::stopCurrentJob()
    {
        m_JobTimer.stop();
    }
    void FakePrinter::togglePaused()
    {

        Printer::togglePaused();
        qDebug() << "3. jobstatus" << this->currentJob()->jobStatus();

        if(this->currentJob()->jobStatus() == PAUSED)
        {
            stopCurrentJob();
        }
        else if(this->currentJob()->jobStatus() == PRINTING)
        {
            startCurrentJob();
        }
    }
    Job *
    FakePrinter::print
        ( QString const & inputFile
        )
    {
        Job * job = Printer::print(inputFile);
        connect(&m_JobTimer, SIGNAL(timeout()), job, SLOT(incrementProgress()));
        return job;
    }

    Job *
    FakePrinter::printToFile
        ( QString const & inputFile
        , QString const & outputFile
        )
    {

        Job * job = Printer::printToFile(inputFile,outputFile);
        connect(&m_JobTimer, SIGNAL(timeout()), job, SLOT(incrementProgress()));
        return job;
    }

    Job *
    FakePrinter::slice
        ( QString const & inputFile
        , QString const & outputFile
        )
    {
        Job * job = Printer::slice(inputFile,outputFile);
        connect(&m_JobTimer, SIGNAL(timeout()), job, SLOT(incrementProgress()));
        return job;
    }
    void FakePrinter::cancelCurrentJob()
    {
        stopCurrentJob();
        Printer::cancelCurrentJob();
    }

    Address WindowsDefaultAddress;
    Address UNIXDefaultAddress;

    Address&
    defaultAddress()
    {
        #if defined(CONVEYOR_ADDRESS)
            return CONVEYOR_ADDRESS;
        #elif defined(Q_OS_WIN32)
            return WindowsDefaultAddress;
        #elif defined(Q_OS_MAC)
            return UNIXDefaultAddress;
        #elif defined(Q_OS_LINUX)
            return UNIXDefaultAddress;
        #else
            #error No CONVEYOR_ADDRESS defined and no default location known for this platform
        #endif
    }
}
