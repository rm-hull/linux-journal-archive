#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <QList>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QString>

class Worker; // forw decl

class QueryThread : public QThread
{
  Q_OBJECT

  public: 
    QueryThread(QObject *parent = 0);
    ~QueryThread();

    void execute( const QString& query );
 
  signals:
    void progress( const QString& msg );
    void ready(bool);
    void results( const QList<QSqlRecord>& records );

  protected:
    void run();

  signals:
    void executefwd( const QString& query );

  private:
     Worker* m_worker;
};

#endif
