#include "khellotw.h"

int main (int argc, char *argv[])
{

  KApplication app (argc, argv, "khello");
  KHelloTW hello;

  hello.show();

  return app.exec();
}
