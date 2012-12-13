#include "MediaQueue.h"


MediaQueue::MediaQueue()
    : QObject()
{
    m_currentMediaObject = 0;

    for (int i=0; i<MEDIA_OBJECT_COUNT; i++)
    {
        MediaOutput* mediaOutput = new MediaOutput;

        mediaOutput->setTickInterval( 150 );

        connect( mediaOutput, SIGNAL( stateChanged( Phonon::State, Phonon::State ) ), SLOT( onStateChanged1( Phonon::State, Phonon::State ) ) );
        connect( mediaOutput, SIGNAL( tick( qint64 ) ), SLOT( timerTriggered1( qint64 ) ) );
        connect( mediaOutput, SIGNAL( aboutToFinish() ), SLOT( onAboutToFinish1() ) );
        connect( mediaOutput, SIGNAL( volumeChanged( qreal ) ), SLOT( onVolumeChanged( qreal ) ) );

        // only need next source earlier if the fading is available and so a crossfading is possible
        // if fading not available do not make a crossfading
        if ( mediaOutput->isFadingAvailable() )
        {
            mediaOutput->setPrefinishMark( CROSSFADING_TIME_IN_MS );
            connect( mediaOutput, SIGNAL( prefinishMarkReached( qint32 ) ), SLOT( onPrefinishMarkReached1( qint32 ) ) );
        }

        m_mediaOutputs[i] = mediaOutput;
    }
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
MediaQueue::setNextSource( const Phonon::MediaSource& source, const bool autoDelete, const bool doCrossfading )
{
    // TODO lautstärke muss wird auf 100% gesetzt werden wenn doCrassfading == false
    if (doCrossfading)
        // fade out running source
        m_mediaOutputs[m_currentMediaObject]->fadeOut( CROSSFADING_TIME_IN_MS );
    else
        stop();

    m_mediaOutputs[m_currentMediaObject]->blockSignals( true );

    m_currentMediaObject = getNextMediaIndex( m_currentMediaObject );

    m_mediaOutputs[m_currentMediaObject]->blockSignals( false );

    // TODO nach 3. lied keine wiedergabe mehr

    // load next source
    const_cast<Phonon::MediaSource&>(source).setAutoDelete( autoDelete );
    m_mediaOutputs[m_currentMediaObject]->setCurrentSource( source );

    if (doCrossfading)
    {
        // fade in next source
        m_mediaOutputs[m_currentMediaObject]->fadeIn( CROSSFADING_TIME_IN_MS );
    }

    // start next source
    play();
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
MediaQueue::onAboutToFinish1()
{
    emit aboutToFinish();
}


void
MediaQueue::onStateChanged1( Phonon::State newState, Phonon::State oldState )
{
    emit stateChanged( newState, oldState );
}


void
MediaQueue::timerTriggered1( qint64 time )
{
    emit tick( time );
}

void
MediaQueue::onPrefinishMarkReached1( qint32 msecToEnd )
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


void
MediaQueue::onVolumeChanged( qreal volume )
{
    emit volumeChanged( volume );
}
