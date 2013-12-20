#include "web/WebInterface.h"
#include "TomahawkSettings.h"
#include "ViewManager.h"
#include "playlist/PlaylistView.h"
#include "audio/MainAudioEngine.h"
#include "SourceList.h"

WebInterface::WebInterface(QxtAbstractWebSessionManager* sm)
    : Api_v1(sm),
      m_htmlHeader( getFileContent("header.html") ),
      m_htmlQueue( getFileContent("entry_queue.html") ),
      m_htmlSearch( getFileContent("body_search.html") ),
      m_htmlResult( getFileContent("entry_result.html") )
{
}


const QString
WebInterface::getFileContent(const QString& filename) const
{
    const QString filenameSource = QString(RESPATH "www/") + filename;

    if ( !QFile::exists( filenameSource ) )
        qWarning() << "Passed invalid file for html source:" << filenameSource;

    QFile f( filenameSource );
    f.open( QIODevice::ReadOnly );
    const QString data = f.readAll();

    return data;
}


void
WebInterface::index( QxtWebRequestEvent* event )
{
    if ( event->url.hasQueryItem("action") )
    {
        const QString action = getDecodedURLAttribute(event->url, "action");

        if (action.compare("toggle") == 0)
            MainAudioEngine::instance()->playPause();
        else if (action.compare("next") == 0)
        {
            if ( TomahawkSettings::instance()->partyModeEnabled() )
            {
                // TODO ask for authentification before playing the next song
                sendMessagePage( event, "<font color=red>Access denied!</font>" );
                return;
            }
            else
                MainAudioEngine::instance()->next();
        }
        else if (action.compare("lower") == 0)
            MainAudioEngine::instance()->lowerVolume();
        else if (action.compare("higher") == 0)
            MainAudioEngine::instance()->raiseVolume();
    }


    QStringMap bodyArgs;
    bodyArgs["query"] = QString();
    bodyArgs["playlist"] = "Noname";


    // TODO possible save the title oder the guid of the playlist in the interface
    const Tomahawk::playlistinterface_ptr currentPlInterface = MainAudioEngine::instance()->playlist();

    const QList<Tomahawk::playlist_ptr> pls = SourceList::instance()->getLocal()->collection()->playlists();
    foreach (const Tomahawk::playlist_ptr& pl, pls)
    {
        Tomahawk::playlistinterface_ptr interface = ViewManager::instance()->pageForPlaylist( pl )->playlistInterface();

        if (interface == currentPlInterface)
        {
            bodyArgs["playlist"] = pl->title();
            break;
        }
    }


    const Tomahawk::result_ptr currentTrack = MainAudioEngine::instance()->currentTrack();
    if (currentTrack)
        bodyArgs.unite( currentTrack->toHashMap() );


    const QList<Tomahawk::query_ptr> queries = ViewManager::instance()->queue()->model()->queries();

    QList< QStringMap > entries;
    foreach( const Tomahawk::query_ptr& query, queries )
    {
        const QStringMap entry = query->toHashMap();
        entries << entry;
    }

    sendMultiFilePage(event, "body_state.html", bodyArgs, "entry_queue.html", entries);
}


void
WebInterface::search( QxtWebRequestEvent* event )
{
    if ( event->url.hasQueryItem( "query" ) )
    {
        const QString query = getDecodedURLAttribute(event->url, "query");
        if (query.trimmed().isEmpty())
        {
            index(event);
        }
        else
        {
            const Tomahawk::WebSearchQuery* qry = new Tomahawk::WebSearchQuery(query, event);
            connect( qry, SIGNAL( searchFinished( const QString, const QList<Tomahawk::result_ptr>, const QxtWebRequestEvent* ) ),
                     SLOT( onSearchFinished( const QString, const QList<Tomahawk::result_ptr>, const QxtWebRequestEvent* ) ) );
        }
    }
    else
    {
        send404(event);
    }
}


void
WebInterface::onSearchFinished( const QString query, const QList<Tomahawk::result_ptr> results, const QxtWebRequestEvent* event )
{
    QList< QStringMap > entries;
    foreach( const Tomahawk::result_ptr& rp, results )
    {
        if ( rp->isOnline() )
        {
            const QStringMap entry = rp->toHashMap();
            entries << entry;
        }
    }

    QStringMap bodyArgs;
    bodyArgs["query"] = query;
    sendMultiFilePage(event, "body_search.html", bodyArgs, "entry_result.html", entries);
}


void
WebInterface::add( QxtWebRequestEvent* event )
{
    QString message = "<font color=red>Track could not be added!</font>";

    const QUrl& url = event->url;
    if ( url.hasQueryItem("trackid") )
    {
        const QString base64TrackID = getDecodedURLAttribute(url, "trackid");
        const QString trackID = QByteArray::fromBase64( base64TrackID.toAscii() );
        const QStringList trackInfo = trackID.split("|");

        const QString artist = trackInfo[0];
        const QString track = trackInfo[1];
        const QString album = trackInfo[2];

        // add track to the queue
        const Tomahawk::query_ptr query = Tomahawk::Query::get(artist, track, album);
        ViewManager::instance()->queue()->model()->appendQuery(query);

        // TODO PlaylistModel::playlistToFull abfangen und
        // nachricht an benutzer weiter geben

        message = QString("Track %1 - %2 successfully added to queue.").arg(artist, track);
    }

    sendMessagePage( event, message );
}


void
WebInterface::sendMessagePage( QxtWebRequestEvent* event, const QString message )
{
    QString page = getFileContent("message.html");
    page.replace("<%MESSAGE%>", message);

    sendPage(event, page);
}

void
WebInterface::playlists( QxtWebRequestEvent* event )
{
    const QList<Tomahawk::playlist_ptr> pls = SourceList::instance()->getLocal()->collection()->playlists();

    QList< QStringMap > entries;
    foreach (const Tomahawk::playlist_ptr& pl, pls)
    {
        QStringMap entry;
        entry["playlist"] = pl->title();
        entry["guid"] = pl->guid();
        entries << entry;
    }

    QStringMap bodyArgs;
    bodyArgs["query"] = QString();
    sendMultiFilePage(event, "body_playlists.html", bodyArgs, "entry_playlist.html", entries);
}


void
WebInterface::playlist( QxtWebRequestEvent* event, QString guid )
{
    const Tomahawk::playlist_ptr pls = Tomahawk::Playlist::load(guid);
    if ( pls.isNull() )
    {
        send404(event);
        return;
    }


    const QUrl& url = event->url;
    if ( url.hasQueryItem("action") )
    {
        const QString action = url.queryItemValue("action");
        if (action.compare("use") == 0)
        {
            if ( TomahawkSettings::instance()->partyModeEnabled() )
            {
                // TODO ask for authentification before playing the next song
                sendMessagePage( event, "<font color=red>Access denied!</font>" );
                return;
            }
            else
            {
                Tomahawk::playlistinterface_ptr interface = ViewManager::instance()->pageForPlaylist( pls )->playlistInterface();

                // start playback with first song of the selected playlist if not playing
                // otherwise only change the playlist (the next track will be used from the new playlist)
                if ( MainAudioEngine::instance()->state() == AudioEngine::Stopped )
                    MainAudioEngine::instance()->playItem( interface, interface->nextResult() );
                else
                    MainAudioEngine::instance()->setPlaylist( interface );
            }
        }

    }


    QList<QStringMap> entries;
    foreach (const Tomahawk::plentry_ptr& plEntry, pls->entries())
    {
        const QStringMap entry = plEntry->query()->toHashMap();
        entries << entry;
    }

    QStringMap bodyArgs;
    bodyArgs["query"] = QString();
    bodyArgs["playlist"] = pls->title();
    sendMultiFilePage(event, "body_playlist.html", bodyArgs, "entry_result.html", entries);
}


void
WebInterface::sendMultiFilePage(const QxtWebRequestEvent* event, const QString& bodyFile, const QStringMap& bodyArgs,
                                const QString& entryFile, const QList<QStringMap>& entryArgs)
{
    QString page = m_htmlHeader;

    const QString body = getFileContent(bodyFile);
    page.replace("<%BODY%>", body);

    foreach( const QString& param, bodyArgs.keys() )
    {
        page.replace( QString( "<%%1%>" ).arg( param.toUpper() ), bodyArgs.value( param ).toUtf8() );
    }


    const QString entryTemplate = getFileContent(entryFile);

    QString entries;
    foreach (QStringMap args, entryArgs)
    {
        QString entry = entryTemplate;

        if ( args.contains("album") )
        {
            // Add a seperator to the between artist and album if album is available
            const QString album = args["album"];
            const QString albumWithSeperator = album.trimmed().isEmpty() ? "" : QString("- %1").arg(album);
            args["album"] = albumWithSeperator;

            // Replace all track id fields if artist, track and album is set
            if (args.contains("artist") && args.contains("track"))
            {
                const QString trackID = QString("%1|%2|%3").arg( args["artist"], args["track"], album );

                const QString base64TrackID = trackID.toAscii().toBase64();
                entry.replace("<%TRACKID%>", base64TrackID);
            }
        }

        foreach( const QString& param, args.keys() )
        {
            entry.replace( QString( "<%%1%>" ).arg( param.toUpper() ), args.value( param ).toUtf8() );
        }
        entries += entry;
    }
    page.replace("<%ENTRIES%>", entries);


    sendPage(event, page);
}


void
WebInterface::sendPage(const QxtWebRequestEvent* event, const QString& page)
{
    QxtWebPageEvent* wpe = new QxtWebPageEvent( event->sessionID, event->requestID, page.toAscii() );
    postEvent( wpe );
}


const QString
WebInterface::getDecodedURLAttribute(const QUrl& url, const QString& key) const
{
    const QString encodedValue = url.queryItemValue(key);
    QString value = QUrl::fromPercentEncoding( encodedValue.toAscii() );
    value.replace("+", " ");

    return value;
}

