
#ifndef __WARDEN_WIN_H
#define __WARDEN_WIN_H

#include "Warden.h"

class WorldSession;
class Warden;

class WardenWin : public Warden
{
    public:
        WardenWin();
        ~WardenWin();

        WardenModule* GetModuleForClient();

        void Init(WorldSession* pClient, BigNumber* k);
        void InitializeModule();
        void RequestHash(bool reInitialize = false);
        void HandleHashResult(ByteBuffer &buff);
        
        void RequestData();
        void RequestData2();

        std::string Penalty(WardenCheck* rd, std::string realData = "", std::string packetData = "");
        
        void HandleData(ByteBuffer &buff);
        void HandleData2(ByteBuffer &buff);
        void HandleData3(ByteBuffer &buff);

        // some helper func in memory reading
        template<class T>
        bool ReadMemChunk(ByteBuffer &buff, T &data);
        
        template <typename I>
        I GetRandomCheckFromList(I begin, I end);

        //speed checks helper functions
        float CalculateServerSpeed(WorldSession * client, MoveType mtype);
        MoveType SelectSpeedType(uint32 moveFlags);

        // movement flags checks helper functions
        std::string MovementFlagsCheck(uint32 moveFlags, std::string &reason, bool extended = false);
        std::string GetMovementFlagInfo(uint32 moveFlags);

        // controlling player movement helper functions
        void SendControlMovementPacket(uint32 opcode, uint32 moveFlags, bool remove);

        void TestInit();
        std::string GetAuraList(Player* plr);

    private:
        uint32 _serverTicks;
        std::vector<uint16> _otherChecksTodo;
        std::vector<uint16> _memChecksTodo;
        std::list<uint16> _currentChecks;
};

#endif
