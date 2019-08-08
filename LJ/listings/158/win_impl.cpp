#include "win_impl.h"
#include "querythread.h"

#include <QDebug>

Win::Win( QWidget* parent )
    : QWidget(parent)
{
    m_querythread = new QueryThread();

    setupUi( this );

    // Buttons
    connect( goButton, SIGNAL( clicked() ),
             this, SLOT( slotGo() ) );
    connect( closeButton, SIGNAL( clicked() ),
             this, SLOT( close() ) );

    // Worker thread
    connect( m_querythread, SIGNAL( progress(const QString&) ),
             textBrowser, SLOT( append(const QString&) ) );
    connect( m_querythread, SIGNAL( ready(bool) ),
             goButton, SLOT( setEnabled(bool) ) );
    connect( m_querythread, SIGNAL( results( const QList<QSqlRecord>& ) ),
             this, SLOT( slotResults( const QList<QSqlRecord>& ) ) );

    // Launch worker thread
    m_querythread->start();
    
}

Win::~Win()
{
    m_querythread->quit();
    m_querythread->wait();
    delete m_querythread;
}

void Win::slotGo()
{
    textBrowser->append( "Running queries, please wait..." );
    dispatch( "select avg(id) from item;" );
    dispatch( "select name from item;" );
    dispatch( "select min(id) from item;" );
    dispatch( "select max(id) from item;" );
    dispatch( "select distinct(id) from item;" );
}

void Win::dispatch( const QString& query )
{
    textBrowser->append( "Executing:" + query );
    m_querythread->execute( query );
}


void Win::slotResults( const QList<QSqlRecord>& records )
{
    textBrowser->append( QString("Result count:%1").arg( records.count() ) );
}

