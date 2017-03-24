
#ifndef __WARDEN_H
#define __WARDEN_H

#include "Anticheat.h"
#include "Policies/Singleton.h"
#include "WardenKeyGeneration.h"
#include "Auth/AuthCrypt.h"
#include "Common.h"
#include "ByteBuffer.h"
#include "WorldSession.h"
#include "Auth/BigNumber.h"
#include "Auth/SARC4.h"
#include "WardenStorageMgr.h"

enum WardenCheckType
{
    MEM_CHECK               = 0xF3, // 243: byte moduleNameIndex + uint Offset + byte Len (check to ensure memory isn't modified)
    PAGE_CHECK_A            = 0xB2, // 178: uint Seed + byte[20] SHA1 + uint Addr + byte Len (scans all pages for specified hash)
    PAGE_CHECK_B            = 0xBF, // 191: uint Seed + byte[20] SHA1 + uint Addr + byte Len (scans only pages starts with MZ+PE headers for specified hash)
    MPQ_CHECK               = 0x98, // 152: byte fileNameIndex (check to ensure MPQ file isn't modified)
    LUA_STR_CHECK           = 0x8B, // 139: byte luaNameIndex (check to ensure LUA string isn't used)
    DRIVER_CHECK            = 0x71, // 113: uint Seed + byte[20] SHA1 + byte driverNameIndex (check to ensure driver isn't loaded)
    TIMING_CHECK            = 0x57, //  87: empty (check to ensure GetTickCount() isn't detoured)
    PROC_CHECK              = 0x7E, // 126: uint Seed + byte[20] SHA1 + byte moluleNameIndex + byte procNameIndex + uint Offset + byte Len (check to ensure proc isn't detoured)
    MODULE_CHECK            = 0xD9, // 217: uint Seed + byte[20] SHA1 (check to ensure module isn't injected)

    INTERNAL_CHECK          = 0x00,
};

struct WardenModule
{
    uint8 ID[16];
    uint8 Key[16];
    //uint8 ClientKeySeed[16];
    //uint8 ServerKeySeed[16];
    //uint8 Seed[16];
    //RC4_Context ClientRC4State;
    //RC4_Context ServerRC4State;
    uint8* CompressedData;
    uint32 CompressedSize;
};

enum MoveType
{
    MOVETYPE_WALK           = 0,
    MOVETYPE_RUN            = 1,
    MOVETYPE_RUN_BACK       = 2,
    MOVETYPE_SWIM           = 3,
    MOVETYPE_SWIM_BACK      = 4,
    MOVETYPE_TURN_RATE      = 5,
    MOVETYPE_NONE           = 255
};

enum TrackCreatureMask
{
    TRACK_BEASTS     = 0x01,
    TRACK_DRAGONS    = 0x02,
    TRACK_DEMONS     = 0x04,
    TRACK_ELEMENTALS = 0x08,
    TRACK_GIANTS     = 0x10,
    TRACK_UNDEAD     = 0x20,
    TRACK_HUMANS     = 0x40,
    TRACK_ALL        = 0xFF
};

enum TrackResourceMask
{
    TRACK_HERBS     = 0x02,
    TRACK_MINERALS  = 0x04,
    TRACK_TREASURES = 0x10
};

#if defined(__GNUC__)
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

struct WardenModuleUse
{
    uint8 Command;
    uint8 ModuleId[16];
    uint8 ModuleKey[16];
    uint32 Size;
};

struct WardenModuleCache
{
    uint8 Command;
    uint16 DataSize;
    uint8 Data[500];
};

struct WardenHashRequest
{
    uint8 Command;
    uint8 Seed[16];
};

#if defined(__GNUC__)
#pragma pack()
#else
#pragma pack(pop)
#endif

class WorldSession;

class Warden : public WardenInterface
{
    friend class WardenWin;
    friend class WardenMac;

    public:
        Warden();
        virtual ~Warden();

        void HandleWardenDataOpcode(WorldPacket & recvData);

        void RequestModule();
        void SendModuleToClient();
        void Update();
        void Reset();

        virtual void Init(WorldSession *pClient, BigNumber *K) {}
        virtual void InitializeModule() {}
        virtual void RequestHash(bool reInitialize = false) {}
        virtual void HandleHashResult(ByteBuffer &buff) {}
        virtual void RequestData() {}
        virtual void RequestData2() {}
        virtual void HandleData(ByteBuffer &buff) {}
        virtual void HandleData2(ByteBuffer &buff) {}
        virtual void HandleData3(ByteBuffer &buff) {}
        virtual void TestInit() {}

        void EncryptData(uint8* buffer, uint32 length);
        void DecryptData(uint8* buffer, uint32 length);

        uint32 BuildChecksum(const uint8* data, uint32 dataLen);
        bool IsValidCheckSum(uint32 checksum, const uint8* data, const uint16 length);

        std::string ConvertPacketDataToString(const uint8 * packet_data, uint16 length);

        void SendPacket(ByteBuffer &data);

        void ClearAlerts();
        void ClearOffsets();

        uint32 GetCurrentModuleOffset() { return s_currentModule; }
        uint32 GetRandCheckTimer();

    private:
        WorldSession* _session;
        WardenModule* _module;
        uint32 clientBuild;
        bool _initialized;
        uint8 _clientKeySeed[16];
        uint8 _serverKeySeed[16];
        uint8 _moduleSeed[16];
        SARC4 _inputCrypto;
        SARC4 _outputCrypto;
        uint32 _checkTimer;
        uint32 _checkTimer2;
        uint32 _lastTimestamp;

        uint32 playerBase;
        uint32 offset;
        uint32 playerMovementBase;
        uint32 s_currentModulePtr;
        uint32 s_currentModule;

        // alerts for dynamic checks
        int8 m_currentSpeedHackAlert;
        int8 m_baseSpeedHackAlert;
        //int8 m_moveFlagsHackAlert;

        // test IMPORTANT system of sync
        uint32 m_synIndex;
        uint32 m_ackIndex;
        uint32 m_reqAckIndex;

        uint8 m_sendLastPacketCount;
        uint8 m_sendLastPacketCount2;

        uint8 m_failedSyncPacketCount;

        uint8 m_memChecksCount;
        uint8 m_otherChecksCount;
};

#endif
