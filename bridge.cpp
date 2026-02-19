#include "features/Features.h"
#include "ui/Menu.h"

void RunFeaturesAndMenu() {
  Features::run();
  g_menu.render();
}
