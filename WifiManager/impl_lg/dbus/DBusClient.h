namespace WifiManager {

    struct EventSink {
    };

    class DBusClient {
        public:
            static DBusClient& getInstance() {
                static DBusClient client;
                return client;
            }

            void run();
            void stop();
            
            void registerEvents(EventSink *e);

            void networkconfig1_GetInterfaces();
            void networkconfig1_GetStatus();
            void wifimanagement1_GetSSIDParams();

        private:

            DBusClient() : eventSink(nullptr) {}
            ~DBusClient() = default;

            EventSink *eventSink;
    };
}