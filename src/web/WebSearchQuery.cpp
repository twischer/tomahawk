#include "WebSearchQuery.h"

namespace Tomahawk {

WebSearchQuery::WebSearchQuery(const QString& query, const QxtWebRequestEvent* event) :
    QObject(),
    m_query(query),
    m_event(event)
{
    Tomahawk::query_ptr qry = Tomahawk::Query::get( query, uuid() );
    connect( qry.data(), SIGNAL( resolvingFinished( const QList<Tomahawk::result_ptr> ) ), SLOT( onSearchFinished( const QList<Tomahawk::result_ptr> ) ) );
}


WebSearchQuery::~WebSearchQuery(void)
{
    // only needed to make the dstructor private
    // because the object makes self destroing
}


void
WebSearchQuery::onSearchFinished( const QList<Tomahawk::result_ptr> results )
{
    emit searchFinished( m_query, results, m_event );

    // object no longer needed so destroy it
    delete this;
}


} // namespace Tomahawk
