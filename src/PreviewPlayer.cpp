#include "PreviewPlayer.h"
#include "ui_PreviewPlayer.h"

#include "audio/PreviewAudioEngine.h"

PreviewPlayer::PreviewPlayer(QWidget *parent) :
    QDialog(parent, Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint),
    ui(new Ui::PreviewPlayer)
{
    ui->setupUi(this);

    connect( ui->seekSlider,       SIGNAL( valueChanged( int ) ), PreviewAudioEngine::instance(), SLOT( seek( int ) ) );
    connect( ui->volumeSlider,     SIGNAL( valueChanged( int ) ), PreviewAudioEngine::instance(), SLOT( setVolume( int ) ) );
    connect( ui->volumeLowButton,  SIGNAL( clicked() ), PreviewAudioEngine::instance(), SLOT( lowerVolume() ) );
    connect( ui->volumeHighButton, SIGNAL( clicked() ), PreviewAudioEngine::instance(), SLOT( raiseVolume() ) );
/*
    // <From PreviewEngine>
    connect( PreviewEngine::instance(), SIGNAL( loading( Tomahawk::result_ptr ) ), SLOT( onPlaybackLoading( Tomahawk::result_ptr ) ) );
    connect( PreviewEngine::instance(), SIGNAL( started( Tomahawk::result_ptr ) ), SLOT( onPlaybackStarted( Tomahawk::result_ptr ) ) );
    connect( PreviewEngine::instance(), SIGNAL( seeked( qint64 ) ), SLOT( onPlaybackSeeked( qint64 ) ) );
    connect( PreviewEngine::instance(), SIGNAL( timerMilliSeconds( qint64 ) ), SLOT( onPlaybackTimer( qint64 ) ) );
    connect( PreviewEngine::instance(), SIGNAL( volumeChanged( int ) ), SLOT( onVolumeChanged( int ) ) );*/
}

PreviewPlayer::~PreviewPlayer()
{
    delete ui;
}
