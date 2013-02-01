// vim:cindent:cino=\:0:et:fenc=utf-8:ff=unix:sw=4:ts=4:

#ifndef JOBPRIVATE_H
#define JOBPRIVATE_H

#include <QString>

#include <conveyor/job.h>
#include <conveyor/conveyor.h>
#include <conveyor/jobstatus.h>

namespace conveyor
{
    class JobPrivate
    {
    public:
        JobPrivate
            ( Conveyor * conveyor
            , Job * job
            , int const & id
            );

        void updateFromJson (Json::Value const &);

        void cancel (void);
        
        Conveyor * const m_conveyor;
        Job * const m_job;
        Printer * m_printer;
        int m_id;
        QString m_name;
        JobState m_state;
        JobConclusion m_conclusion;
        QString m_currentStepName;
        int m_currentStepProgress;

        // Information about the machine or type of machine associated
        // with this job
        QString m_machineName;
        QString m_profileName;
    };
}

#endif
