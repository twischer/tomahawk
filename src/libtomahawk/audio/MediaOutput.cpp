#include "MediaOutput.h"

MediaOutput::MediaOutput()
    : m_audioOutput( Phonon::MusicCategory, this )
    , m_mediaFader()
{
    blockSignals( true );

    m_mediaFader.setFadeCurve( Phonon::VolumeFaderEffect::Fade12Decibel );

    Phonon::Path path = Phonon::createPath( this, &m_audioOutput );

    fadingAvailable = path.insertEffect( &m_mediaFader );

    connect( &m_audioOutput, SIGNAL( volumeChanged( qreal ) ), SLOT( onVolumeChanged( qreal ) ) );
}


bool
MediaOutput::isFadingAvailable()
{
    return fadingAvailable;
}


void
MediaOutput::fadeIn( int fadeTime )
{
    blockSignals( false );

    m_mediaFader.setVolume( 0.0 );
    m_mediaFader.fadeIn( fadeTime );
}


void
MediaOutput::fadeOut( int fadeTime )
{
    m_mediaFader.fadeOut( fadeTime );

    blockSignals( true );
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
