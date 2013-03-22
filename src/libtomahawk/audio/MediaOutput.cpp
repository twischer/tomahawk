#include "MediaOutput.h"


const qint32 MediaOutput::CROSSFADING_INTERVAL_IN_MS = 100;


MediaOutput::MediaOutput()
    : totalTimeInMSec(0)
    , prefinishMark( 0 )
    , fadeVolumeDiff( 0.0 )
    , leftFadeSteps ( 0 )
    , timer(this)
    , m_audioOutput( Phonon::MusicCategory, this )
{
    Phonon::Path path = Phonon::createPath( this, &m_audioOutput );

    connect( &m_audioOutput, SIGNAL( volumeChanged( qreal ) ), SLOT( onVolumeChanged( qreal ) ) );
    connect( this, SIGNAL( tick( qint64 ) ), SLOT( checkPrefinishMark( qint64 ) ) );

    connect( &timer, SIGNAL(timeout()), SLOT(timerTriggered()) );

    blockSignals( true );
}


void
MediaOutput::fadeIn( qint32 fadeTime, qreal targetVolume )
{
    if (fadeTime <= 0)
        setVolume( targetVolume );
    else
    {
        setVolume( 0.0 );
        fadeTo( fadeTime, targetVolume );
    }
}


void
MediaOutput::fadeOut( qint32 fadeTime )
{
    fadeTo( fadeTime, -volume() );
}

void
MediaOutput::fadeTo( qint32 fadeTime, qreal targetVolumeDiff )
{
    leftFadeSteps = fadeTime / CROSSFADING_INTERVAL_IN_MS;
    fadeVolumeDiff = targetVolumeDiff / leftFadeSteps;
    timer.start(CROSSFADING_INTERVAL_IN_MS);
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

void
MediaOutput::timerTriggered()
{
    leftFadeSteps--;

    if (leftFadeSteps >= 0)
    {
        const qreal currentVolume = volume();
        setVolume( currentVolume + fadeVolumeDiff );

        if (leftFadeSteps == 0)
            timer.stop();
    }
}
