#ifndef MEDIAOUTPUT_H
#define MEDIAOUTPUT_H

#include "DllMacro.h"

#include <QtCore/QObject>
#include <QTimer>

#include <phonon/AudioOutput>
#include <phonon/MediaObject>


class DLLEXPORT MediaOutput : public Phonon::MediaObject
{
    Q_OBJECT

public:
    static const qint32 CROSSFADING_INTERVAL_IN_MS;

    MediaOutput();

    void blockSignals( bool block );

    void setVolume( qreal newVolume );
    qreal volume();

    void fadeIn( qint32 fadeTime, qreal targetVolume );
    void fadeOut( qint32 fadeTime );

    void setCurrentSource(const Phonon::MediaSource &source);

    void setTotalTime( const qint64 totalTime );
    qint64 totalTime();

    void setPrefinishMark(qint32 msecToEnd);


signals:
    void volumeChanged( qreal volume );
    void prefinishMarkReached(qint32 msecToEnd);


private slots:
    void onVolumeChanged( qreal volume );
    void checkPrefinishMark( qint64 time );
    void timerTriggered();


private:
    qint64 totalTimeInMSec;
    qint64 prefinishMark;
    qreal fadeVolumeDiff;
    qint32 leftFadeSteps;
    QTimer timer;
    Phonon::AudioOutput m_audioOutput;

    void fadeTo( qint32 fadeTime, qreal targetVolumeDiff );
};

#endif // MEDIAOUTPUT_H
