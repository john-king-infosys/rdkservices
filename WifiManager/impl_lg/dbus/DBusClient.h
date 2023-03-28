#pragma once

#include <thread>
#include <future>
#include <string>
#include <vector>
#include <mutex>
#include <functional>
#include <map>

#include <gio/gio.h>
#include <glib.h>
#include "networkconfig1_dbus_api.h"
#include "wifimanagement1_dbus_api.h"

namespace WifiManagerImpl {

    enum InterfaceStatus {
        Disabled,
        Disconnected,
        Associating,
        Dormant,
        Binding,
        Assigned,
        Scanning
    };

    class DBusClient {
        public:

            using StatusChangedHandler = std::function<void(const std::string& interface, InterfaceStatus state)>;

            static DBusClient& getInstance() {
                static DBusClient client;
                return client;
            }

            void run();
            void stop();
            
            void registerStatusChanged(StatusChangedHandler handler);

            std::vector<std::string> networkconfig1_GetInterfaces();
            bool networkconfig1_GetParam(const std::string& interface, const std::string& name, std::string& res);
            InterfaceStatus networkconfig1_GetStatus(const std::string& interface);
            bool wifimanagement1_GetSSIDParams(const std::string &ssid, const std::string &netid, std::map<std::string,std::string> &params);

        private:

            DBusClient();
            ~DBusClient() = default;

            void dbusWorker();

            std::thread m_loopThread;
            std::promise<bool> m_initialized_future;
            bool m_initialized {false};
            GMainContext *m_mainContext {nullptr};
            GMainLoop *m_mainLoop {nullptr};
            Networkconfig1 *m_networkconfig1_interface {nullptr};
            Wifimanagement1 *m_wifimanagement1_interface {nullptr};
            StatusChangedHandler statusChangedHandler {nullptr};

            std::mutex lock;
    };
}