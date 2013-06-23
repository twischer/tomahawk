#include "PreviewPlayer.h"
#include "ui_PreviewPlayer.h"

#include "audio/PreviewAudioEngine.h"

PreviewPlayer::PreviewPlayer(QWidget *parent) :
    QDialog(parent, Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint),
    ui(new Ui::PreviewPlayer)
{
    ui->setupUi(this);

    ui->seekSlider->setTimeLine( &m_sliderTimeLine );
    ui->volumeSlider->setRange( 0, 100 );
    ui->volumeSlider->setValue( PreviewAudioEngine::instance()->volume() );

    connect( ui->seekSlider,       SIGNAL( valueChanged( int ) ), PreviewAudioEngine::instance(), SLOT( seek( int ) ) );
    connect( ui->volumeSlider,     SIGNAL( valueChanged( int ) ), PreviewAudioEngine::instance(), SLOT( setVolume( int ) ) );
    connect( ui->volumeLowButton,  SIGNAL( clicked() ), PreviewAudioEngine::instance(), SLOT( lowerVolume() ) );
    connect( ui->volumeHighButton, SIGNAL( clicked() ), PreviewAudioEngine::instance(), SLOT( raiseVolume() ) );

    connect( &m_sliderTimeLine,    SIGNAL( frameChanged( int ) ), ui->seekSlider, SLOT( setValue( int ) ) );

    // <From PreviewAudioEngine>
    connect( PreviewAudioEngine::instance(), SIGNAL( loading( Tomahawk::result_ptr ) ), SLOT( onPlaybackLoading( Tomahawk::result_ptr ) ) );
    connect( PreviewAudioEngine::instance(), SIGNAL( started( Tomahawk::result_ptr ) ), SLOT( onPlaybackStarted( Tomahawk::result_ptr ) ) );
    connect( PreviewAudioEngine::instance(), SIGNAL( seeked( qint64 ) ), SLOT( onPlaybackTimer( qint64 ) ) );
    connect( PreviewAudioEngine::instance(), SIGNAL( timerMilliSeconds( qint64 ) ), SLOT( onPlaybackTimer( qint64 ) ) );
    connect( PreviewAudioEngine::instance(), SIGNAL( volumeChanged( int ) ), SLOT( onVolumeChanged( int ) ) );
}


PreviewPlayer::~PreviewPlayer()
{
    delete ui;
}


void
PreviewPlayer::onVolumeChanged( int volume )
{
    ui->volumeSlider->blockSignals( true );
    ui->volumeSlider->setValue( volume );
    ui->volumeSlider->blockSignals( false );
}


void
PreviewPlayer::onPlaybackStarted( const Tomahawk::result_ptr& result )
{
    if ( result.isNull() )
        return;

    if ( m_currentTrack.isNull() || ( !m_currentTrack.isNull() && m_currentTrack.data()->id() != result.data()->id() ) )
        onPlaybackLoading( result );

    qint64 duration = PreviewAudioEngine::instance()->currentTrackTotalTime();

    if ( duration == -1 )
        duration = result.data()->duration() * 1000;

    ui->seekSlider->setRange( 0, duration );
    ui->seekSlider->setValue( 0 );

    ui->seekSlider->setEnabled( PreviewAudioEngine::instance()->canSeek() );

    ui->timeLabel->setText( TomahawkUtils::timeToString( 0 ) );
    ui->timeLeftLabel->setText( "-" + TomahawkUtils::timeToString( 0 ) );

    m_sliderTimeLine.setDuration( duration );
    m_sliderTimeLine.setFrameRange( 0, duration );
    m_sliderTimeLine.setCurveShape( QTimeLine::LinearCurve );
    m_sliderTimeLine.setCurrentTime( 0 );

    int updateRate = (double)1000 / ( (double)ui->seekSlider->contentsRect().width() / (double)( duration / 1000 ) );
    m_sliderTimeLine.setUpdateInterval( qBound( 40, updateRate, 500 ) );

    m_sliderTimeLine.resume();
}


void
PreviewPlayer::onPlaybackLoading( const Tomahawk::result_ptr& result )
{
    m_currentTrack = result;

    ui->artistTrackLabel->setResult( result );
    ui->albumLabel->setResult( result );

    const QString duration = TomahawkUtils::timeToString( result.data()->duration() );
    ui->timeLabel->setFixedWidth( ui->timeLabel->fontMetrics().width( QString( duration.length(), QChar( '0' ) ) ) );
    ui->timeLabel->setText( TomahawkUtils::timeToString( 0 ) );
    ui->timeLeftLabel->setFixedWidth( ui->timeLeftLabel->fontMetrics().width( QString( duration.length() + 1, QChar( '0' ) ) ) );
    ui->timeLeftLabel->setText( "-" + duration );
}


void
PreviewPlayer::onPlaybackTimer( qint64 msElapsed )
{
    //tDebug() << Q_FUNC_INFO;

    if ( m_currentTrack.isNull() )
    {
        m_sliderTimeLine.stop();
        return;
    }

    const int seconds = msElapsed / 1000;
    if ( seconds != m_lastTextSecondShown )
    {
        ui->timeLabel->setText( TomahawkUtils::timeToString( seconds ) );
        ui->timeLeftLabel->setText( "-" + TomahawkUtils::timeToString( m_currentTrack->duration() - seconds ) );
        m_lastTextSecondShown = seconds;
    }


    if ( msElapsed == 0 )
        return;

    int currentTime = m_sliderTimeLine.currentTime();
    if ( qAbs( msElapsed - currentTime ) > AudioControls::ALLOWED_MAX_DIVERSION )
    {
        ui->seekSlider->blockSignals( true );

        m_sliderTimeLine.setPaused( true );
        m_sliderTimeLine.setCurrentTime( msElapsed );
        m_sliderTimeLine.resume();

        ui->seekSlider->blockSignals( false );
    }
}


bool
PreviewPlayer::canGoNext()
{
    // disabling playing next song if the last one was played
    return false;
}
