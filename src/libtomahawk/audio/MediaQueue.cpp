#include "MediaQueue.h"


MediaQueue::MediaQueue()
    : QObject()
{
    m_currentMediaObject = 0;

    for (int i=0; i<MEDIA_OBJECT_COUNT; i++)
    {
        MediaOutput* mediaOutput = new MediaOutput;

        mediaOutput->setTickInterval( 150 );

        connect( mediaOutput, SIGNAL( stateChanged( Phonon::State, Phonon::State ) ), SLOT( onStateChanged( Phonon::State, Phonon::State ) ) );
        connect( mediaOutput, SIGNAL( tick( qint64 ) ), SLOT( timerTriggered( qint64 ) ) );
        connect( mediaOutput, SIGNAL( aboutToFinish() ), SLOT( onAboutToFinish() ) );
        connect( mediaOutput, SIGNAL( volumeChanged( qreal ) ), SLOT( onVolumeChanged( qreal ) ) );

        // only need next source earlier if the fading is available and so a crossfading is possible
        // if fading not available do not make a crossfading
        if ( mediaOutput->isFadingAvailable() )
        {
            mediaOutput->setPrefinishMark( CROSSFADING_TIME_IN_MS );
            connect( mediaOutput, SIGNAL( prefinishMarkReached( qint32 ) ), SLOT( onPrefinishMarkReached( qint32 ) ) );
        }

        m_mediaOutputs[i] = mediaOutput;
    }

    // 0 is first media output which will be used so disable other one
    m_mediaOutputs[m_currentMediaObject]->blockSignals( false );
}


qint64
MediaQueue::currentTime() const
{
    return m_mediaOutputs[m_currentMediaObject]->currentTime();
}


qint64
MediaQueue::totalTime() const
{
    return m_mediaOutputs[m_currentMediaObject]->totalTime();
}


void
MediaQueue::play()
{
    m_mediaOutputs[m_currentMediaObject]->play();
}


void
MediaQueue::pause()
{
    m_mediaOutputs[m_currentMediaObject]->pause();
}


void
MediaQueue::stop()
{
    m_mediaOutputs[m_currentMediaObject]->stop();
}


void
MediaQueue::seek( qint64 time )
{
    m_mediaOutputs[m_currentMediaObject]->seek( time );
}


void
MediaQueue::setNextSource( const Phonon::MediaSource& source, const bool autoDelete, const bool doCrossfading, const qint64 totalTime )
{
    if (doCrossfading)
    {
        // fade out running source
        m_mediaOutputs[m_currentMediaObject]->fadeOut( CROSSFADING_TIME_IN_MS );

        m_mediaOutputs[m_currentMediaObject]->blockSignals( true );
        m_currentMediaObject = getNextMediaIndex( m_currentMediaObject );

        m_mediaOutputs[m_currentMediaObject]->blockSignals( false );
    }

    // load next source
    const_cast<Phonon::MediaSource&>(source).setAutoDelete( autoDelete );
    m_mediaOutputs[m_currentMediaObject]->setCurrentSource( source );
    m_mediaOutputs[m_currentMediaObject]->setTotalTime(totalTime);

    if (doCrossfading)
    {
        // fade in next source
        m_mediaOutputs[m_currentMediaObject]->fadeIn( CROSSFADING_TIME_IN_MS );

        // start next source
        play();
    }
}


const int
MediaQueue::getNextMediaIndex(const int lastMediaQueue) const
{
    int nextMediaQueue = lastMediaQueue + 1;
    nextMediaQueue %= MEDIA_OBJECT_COUNT;

    return nextMediaQueue;
}


QString
MediaQueue::errorString() const
{
    m_mediaOutputs[m_currentMediaObject]->errorString();
}


Phonon::ErrorType
MediaQueue::errorType() const
{
    m_mediaOutputs[m_currentMediaObject]->errorType();
}


void
MediaQueue::onAboutToFinish()
{
    emit aboutToFinish();
}


void
MediaQueue::onStateChanged( Phonon::State newState, Phonon::State oldState )
{
    emit stateChanged( newState, oldState );
}


void
MediaQueue::timerTriggered( qint64 time )
{
    emit tick( time );
}

void
MediaQueue::onPrefinishMarkReached( qint32 msecToEnd )
{
    emit needNextSource();
}


qreal
MediaQueue::volume()
{
    return m_mediaOutputs[m_currentMediaObject]->volume();
}


void
MediaQueue::setVolume( qreal newVolume )
{
    for (int i=0; i<MEDIA_OBJECT_COUNT; i++)
        m_mediaOutputs[i]->setVolume( newVolume );
}

Phonon::State
MediaQueue::state() const
{
    m_mediaOutputs[m_currentMediaObject]->state();
}


void
MediaQueue::onVolumeChanged( qreal volume )
{
    emit volumeChanged( volume );
}
