#ifndef MEDIAOUTPUT_H
#define MEDIAOUTPUT_H

#include "DllMacro.h"

#include <QtCore/QObject>

#include <phonon/AudioOutput>
#include <phonon/MediaObject>
#include <phonon/VolumeFaderEffect>


class DLLEXPORT MediaOutput : public Phonon::MediaObject
{
    Q_OBJECT

public:
    MediaOutput();

    bool isFadingAvailable();

    void blockSignals( bool block );

    void setVolume( qreal newVolume );
    qreal volume();

    void fadeIn( int fadeTime );
    void fadeOut( int fadeTime );

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


private:
    bool fadingAvailable;
    qint64 totalTimeInMSec;
    qint64 prefinishMark;
    Phonon::AudioOutput m_audioOutput;
    Phonon::VolumeFaderEffect m_mediaFader;
};

#endif // MEDIAOUTPUT_H
