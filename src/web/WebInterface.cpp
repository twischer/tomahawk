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
    // execute load revision if a new playlist was loaded or created
    connect( SourceList::instance()->getLocal()->dbCollection().data(), SIGNAL( playlistsAdded( QList<Tomahawk::playlist_ptr> ) ),
             SLOT( onPlaylistsAdded( QList<Tomahawk::playlist_ptr> ) ) );

    // load all playlists from the database
    SourceList::instance()->getLocal()->dbCollection()->playlists();
}


void
WebInterface::onPlaylistsAdded( const QList<Tomahawk::playlist_ptr>& playlists )
{
    foreach (const Tomahawk::playlist_ptr& pl, playlists)
    {
        // load and resolve the tracks of the playlist
        pl->loadRevision();
        pl->resolve();
    }
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
        if ( TomahawkSettings::instance()->partyModeEnabled() )
        {
            if ( !checkAuthorization(event) )
                return;
        }

        const QString action = getDecodedURLAttribute(event->url, "action");

        if (action.compare("toggle") == 0)
            MainAudioEngine::instance()->playPause();
        else if (action.compare("next") == 0)
            MainAudioEngine::instance()->next();
        else if (action.compare("lower") == 0)
            MainAudioEngine::instance()->lowerVolume();
        else if (action.compare("higher") == 0)
            MainAudioEngine::instance()->raiseVolume();
        else if (action.compare("partymode") == 0)
            TomahawkSettings::instance()->setPartyModeEnabled( !TomahawkSettings::instance()->partyModeEnabled() );
    }


    QStringMap bodyArgs;
    bodyArgs["query"] = QString();
    bodyArgs["playlist"] = "Noname";


    // TODO possible save the title or the guid of the playlist in the interface
    const Tomahawk::playlistinterface_ptr currentPlInterface = MainAudioEngine::instance()->playlist();

    foreach (const Tomahawk::collection_ptr& collection, SourceList::instance()->getLocal()->collections())
    {
        const QList<Tomahawk::playlist_ptr> pls = collection->playlists();
        foreach (const Tomahawk::playlist_ptr& pl, pls)
        {
            Tomahawk::playlistinterface_ptr interface = ViewManager::instance()->pageForPlaylist( pl )->playlistInterface();

            if (interface == currentPlInterface)
            {
                bodyArgs["playlist"] = pl->title();
                break;
            }
        }
    }

    const Tomahawk::result_ptr currentTrack = MainAudioEngine::instance()->currentTrack();
    if (currentTrack)
        bodyArgs.unite( currentTrack->toHashMap() );


    QList< QStringMap > entries;
    const QList<Tomahawk::query_ptr> queries = ViewManager::instance()->queue()->model()->queries();
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
WebInterface::addResultsToMap(const QList<Tomahawk::result_ptr> results, QList<QStringMap>& entries)
{
    foreach( const Tomahawk::result_ptr& rp, results )
    {
        if ( rp->isOnline() )
        {
            const QStringMap entry = rp->toHashMap();
            entries << entry;
        }
    }
}

void
WebInterface::onSearchFinished( const QString query, const QList<Tomahawk::result_ptr> results, const QxtWebRequestEvent* event )
{
    // Cache for the results to make it possible to resolve the results by url
    static QHash< const QHostAddress, QList<Tomahawk::result_ptr> > resultCache;
    const QHostAddress remoteAddress = event->remoteAddress;
    // clean the result list if already exists one (so the result hash will be cleaned automatically, too)
    resultCache.remove(remoteAddress);
    resultCache.insert(remoteAddress, results);

    QList< QStringMap > entries;
    addResultsToMap(results, entries);

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

        // add track to the queue
        const Tomahawk::result_ptr result = Tomahawk::Result::get(trackID);
        const Tomahawk::query_ptr query = result->toQuery();

        ViewManager::instance()->queue()->model()->appendQuery(query);
// TODO use this one on heeadless to mind to initilize the view manager
//        MainAudioEngine::instance()->queue()->tracks().append(query);

        // TODO PlaylistModel::playlistToFull abfangen und
        // nachricht an benutzer weiter geben

        message = QString("Track %1 successfully added to queue.").arg(trackID);
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
    QList< QStringMap > entries;
    foreach (const Tomahawk::collection_ptr& collection, SourceList::instance()->getLocal()->collections())
    {
        const QList<Tomahawk::playlist_ptr> pls = collection->playlists();
        foreach (const Tomahawk::playlist_ptr& pl, pls)
        {
            QStringMap entry;
            entry["playlist"] = pl->title();
            entry["guid"] = pl->guid();
            entries << entry;
        }
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
                if ( !checkAuthorization(event) )
                    return;
            }


            Tomahawk::playlistinterface_ptr interface = ViewManager::instance()->pageForPlaylist( pls )->playlistInterface();

            // start playback with first song of the selected playlist if not playing
            // otherwise only change the playlist (the next track will be used from the new playlist)
            if ( MainAudioEngine::instance()->state() == AudioEngine::Stopped )
                MainAudioEngine::instance()->playItem( interface, interface->nextResult() );
            else
                MainAudioEngine::instance()->setPlaylist( interface );
        }

    }


    QList<QStringMap> entries;
    foreach (const Tomahawk::plentry_ptr& plEntry, pls->entries())
    {
        addResultsToMap(plEntry->query()->results(), entries);
    }

    QStringMap bodyArgs;
    bodyArgs["query"] = QString();  // TODO remember the last search query
    bodyArgs["playlist"] = pls->title();
    sendMultiFilePage(event, "body_playlist.html", bodyArgs, "entry_result.html", entries);
}


const bool
WebInterface::checkAuthorization(const QxtWebRequestEvent* event)
{
    const QString authString = event->headers.value("Authorization");
    const QStringList authValues = authString.split(" ");
    if (!authString.isEmpty() && authValues.count() == 2)
    {
        const QString userPasswordValue = QByteArray::fromBase64( authValues[1].toAscii() );
        const QStringList userPasswordValues = userPasswordValue.split(":");

        // Check if the password is the right one
        const QString password = userPasswordValues[1];
        if (  password.compare( TomahawkSettings::instance()->partyModePassword() ) == 0  )
            return true;
    }


    QxtWebPageEvent* wpe = new QxtWebPageEvent( event->sessionID, event->requestID, QByteArray() );
    wpe->status = 401;
    wpe->statusMessage = "Authorization Required";
    wpe->headers.insert("WWW-Authenticate", "Basic realm=\"Tomahawk\"");
    postEvent( wpe );

    return false;
}


const QString
WebInterface::getPageWithBody(const QString& bodyFile, const QStringMap& bodyArgs) const
{
    QString page = m_htmlHeader;

    const QString invertedPartyModeState = TomahawkSettings::instance()->partyModeEnabled() ? tr("Unlock") : tr("Lock");
    page.replace("<%PARTYMODE%>", invertedPartyModeState);

    const QString body = getFileContent(bodyFile);
    page.replace("<%BODY%>", body);

    foreach( const QString& param, bodyArgs.keys() )
    {
        page.replace( QString( "<%%1%>" ).arg( param.toUpper() ), bodyArgs.value( param ).toAscii() );
    }

    return page;
}

void
WebInterface::sendMultiFilePage(const QxtWebRequestEvent* event, const QString& bodyFile, const QStringMap& bodyArgs,
                                const QString& entryFile, const QList<QStringMap>& entryArgs)
{
    QString page = getPageWithBody(bodyFile, bodyArgs);


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
        }

        if (args.contains("url"))
        {
            const QString url = args.value("url");
            const QString base64URL = url.toAscii().toBase64();
            args["trackid"] = base64URL;
        }

        foreach( const QString& param, args.keys() )
        {
            entry.replace( QString( "<%%1%>" ).arg( param.toUpper() ), args.value( param ).toAscii() );
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

