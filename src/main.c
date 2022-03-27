
#include "./lib/app/app.h"

int main(int argc, char* args[]) {
    App app = {0};
    app_clear(&app);
    app_init(&app);

    int error_code = app_start(&app);

    app_destroy(&app);

    return error_code;
}
