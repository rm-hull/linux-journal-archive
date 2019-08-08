//#include <qpopmenu.h>
//#include <qlabel.h>

#include <kapp.h>
#include <ktopwidget.h>
#include <ktoolbar.h>
#include <kmenubar.h>
#include <drag.h>

class KHelloTW : public KTopLevelWidget
{
  Q_OBJECT;

public:

  KHelloTW (void);

  void closeEvent (QCloseEvent *);

private:
  QPopupMenu *file, *help;
  KToolBar *toolbar;
  KMenuBar *menubar;
  QLabel *label;
  KDNDDropZone *dropzone;

public  slots:
  void slotQuit();
/** 
 * This method is no longer needed. 
 *  void slotAbout();
 **/

  void slotHello();
  void slotDropped(KDNDDropZone *);

};







