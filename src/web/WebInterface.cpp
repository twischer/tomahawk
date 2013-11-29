#include "web/WebInterface.h"
#include "ViewManager.h"
#include "playlist/PlaylistView.h"

WebInterface::WebInterface(QxtAbstractWebSessionManager* sm)
    : Api_v1(sm),
      m_htmlHeader(
                   "<h1>Tomahawk-Player</h1>\n"
                   "<form name=\"input\" action=\"search\" method=\"get\">\n"
                   "<input type=\"text\" name=\"query\"/>\n"
                   "<input type=\"submit\" value=\"Search\"/>\n"
                   "</form>\n"
                   "<%BODY%>\n"),
      m_htmlStates("<h2>Current playing song</h2>\n"
                   "TODO\n"
                   "<h2>Next songs</h2>\n"
                   "TODO\n"),
      m_htmlSearch("<h1>Found Songs</h1>\n"
                   "<h2>for <%QUERY%></h2>\n"
                   "<table>"
                   "<%RESULT%>"
                   "</table>"),
      m_htmlResult("<tr>\n"
                   "<td><input type=button value=\"Add\"/></td><td><%TRACK%></td>\n"
                   "<tr>\n")
{
}


void
WebInterface::index( QxtWebRequestEvent* event )
{
    QString page = m_htmlHeader;
    page.replace("<%BODY%>", m_htmlStates);

    QxtWebPageEvent* wpe = new QxtWebPageEvent( event->sessionID, event->requestID, page.toUtf8() );
    postEvent( wpe );
}


void
WebInterface::search( QxtWebRequestEvent* event )
{
    if ( event->url.hasQueryItem( "query" ) )
    {
        const QString encodedQuery = event->url.queryItemValue("query");
        const QString query = QUrl::fromPercentEncoding( encodedQuery.toUtf8() ).replace("+", " ");

        const Tomahawk::WebSearchQuery* qry = new Tomahawk::WebSearchQuery("fury hello and goodbye", event);
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
            QString trackName = rp->artist()->name();
            trackName += " - ";
            trackName += rp->track();

            QString resultLine = m_htmlResult;
            resultLine.replace("<%TRACK%>", trackName);

            resultString += resultLine;
        }
    }


    QString page = m_htmlHeader;
    page.replace( "<%BODY%>", m_htmlSearch );

    page.replace( "<%QUERY%>", query );
    page.replace( "<%RESULT%>", resultString );

    QxtWebPageEvent* wpe = new QxtWebPageEvent( event->sessionID, event->requestID, page.toUtf8() );
    postEvent( wpe );



    // TODO
    Tomahawk::result_ptr firstResult = results.first();

    //Tomahawk::query_ptr qry = Tomahawk::Query::get( firstResult->artist()->name(), firstResult->track(), firstResult->album()->name(), firstResult->)

    ViewManager::instance()->queue()->model()->appendQuery( firstResult->toQuery() );
    // RID oder trackid
    // Result::get( url ) vielleicht verwenden
    // alternativ mal nach erbenen von Result suchen und gucken wo trackid gestezt wird



}
