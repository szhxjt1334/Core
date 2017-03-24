
#ifndef _WARDEN_MAC_H
#define _WARDEN_MAC_H

#include "Warden.h"

class WorldSession;
class Warden;

class WardenMac : public Warden
{
public:
    WardenMac();
    ~WardenMac();

    WardenModule* GetModuleForClient();

    void Init(WorldSession* pClient, BigNumber* k);
    void RequestHash();
};

#endif
