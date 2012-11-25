#ifndef MEDIAQUEUE_H
#define MEDIAQUEUE_H

#include "DllMacro.h"

#include <phonon/AudioOutput>
#include <phonon/MediaObject>
#include <phonon/VolumeFaderEffect>

#include <QtCore/QObject>


#define MEDIA_OBJECT_COUNT  2
#define CROSSFADING_TIME_IN_MS  16000


class DLLEXPORT MediaQueue : public QObject
{
    Q_OBJECT

public:
    MediaQueue( Phonon::AudioOutput* audioOutput );

    qint64 currentTime() const;
    qint64 totalTime() const;

    void play();
    void pause();
    void stop();
    void seek( qint64 time );

    void setNextSource( const Phonon::MediaSource& source, const bool autoDelete );

    QString	errorString() const;
    Phonon::ErrorType errorType() const;


signals:
    void aboutToFinish();
    void stateChanged( Phonon::State newState, Phonon::State oldState );
    void tick( qint64 time );


private slots:
    void onAboutToFinish1();
    void onStateChanged1( Phonon::State newState, Phonon::State oldState );
    void timerTriggered1( qint64 time );


private:
    int m_currentMediaObject;

    Phonon::MediaObject* m_mediaObjects[MEDIA_OBJECT_COUNT];
    Phonon::VolumeFaderEffect* m_mediaFader[MEDIA_OBJECT_COUNT];
};

#endif // MEDIAQUEUE_H
