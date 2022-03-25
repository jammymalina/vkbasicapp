
#include "./lib/app/app.h"

int main(int argc, char* args[]) {
    App app = app_create();
    app_init(&app);

    if (!app_is_init(&app)) {
        return 1;
    }

    app_start(&app);

    app_destroy(&app);

    return 0;
}
