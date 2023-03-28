#include "DBusClient.h"
#include "Module.h"
#include <UtilsLogging.h>
#include <UtilsFile.h>

#define NETWORK_CONFIG_DBUS_INTERFACE_NAME "com.lgi.rdk.utils.networkconfig1"
#define NETWORK_CONFIG_DBUS_INTERFACE_OBJECT_PATH "/com/lgi/rdk/utils/networkconfig1"

#define WIFI_MANAGEMENT_DBUS_INTERFACE_NAME "com.lgi.rdk.utils.wifimanagement1"
#define WIFI_MANAGEMENT_DBUS_INTERFACE_OBJECT_PATH "/com/lgi/rdk/utils/wifimanagement1"

namespace WifiManagerImpl
{
    static const std::map<std::string, InterfaceStatus> statusFromString {
        {"Disabled", Disabled},
        {"Disconnected", Disconnected},
        {"Associating", Associating},
        {"Dormant", Dormant},
        {"Binding", Binding},
        {"Assigned", Assigned},
        {"Scanning", Scanning}
    };

    DBusClient::DBusClient()
    {
        run();
    }

    void DBusClient::run()
    {
        if (!m_initialized) {
            m_loopThread = std::thread(&DBusClient::dbusWorker, this);
            m_initialized  = m_initialized_future.get_future().get();
        }
    }

    void DBusClient::stop()
    {
        throw "todo";
    }

    std::vector<std::string> DBusClient::networkconfig1_GetInterfaces()
    {
        return std::vector<std::string>();
    }

    bool DBusClient::networkconfig1_GetParam(const std::string& interface, const std::string& name, std::string& res)
    {
        res = "";
        return false;
    }

    InterfaceStatus DBusClient::networkconfig1_GetStatus(const std::string& interface)
    {
        return InterfaceStatus::Disconnected;
    }

    bool DBusClient::wifimanagement1_GetSSIDParams(const std::string &ssid, const std::string &netid, std::map<std::string,std::string> &params)
    {
        gint status = 0;
        guint count = 0;
        GVariant *out_params{nullptr};
        GError *error{nullptr};

        if (com_lgi_rdk_utils_wifimanagement1_call_get_ssidparams_sync(
                m_wifimanagement1_interface,
                ssid.c_str(),  // const gchar *arg_id,
                netid.c_str(), // const gchar *arg_netid,
                &status,
                &count,
                &out_params,
                NULL,
                &error))
        {
            if (status)
            {
                LOGERR("GetSSIDParams failed; status: %u error: '%s'", status, error ? error->message : "(unknown)");
                if (out_params)
                {
                    g_variant_unref(out_params);
                }
                return false;
            }
            GVariantIter iter;
            gchar *key;
            gchar *value;

            if (out_params)
            {
                g_variant_iter_init(&iter, out_params);
                while (g_variant_iter_loop(&iter, "{ss}", &key, &value))
                {
                    params[key] = value;
                }
            }
            if (out_params)
            {
                g_variant_unref(out_params);
            }
            return true;
        }
        else
        {
            LOGERR("Failed to call wifimanagement1_call_get_ssidparams_sync - %s", error ? error->message : "(unknown)");
            if (error)
                g_error_free(error);
            return false;
        }
    }

    void DBusClient::dbusWorker()
    {
        LOGINFO("LgiNetworkClient::Worker() TID: %u", gettid());

        m_mainContext = g_main_context_new();
        m_mainLoop = g_main_loop_new(m_mainContext, false);

        if (m_mainLoop && m_mainContext)
        {
            g_main_context_push_thread_default(m_mainContext);
            GError *error = nullptr;
            m_networkconfig1_interface = networkconfig1_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
                                                                               G_DBUS_PROXY_FLAGS_NONE,
                                                                               NETWORK_CONFIG_DBUS_INTERFACE_NAME,
                                                                               NETWORK_CONFIG_DBUS_INTERFACE_OBJECT_PATH,
                                                                               NULL, /* GCancellable */
                                                                               &error);
            if (error)
            {
                LOGERR("Failed to create networkconfig proxy: %s", error->message);
                g_error_free(error);
            }

            m_wifimanagement1_interface = com_lgi_rdk_utils_wifimanagement1_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
                                                                                                   G_DBUS_PROXY_FLAGS_NONE,
                                                                                                   WIFI_MANAGEMENT_DBUS_INTERFACE_NAME,
                                                                                                   WIFI_MANAGEMENT_DBUS_INTERFACE_OBJECT_PATH,
                                                                                                   NULL, /* GCancellable */
                                                                                                   &error);
            if (error)
            {
                LOGERR("Failed to create networkconfig proxy: %s", error->message);
                g_error_free(error);
            }

            m_initialized_future.set_value(m_networkconfig1_interface != nullptr && m_wifimanagement1_interface != nullptr);

            LOGINFO("DBusClient::dbusWorker() start main loop TID: %u", gettid());
            g_main_loop_run(m_mainLoop); // blocks
            LOGINFO("DBusClient::dbusWorker() main loop finished TID: %u", gettid());
            g_main_context_pop_thread_default(m_mainContext);
            g_main_loop_unref(m_mainLoop);
            g_main_context_unref(m_mainContext);
            m_mainLoop = nullptr;
            m_mainContext = nullptr;
        }
        else
        {
            LOGERR("Failed to create glib main loop");
            m_initialized_future.set_value(false);
        }
    }
}