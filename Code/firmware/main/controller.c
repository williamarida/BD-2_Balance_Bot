// Adapted from Bluepad32's public-domain ESP32 example.
#include <uni.h>
#include <btstack_port_esp32.h>
#include <btstack_run_loop.h>
#include "controller.h"

static void init(int argc, const char** argv) { (void)argc; (void)argv; }
static void complete(void) {
    // Keep pairing keys across boots. Do not erase keys every startup.
    uni_bt_start_scanning_and_autoconnect_unsafe();
    uni_bt_allow_incoming_connections(true);
}
static void connected(uni_hid_device_t* d) { (void)d; }
static void disconnected(uni_hid_device_t* d) {
    (void)d;
    bd2_controller_update(false, 0, 0, 0);
}
static uni_error_t ready(uni_hid_device_t* d) {
    (void)d;
    bd2_controller_update(true, 0, 0, 0);
    return UNI_ERROR_SUCCESS;
}
static void data(uni_hid_device_t* d, uni_controller_t* ctl) {
    (void)d;
    if (ctl->klass == UNI_CONTROLLER_CLASS_GAMEPAD)
        bd2_controller_update(true, ctl->gamepad.axis_x,
                              ctl->gamepad.axis_y, ctl->gamepad.buttons);
}
static const uni_property_t* property(uni_property_idx_t idx) { (void)idx; return NULL; }
static void oob(uni_platform_oob_event_t event, void* data) { (void)event; (void)data; }
void bd2_controller_start(void) {
    static struct uni_platform platform = {
        .name = "bd2", .init = init, .on_init_complete = complete,
        .on_device_connected = connected, .on_device_disconnected = disconnected,
        .on_device_ready = ready, .on_controller_data = data,
        .on_oob_event = oob, .get_property = property,
    };
    btstack_init();
    uni_platform_set_custom(&platform);
    uni_init(0, NULL);
    btstack_run_loop_execute();
}
