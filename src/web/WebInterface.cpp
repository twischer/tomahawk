#include "web/WebInterface.h"
#include "ViewManager.h"
#include "playlist/PlaylistView.h"
#include "audio/MainAudioEngine.h"

WebInterface::WebInterface(QxtAbstractWebSessionManager* sm)
    : Api_v1(sm),
      m_htmlHeader("<html>\n"
                   "<head>\n"
                   "<title>Tomahawk-Player</title>\n"
                   "</head>\n"
                   "<body>"
                   "<form action=\"search\" method=\"get\">\n"
                   "<input type=\"text\" name=\"query\" value=\"<%QUERY%>\"/>\n"
                   "<input type=\"submit\" value=\"Search\"/>\n"
                   "</form>\n"
                   "<%BODY%>\n"
                   "</body>\n"
                   "</html>\n"),
      m_htmlStates("<h2>Current playing song</h2>\n"
                   "<%ARTIST%> - <%TRACK%>\n"
                   "<h2>Next songs</h2>\n"
                   "<table>"
                   "<%QUEUE%>\n"
                   "</table>"),
      m_htmlQueue("<tr>\n"
                   "<td><%ARTIST%></td>\n"
                   "<td>- <%TRACK%></td>\n"
                   "<td><%ALBUM%></td>\n"
                   "</tr>\n"),
      m_htmlSearch("<h1>Found Songs</h1>\n"
                   "<table>"
                   "<%RESULT%>"
                   "</table>"),
      m_htmlResult("<tr>\n"
                   "<form action=\"add\" method=\"get\">\n"
                   "<td><input type=\"text\" name=\"artist\" value=\"<%ARTIST%>\" readonly/></td>\n"
                   "<td><input type=\"text\" name=\"track\" value=\"<%TRACK%>\" readonly/></td>\n"
                   "<td><input type=\"text\" name=\"album\" value=\"<%ALBUM%>\" readonly/></td>\n"
                   "<td><input type=submit value=\"Add\"/></td>\n"
                   "</form>\n"
                   "</tr>\n")
{
}


void
WebInterface::index( QxtWebRequestEvent* event )
{
    QString page = m_htmlHeader;
    page.replace("<%QUERY%>", "");
    page.replace("<%BODY%>", m_htmlStates);

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

        const Tomahawk::WebSearchQuery* qry = new Tomahawk::WebSearchQuery(query, event);
        connect( qry, SIGNAL( searchFinished( const QString, const QList<Tomahawk::result_ptr>, const QxtWebRequestEvent* ) ),
                 SLOT( onSearchFinished( const QString, const QList<Tomahawk::result_ptr>, const QxtWebRequestEvent* ) ) );
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
            // TODDO use one base64 string for idenfikation
            // und damit keine text boxen verwenden
            // zusätzlich mit einem href arbeiten
            // spart übertragungsraum und ist beim rendern aud smartphones weniger aufwendig

            QString resultLine = m_htmlResult;
            replaceTrackInformation(rp, resultLine);

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
    const QUrl& url = event->url;
    if ( url.hasQueryItem("artist") && url.hasQueryItem("track") && url.hasQueryItem("album") )
    {
        const QString artist = getDecodedURLAttribute(url, "artist");
        const QString track = getDecodedURLAttribute(url, "track");
        const QString album = getDecodedURLAttribute(url, "album");

        // add track to the queue
        const Tomahawk::query_ptr query = Tomahawk::Query::get(artist, track, album);
        ViewManager::instance()->queue()->model()->appendQuery(query);

        // TODO PlaylistModel::playlistToFull abfangen und
        // nachricht an benutzer weiter geben
    }

    index(event);
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
    toReplace.replace( "<%ALBUM%>", track->album()->name() );
}
