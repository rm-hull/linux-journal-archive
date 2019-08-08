#include "querythread.h"

#include "db.h"

#include <QDebug>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

// The class that does all the work with the database. This class will
// be instantiated in the thread object's run() method.
class Worker : public QObject
{
  Q_OBJECT

   public:
    Worker( QObject* parent = 0);
    ~Worker();

  public slots:
    void slotExecute( const QString& query );
 
  signals:
    void results( const QList<QSqlRecord>& records );

   private:
     QSqlDatabase m_database;
};

//

Worker::Worker( QObject* parent )
    : QObject( parent )
{
    // thread-specific connection, see db.h
    m_database = QSqlDatabase::addDatabase( DATABASE_DRIVER, 
                                            "WorkerDatabase" ); // named connection
    m_database.setDatabaseName( DATABASE_NAME );
    m_database.setHostName( DATABASE_HOST );
    m_database.setUserName( DATABASE_USER );
    m_database.setPassword( DATABASE_PASS );
    if ( !m_database.open() )
    {
        qWarning() << "Unable to connect to database, giving up:" << m_database.lastError().text();
        return;
    }

    // initialize db
    if ( !m_database.tables().contains( "item" ) )
    {
        // some data
        m_database.exec( "create table item(id int, name varchar);" );
        m_database.transaction();
        QSqlQuery query(m_database);
        query.prepare("INSERT INTO item (id, name) "
                      "VALUES (?,?)");
        for ( int i = 0; i < SAMPLE_RECORDS; ++i )
        {
            query.addBindValue(i);
            query.addBindValue(QString::number(i));
            query.exec();
        }
        m_database.commit();
    }

}

Worker::~Worker()
{
}

void Worker::slotExecute( const QString& query )
{
    QList<QSqlRecord> recs;
    QSqlQuery sql( query, m_database );
    while ( sql.next() )
    {
        recs.push_back( sql.record() );
    }
    emit results( recs );
}

////

QueryThread::QueryThread(QObject *parent)
    : QThread(parent)
{
}

QueryThread::~QueryThread()
{
    delete m_worker;
}

void QueryThread::execute( const QString& query )
{
    emit executefwd( query ); // forwards to the worker
}

void QueryThread::run()
{
    emit ready(false);
    emit progress( "QueryThread starting, one moment please..." );

    // Create worker object within the context of the new thread
    m_worker = new Worker();

    connect( this, SIGNAL( executefwd( const QString& ) ),
             m_worker, SLOT( slotExecute( const QString& ) ) );

    // Critical: register new type so that this signal can be
    // dispatched across thread boundaries by Qt using the event
    // system
    qRegisterMetaType< QList<QSqlRecord> >( "QList<QSqlRecord>" );

    // forward final signal
    connect( m_worker, SIGNAL( results( const QList<QSqlRecord>& ) ),
             this, SIGNAL( results( const QList<QSqlRecord>& ) ) );


    emit progress( "Press 'Go' to run a query." );
    emit ready(true);

    exec();  // our event loop
}

#include "querythread.moc"
