
#ifndef _WARDENSTORAGEMGR_H
#define _WARDENSTORAGEMGR_H

#include "Policies/Singleton.h"
#include "Common.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Auth/BigNumber.h"

enum CheckClasses
{
    CHECK_CLASS_GENERIC    = 0,
    CHECK_CLASS_SURVEY     = 1,
    CHECK_CLASS_SYSTEM     = 2,
    CHECK_CLASS_API        = 3,
    CHECK_CLASS_DYNAMIC    = 4
};

struct SecurityData
{
    BigNumber data[8];
};

struct WardenCheck
{
    uint16 checkId;
    uint8 groupId;
    uint8 type;
    BigNumber data;
    uint32 address;
    uint32 length;
    std::string str;
    // 0 - log, 1 - kick, 2 - ban
    uint8 action;
    uint32 banTime;
    std::string reason;
};

struct WardenCheckResult
{
    uint16 checkId;
    uint8 groupId;
    std::string result;
    std::string result2;
};

class WardenStorageMgr
{
    public:
        WardenStorageMgr();
        ~WardenStorageMgr();

        typedef std::map<uint16, std::vector<WardenCheck*> > CheckContainer;
        typedef std::map<uint16, std::vector<WardenCheckResult*> > CheckResultContainer;

        WardenCheck* GetCheckDataById(uint16 Id, uint16 build, bool usesGroup = false);
        WardenCheckResult* GetCheckResultById(uint16 Id, uint16 build, bool usesGroup = false);

        std::map<uint16, std::vector<uint16> > MemChecksIdPool;
        std::map<uint16, std::vector<uint16> > OtherChecksIdPool;

        void LoadWardenChecks();
        bool LoadSecurityData();
        void LoadWardenPenalty();

        template <typename I>
        I GetRandomCheck(I begin, I end);

        ACE_RW_Mutex _checkStoreLock;

        //void GenerateSecurityData(SecurityData * ss_data, std::string data[]) ;

    private:
        CheckContainer checkStore;
        CheckResultContainer checkResultStore;
};

#define _wardenStorage MaNGOS::Singleton<WardenStorageMgr>::Instance()

#endif
