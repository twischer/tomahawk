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

    void setVolume( qreal newVolume );
    qreal volume();

    void fadeIn( int fadeTime );
    void fadeOut( int fadeTime );


signals:
    void volumeChanged( qreal volume );


private slots:
    void onVolumeChanged( qreal volume );


private:
    bool fadingAvailable;
    Phonon::AudioOutput m_audioOutput;
    Phonon::VolumeFaderEffect m_mediaFader;

    void blockSignals( bool block );
};

#endif // MEDIAOUTPUT_H
