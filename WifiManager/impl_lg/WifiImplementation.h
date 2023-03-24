#include "dbus/DBusClient.h"

namespace WPEFramework {
    namespace Plugin {
        namespace WifiImplementation {
            void init() {
                ::WifiManager::DBusClient::getInstance().run();
            }
            void deinit() {
                ::WifiManager::DBusClient::getInstance().stop();
            }
        }
}}