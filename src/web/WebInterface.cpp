#include "web/WebInterface.h"
#include "ViewManager.h"
#include "playlist/PlaylistView.h"
#include "audio/MainAudioEngine.h"

WebInterface::WebInterface(QxtAbstractWebSessionManager* sm)
    : Api_v1(sm),
      m_htmlHeader( getFileContent("header.html") ),
      m_htmlState( getFileContent("state.html") ),
      m_htmlQueue( getFileContent("queue.html") ),
      m_htmlSearch( getFileContent("search.html") ),
      m_htmlResult( getFileContent("result.html") )
{
}


const QString
WebInterface::getFileContent(const QString& filename)
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
    QString page = m_htmlHeader;
    page.replace("<%QUERY%>", "");
    page.replace("<%BODY%>", m_htmlState);

    const Tomahawk::result_ptr currentTrack = MainAudioEngine::instance()->currentTrack();
    if (currentTrack)
    {
        replaceTrackInformation(currentTrack, page);
    }


    QString queueString;
    const QList<Tomahawk::query_ptr> queries = ViewManager::instance()->queue()->model()->queries();
    foreach( const Tomahawk::query_ptr& query, queries )
    {

        foreach( const Tomahawk::result_ptr& rp, query->results() )
        {
            QString queueLine = m_htmlQueue;
            replaceTrackInformation(rp, queueLine);
            queueString += queueLine;
        }
    }
    page.replace("<%QUEUE%>", queueString);


    QxtWebPageEvent* wpe = new QxtWebPageEvent( event->sessionID, event->requestID, page.toAscii() );
    postEvent( wpe );
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
    QString resultString;
    foreach( const Tomahawk::result_ptr& rp, results )
    {
        if ( rp->isOnline() )
        {
            QString resultLine = m_htmlResult;
            replaceTrackInformation(rp, resultLine);

            const QString trackID = rp->artist()->name() + QString("|")
                    + rp->track() + QString("|")
                    + rp->album()->name();

            const QString base64TrackID = trackID.toAscii().toBase64();
            resultLine.replace("<%TRACKID%>", base64TrackID);

            resultString += resultLine;
        }
    }


    QString page = m_htmlHeader;
    page.replace("<%QUERY%>", query);

    page.replace( "<%BODY%>", m_htmlSearch );
    page.replace( "<%RESULT%>", resultString );

    QxtWebPageEvent* wpe = new QxtWebPageEvent( event->sessionID, event->requestID, page.toAscii() );
    postEvent( wpe );
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

    QString page = getFileContent("message.html");
    page.replace("<%MESSAGE%>", message);

    QxtWebPageEvent* wpe = new QxtWebPageEvent( event->sessionID, event->requestID, page.toAscii() );
    postEvent( wpe );
}


const QString
WebInterface::getDecodedURLAttribute(const QUrl& url, const QString& key)
{
    const QString encodedValue = url.queryItemValue(key);
    QString value = QUrl::fromPercentEncoding( encodedValue.toAscii() );
    value.replace("+", " ");

    return value;
}


void
WebInterface::replaceTrackInformation(const Tomahawk::result_ptr& track, QString& toReplace)
{
    toReplace.replace( "<%ARTIST%>", track->artist()->name() );
    toReplace.replace( "<%TRACK%>", track->track() );

    // Add seperator "- " if the album is not empty
    const QString album = track->album()->name();
    const QString albumWithSeperator = album.trimmed().isEmpty() ? "" : QString("- %1").arg(album);
    toReplace.replace("<%ALBUM%>", albumWithSeperator);
}
