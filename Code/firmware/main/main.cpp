// Bench diagnostics only: no motor, servo, lights, or buzzer outputs.
#include <cmath>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "lwip/sockets.h"
#include "cJSON.h"
#include "controller.h"
#ifdef BD2_WIFI_CONFIGURED
#include "wifi_credentials.h"
#else
#define BD2_WIFI_SSID ""
#define BD2_WIFI_PASSWORD ""
#endif

namespace {
constexpr int port = 4242;
constexpr char tag[] = "bd2";
portMUX_TYPE lock = portMUX_INITIALIZER_UNLOCKED;
struct Input { bool connected; int32_t x; int32_t y; uint16_t buttons; } input{};
struct Settings { float kp = 0; float deadzone = 0.08f; float scale = 0.25f; } settings;
nvs_handle_t storage;

void wifi_event(void*, esp_event_base_t base, int32_t id, void* payload) {
    if (base == WIFI_EVENT && (id == WIFI_EVENT_STA_START || id == WIFI_EVENT_STA_DISCONNECTED))
        esp_wifi_connect();
    if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        auto* event = static_cast<ip_event_got_ip_t*>(payload);
        ESP_LOGI(tag, "Wi-Fi address: " IPSTR "; UDP port %d", IP2STR(&event->ip_info.ip), port);
    }
}
void wifi_start() {
    if (std::strlen(BD2_WIFI_SSID) == 0) {
        ESP_LOGI(tag, "No Wi-Fi credentials configured. Bluetooth and serial diagnostics available.");
        return;
    }
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event, nullptr));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event, nullptr));
    wifi_config_t config{};
    std::snprintf(reinterpret_cast<char*>(config.sta.ssid), sizeof(config.sta.ssid), "%s", BD2_WIFI_SSID);
    std::snprintf(reinterpret_cast<char*>(config.sta.password), sizeof(config.sta.password), "%s", BD2_WIFI_PASSWORD);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

cJSON* number(float value) { return cJSON_CreateNumber(value); }
void server(void*) {
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    sockaddr_in address{}; address.sin_family = AF_INET;
    address.sin_port = htons(port); address.sin_addr.s_addr = htonl(INADDR_ANY);
    if (fd < 0 || bind(fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
        ESP_LOGE(tag, "UDP socket failed"); if (fd >= 0) close(fd); vTaskDelete(nullptr); return;
    }
    char buffer[1024];
    for (;;) {
        sockaddr_in peer{}; socklen_t length = sizeof(peer);
        int received = recvfrom(fd, buffer, sizeof(buffer)-1, 0, reinterpret_cast<sockaddr*>(&peer), &length);
        if (received <= 0) continue;
        buffer[received] = '\0';
        cJSON* request = cJSON_Parse(buffer);
        const cJSON* operation = cJSON_GetObjectItemCaseSensitive(request, "op");
        bool ok = false;
        const char* message = "invalid operation";
        if (cJSON_IsString(operation)) {
            if (std::strcmp(operation->valuestring, "get") == 0) { ok = true; message = "ok"; }
            else if (std::strcmp(operation->valuestring, "set") == 0) {
                const cJSON* key = cJSON_GetObjectItemCaseSensitive(request, "key");
                const cJSON* value = cJSON_GetObjectItemCaseSensitive(request, "value");
                if (cJSON_IsString(key) && cJSON_IsNumber(value) && std::isfinite(value->valuedouble)) {
                    double n = value->valuedouble;
                    if (std::strcmp(key->valuestring, "gain_kp") == 0 && n >= 0 && n <= 1000) { settings.kp = n; ok = true; }
                    if (std::strcmp(key->valuestring, "stick_deadzone") == 0 && n >= 0 && n <= 0.5) { settings.deadzone = n; ok = true; }
                    if (std::strcmp(key->valuestring, "stick_scale") == 0 && n >= 0 && n <= 1) { settings.scale = n; ok = true; }
                }
                message = ok ? "applied in RAM" : "unknown key or out-of-range value";
            } else if (std::strcmp(operation->valuestring, "save") == 0) {
                ok = nvs_set_blob(storage, "settings_v1", &settings, sizeof(settings)) == ESP_OK && nvs_commit(storage) == ESP_OK;
                message = ok ? "saved" : "save failed";
            }
        }
        Input snapshot;
        portENTER_CRITICAL(&lock); snapshot = input; portEXIT_CRITICAL(&lock);
        cJSON* reply = cJSON_CreateObject();
        cJSON_AddBoolToObject(reply, "ok", ok);
        const cJSON* request_id = cJSON_GetObjectItemCaseSensitive(request, "request_id");
        if (cJSON_IsString(request_id)) cJSON_AddStringToObject(reply, "request_id", request_id->valuestring);
        cJSON_AddStringToObject(reply, "message", message);
        cJSON_AddStringToObject(reply, "mode", "bench_diagnostics_no_actuators");
        cJSON_AddNumberToObject(reply, "uptime_s", esp_timer_get_time()/1000000.0);
        cJSON_AddBoolToObject(reply, "controller_connected", snapshot.connected);
        cJSON_AddNumberToObject(reply, "stick_x", snapshot.x);
        cJSON_AddNumberToObject(reply, "stick_y", snapshot.y);
        cJSON_AddNumberToObject(reply, "buttons", snapshot.buttons);
        cJSON* config = cJSON_AddObjectToObject(reply, "settings");
        cJSON_AddItemToObject(config, "gain_kp", number(settings.kp));
        cJSON_AddItemToObject(config, "stick_deadzone", number(settings.deadzone));
        cJSON_AddItemToObject(config, "stick_scale", number(settings.scale));
        char* text = cJSON_PrintUnformatted(reply);
        if (text) { sendto(fd, text, std::strlen(text), 0, reinterpret_cast<sockaddr*>(&peer), length); cJSON_free(text); }
        cJSON_Delete(reply); cJSON_Delete(request);
    }
}
}

extern "C" void bd2_controller_update(bool connected, int32_t x, int32_t y, uint16_t buttons) {
    portENTER_CRITICAL(&lock); input = {connected, x, y, buttons}; portEXIT_CRITICAL(&lock);
}
extern "C" void app_main(void) {
    esp_err_t result = nvs_flash_init();
    if (result == ESP_ERR_NVS_NO_FREE_PAGES || result == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase()); result = nvs_flash_init();
    }
    ESP_ERROR_CHECK(result);
    ESP_ERROR_CHECK(nvs_open("bd2", NVS_READWRITE, &storage));
    size_t size = sizeof(settings);
    Settings saved;
    if (nvs_get_blob(storage, "settings_v1", &saved, &size) == ESP_OK && size == sizeof(saved)
        && std::isfinite(saved.kp) && saved.kp >= 0 && saved.kp <= 1000
        && std::isfinite(saved.deadzone) && saved.deadzone >= 0 && saved.deadzone <= 0.5
        && std::isfinite(saved.scale) && saved.scale >= 0 && saved.scale <= 1) settings = saved;
    ESP_LOGI(tag, "BD-2 bench diagnostics: no actuator outputs. ESP32 booted.");
    wifi_start();
    if (std::strlen(BD2_WIFI_SSID)) {
        if (xTaskCreate(server, "bd2_udp", 6144, nullptr, 3, nullptr) != pdPASS)
            ESP_LOGE(tag, "Could not create telemetry task");
    }
    bd2_controller_start();
}
