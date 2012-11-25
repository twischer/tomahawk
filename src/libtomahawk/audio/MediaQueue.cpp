#include "MediaQueue.h"
#include <phonon/BackendCapabilities>
#include "utils/Logger.h"

MediaQueue::MediaQueue( Phonon::AudioOutput* audioOutput )
    : QObject()
{
    m_currentMediaObject = 0;

    for (int i=0; i<MEDIA_OBJECT_COUNT; i++)
    {
        m_mediaObjects[i] = new Phonon::MediaObject( this );

        Phonon::Path* path = new Phonon::Path();
        path->reconnect( m_mediaObjects[i], audioOutput );

        m_mediaFader[i] = new Phonon::VolumeFaderEffect;
        m_mediaFader[i]->setVolume( 0.0 );

        const bool successful = path->insertEffect( m_mediaFader[i] );

        m_mediaObjects[i]->setTickInterval( 150 );
    }

    QList<Phonon::EffectDescription> list = Phonon::BackendCapabilities::availableAudioEffects();

    foreach ( Phonon::EffectDescription desc, list )
    {
        tDebug() << desc.name() << desc.description() << endl;
    }

    connect( m_mediaObjects[0], SIGNAL( stateChanged( Phonon::State, Phonon::State ) ), SLOT( onStateChanged1( Phonon::State, Phonon::State ) ) );
    connect( m_mediaObjects[0], SIGNAL( tick( qint64 ) ), SLOT( timerTriggered1( qint64 ) ) );
    connect( m_mediaObjects[0], SIGNAL( aboutToFinish() ), SLOT( onAboutToFinish1() ) );
}


qint64
MediaQueue::currentTime() const
{
    return m_mediaObjects[m_currentMediaObject]->currentTime();
}


qint64
MediaQueue::totalTime() const
{
    return m_mediaObjects[m_currentMediaObject]->totalTime();
}


void
MediaQueue::play()
{
    m_mediaObjects[m_currentMediaObject]->play();
}


void
MediaQueue::pause()
{
    m_mediaObjects[m_currentMediaObject]->pause();
}


void
MediaQueue::stop()
{
    m_mediaObjects[m_currentMediaObject]->stop();
}


void
MediaQueue::seek( qint64 time )
{
    m_mediaObjects[m_currentMediaObject]->seek( time );
}


void
MediaQueue::setNextSource( const Phonon::MediaSource& source, const bool autoDelete )
{
    const_cast<Phonon::MediaSource&>(source).setAutoDelete( autoDelete );
    // TODO
    m_mediaObjects[m_currentMediaObject]->setCurrentSource( source );

    m_mediaFader[m_currentMediaObject]->setVolume( 1.0 );
    m_mediaFader[m_currentMediaObject]->fadeOut( CROSSFADING_TIME_IN_MS );
}


QString
MediaQueue::errorString() const
{
    m_mediaObjects[m_currentMediaObject]->errorString();
}


Phonon::ErrorType
MediaQueue::errorType() const
{
    m_mediaObjects[m_currentMediaObject]->errorType();
}


void
MediaQueue::onAboutToFinish1()
{
    if ( m_currentMediaObject == 0 )
        emit aboutToFinish();
}


void
MediaQueue::onStateChanged1( Phonon::State newState, Phonon::State oldState )
{
    if ( m_currentMediaObject == 0 )
        emit stateChanged( newState, oldState );
}


void
MediaQueue::timerTriggered1( qint64 time )
{
    if ( m_currentMediaObject == 0 )
        emit tick( time );
}
