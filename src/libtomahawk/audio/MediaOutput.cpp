#include "MediaOutput.h"

MediaOutput::MediaOutput()
    : totalTimeInMSec(0)
    , prefinishMark( 0 )
    , m_audioOutput( Phonon::MusicCategory, this )
    , m_mediaFader()
{
    m_mediaFader.setFadeCurve( Phonon::VolumeFaderEffect::Fade12Decibel );

    Phonon::Path path = Phonon::createPath( this, &m_audioOutput );

    fadingAvailable = path.insertEffect( &m_mediaFader );

    connect( &m_audioOutput, SIGNAL( volumeChanged( qreal ) ), SLOT( onVolumeChanged( qreal ) ) );
    connect( this, SIGNAL( tick( qint64 ) ), SLOT( checkPrefinishMark( qint64 ) ) );

    blockSignals( true );
}


bool
MediaOutput::isFadingAvailable()
{
    return fadingAvailable;
}


void
MediaOutput::fadeIn( int fadeTime )
{
    if (fadeTime <= 0)
        m_mediaFader.setVolume( 1.0 );
    else
    {
        m_mediaFader.setVolume( 0.0 );
        m_mediaFader.fadeIn( fadeTime );
    }
}


void
MediaOutput::fadeOut( int fadeTime )
{
    m_mediaFader.fadeOut( fadeTime );
}


qreal
MediaOutput::volume()
{
    return m_audioOutput.volume();
}


void
MediaOutput::setVolume( qreal newVolume )
{
    m_audioOutput.setVolume( newVolume );
}


void
MediaOutput::onVolumeChanged( qreal volume )
{
    emit volumeChanged( volume );
}


void
MediaOutput::blockSignals( bool block )
{
    Phonon::MediaObject::blockSignals( block );
    m_audioOutput.blockSignals( block );
    m_mediaFader.blockSignals( block );
}


void
MediaOutput::setCurrentSource(const Phonon::MediaSource& source)
{
    Phonon::MediaObject::setCurrentSource(source);

    totalTimeInMSec = Phonon::MediaObject::totalTime();
}


void
MediaOutput::setTotalTime( const qint64 externalTotalTime )
{
    totalTimeInMSec = Phonon::MediaObject::totalTime();

    // only overwrite if the time from the media object is wrong
    if (totalTimeInMSec <= 0)
        totalTimeInMSec = externalTotalTime;
}


qint64
MediaOutput::totalTime()
{
    return totalTimeInMSec;
}


void
MediaOutput::setPrefinishMark(qint32 msecToEnd)
{
    // do not use the prefinish mark of the media object because the media object
    // does not now the length of tracks which will be played from an online source
    prefinishMark = msecToEnd;
}


void
MediaOutput::checkPrefinishMark( qint64 time )
{
    if (totalTimeInMSec > 0 && prefinishMark > 0)
    {
        const qint64 timeToEnd = totalTimeInMSec - currentTime();
        if (timeToEnd < prefinishMark)
            emit prefinishMarkReached(timeToEnd);
    }
}
