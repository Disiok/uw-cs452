#include <bwio.h>
#include <io.h>
#include <terminal.h>


void tc_init(TerminalController *controller, SmartTerminal *st) {
    controller->st = st;
}


int tc_process_terminal_input(TerminalController *controller) {
    return st_process_terminal_input(controller->st, controller);
}

int tc_process_time(TerminalController *controller, Clock *clock) {
    if (cl_time_changed(clock)) {
        tc_update_time(controller, cl_get_time_ms(clock));
    }
    return 0;
}

int tc_update_command(TerminalController *controller, char *command) {

}

int tc_update_time(TerminalController *controller, long time_ms) {
    st_save(controller->st);
    st_move_top_left(controller->st);
    int time = time_ms;
    int ms = time % 1000;
    time /= 1000;
    int s = time % 60;
    time /= 60;
    int m = time % 60;
    int h = time / 60;

    printf(&(controller->st->channel), "%d:%d:%d:%d\r\n", h, m, s, ms);

    st_restore(controller->st);

    return 0;

}

int tc_tr( int train_number, int train_speed ) {
    return 0;
}

int tc_rv( int train_number ) {
    return 0;
}

int tc_sw( int switch_number, int switch_direction) {
    return 0;
}





