#ifndef MEDIAQUEUE_H
#define MEDIAQUEUE_H

#include "DllMacro.h"

#include "MediaOutput.h"

#include <QtCore/QObject>


#define MEDIA_OBJECT_COUNT  2
#define CROSSFADING_TIME_IN_MS  4000


class DLLEXPORT MediaQueue : public QObject
{
    Q_OBJECT

public:
    MediaQueue();

    qint64 currentTime() const;
    qint64 totalTime() const;

    void play();
    void pause();
    void stop();
    void seek( qint64 time );

    void setNextSource( const Phonon::MediaSource& source, const bool autoDelete, const bool doCrossfading );

    QString	errorString() const;
    Phonon::ErrorType errorType() const;

    void setVolume( qreal newVolume );
    qreal volume();


signals:
    void aboutToFinish();
    void stateChanged( Phonon::State newState, Phonon::State oldState );
    void tick( qint64 time );
    void needNextSource();
    void volumeChanged( qreal volume );


private slots:
    void onAboutToFinish1();
    void onStateChanged1( Phonon::State newState, Phonon::State oldState );
    void timerTriggered1( qint64 time );
    void onPrefinishMarkReached1( qint32 msecToEnd );
    void onVolumeChanged( qreal volume );


private:
    const int getNextMediaIndex(const int lastMediaQueue) const;

    int m_currentMediaObject;

    MediaOutput* m_mediaOutputs[MEDIA_OBJECT_COUNT];
};

#endif // MEDIAQUEUE_H
