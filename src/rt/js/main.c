#include <nautilus/nautilus.h>
#include <nautilus/shell.h>
#include <rt/js/config.h>


static int entry(char* buf, void* priv) {
    char* argv[] = {"js"};
    jsrt_main(1, argv);
    return 0;
}

static struct shell_cmd_impl js_impl = {
    .cmd    = "js",
    .help_str = "js",
    .handler = entry
};

nk_register_shell_cmd(js_impl);
