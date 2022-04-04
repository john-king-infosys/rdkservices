#pragma once

#include <mutex>
#include <libIBus.h>
#include <AbstractPluginWithApiLock.h>

namespace WPEFramework {

    namespace Plugin {

        /*
            extends AbstractPluginWithApiLock with locked_iarm_handler that can be used to synchronize iarm event handlers

            Template parameter LOCK_HOLDER specifies the class with static getApiLock() member
            that returns the std::mutex used.

        */
        template<class LOCK_HOLDER>
        class AbstractPluginWithApiAndIARMLock : public AbstractPluginWithApiLock<LOCK_HOLDER> {
        public:

            AbstractPluginWithApiAndIARMLock(const uint8_t currVersion) : AbstractPluginWithApiLock<LOCK_HOLDER>(currVersion) {}
            AbstractPluginWithApiAndIARMLock() : AbstractPluginWithApiLock<LOCK_HOLDER>() {}

            /*
                locked_iarm_handler template can be used like:

                    IARM_Bus_RegisterEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_RX_SENSE, locked_iarm_handler<DisplResolutionHandler>)

                to register mutex-protected version of given IARM handler. The mutex is obtained from LOCK_HOLDER::getApiLock, so
                can be the same that AbstractPluginWithApiLock::registerMethod uses, which allows to synchronize between IARM handlers
                and API methods invocations.

                Note that if IARM_Bus_RemoveEventHandler is used to unregister specific handlers, this wrapper needs to be used again!
                This is not necessary in case of IARM_Bus_UnRegisterEventHandler (this unregisters all handlers for given event)
            */
            template<IARM_EventHandler_t CALLBACK_FUNCTION>
            static void locked_iarm_handler(const char *owner, IARM_EventId_t eventId, void *data, size_t len) {
                std::lock_guard<std::mutex> lock(LOCK_HOLDER::getApiLock());
                // we can't be in IARM handler thread & API request thread at the same time, so
                // it's safe to use AbstractPluginWithApiLock's isThreadUsingLockedApi here
                isThreadUsingLockedApi = true;
                try {
                    CALLBACK_FUNCTION(owner, eventId, data, len);
                } catch (...) {
                    isThreadUsingLockedApi = false;
                    throw;
                }
                isThreadUsingLockedApi = false;
            }
        };
    } // Plugin
} // WPEFramework