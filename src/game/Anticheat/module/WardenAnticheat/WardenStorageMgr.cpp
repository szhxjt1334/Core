
#include "Common.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "World.h"
#include "Log.h"
#include "Database/DatabaseEnv.h"
#include "Policies/SingletonImp.h"
#include "Auth/BigNumber.h"
#include "WardenStorageMgr.h"
#include "Warden.h"
#include "Util.h"

INSTANTIATE_SINGLETON_1( WardenStorageMgr );

WardenStorageMgr::WardenStorageMgr()
{
    checkStore.clear();
    checkResultStore.clear();
}

WardenStorageMgr::~WardenStorageMgr()
{
    for (CheckContainer::const_iterator itr = checkStore.begin(); itr != checkStore.end(); ++itr)
    {
        std::vector<WardenCheck*> checkGroup = itr->second;
        for (std::vector<WardenCheck*>::const_iterator itr2 = checkGroup.begin(); itr2 != checkGroup.end(); ++itr2)
            delete (*itr2);
    }

    for (CheckResultContainer::const_iterator itr = checkResultStore.begin(); itr != checkResultStore.end(); ++itr)
    {
        std::vector<WardenCheckResult*> checkResultGroup = itr->second;
        for (std::vector<WardenCheckResult*>::const_iterator itr2 = checkResultGroup.begin(); itr2 != checkResultGroup.end(); ++itr2)
            delete (*itr2);
    }

    checkStore.clear();
    checkResultStore.clear();
}

void WardenStorageMgr::LoadWardenChecks()
{
    // Check if Warden is enabled by config before loading anything
    //if (!sWorld.getConfig(CONFIG_BOOL_WARDEN_ENABLE))
    //{
        //sLog.outString(">> Warden disabled, loading checks skipped.");
        //sLog.outString();
        //return;
    //}

    /*if (!LoadSecurityData())
    {
        sLog.outString(">> Security data loading failed, loading checks skipped.");
        sLog.outString();
        return;
    }*/

    QueryResult *result = LoginDatabase.Query("SELECT MAX(id) FROM warden_checks");

    if (!result)
    {
        sLog.outString(">> Loaded 0 Warden checks. DB table `warden_checks` is empty!");
        sLog.outString();
        return;
    }

    checkStore.clear();
    checkResultStore.clear();

    result = LoginDatabase.Query("SELECT id, groupId, type, str, data, address, length, result, result2, build FROM warden_checks ORDER BY id ASC");

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint16 id               = fields[0].GetUInt16();
        uint8 groupId           = fields[1].GetUInt8();
        uint8 checkType         = fields[2].GetUInt8();
        std::string str         = fields[3].GetCppString();
        std::string data        = fields[4].GetCppString();
        uint32 address          = fields[5].GetUInt32();
        uint32 length           = fields[6].GetUInt32();
        std::string checkResult = fields[7].GetCppString();
        std::string checkExResult = fields[8].GetCppString();
        uint16 build            = fields[9].GetUInt16();

        WardenCheck* wardenCheck = new WardenCheck();
        wardenCheck->type = checkType;
        wardenCheck->checkId = id;
        wardenCheck->groupId = groupId;
        //
        wardenCheck->action = 2;
        wardenCheck->reason = "";
        wardenCheck->banTime = WEEK;

        if (checkType == PAGE_CHECK_A || checkType == PAGE_CHECK_B || checkType == DRIVER_CHECK)
        {
            wardenCheck->data.SetHexStr(data.c_str());
            int len = data.size() / 2;

            if (wardenCheck->data.GetNumBytes() < len)
            {
                uint8 temp[24];
                memset(temp, 0, len); 
                auto wData = wardenCheck->data.AsByteArray();
                memcpy(temp, wData.data(), wData.size());
                wardenCheck->data.SetBinary((uint8*)temp, len);
            }
        }

        if (checkType == MEM_CHECK || checkType == MPQ_CHECK)
            MemChecksIdPool[build].push_back(id);
        else if (checkType != INTERNAL_CHECK)
            OtherChecksIdPool[build].push_back(id);

        if (checkType == MEM_CHECK || checkType == PAGE_CHECK_A || checkType == PAGE_CHECK_B || checkType == PROC_CHECK || checkType == INTERNAL_CHECK)
        {
            wardenCheck->address = address;
            wardenCheck->length = length;
        }

        // PROC_CHECK support missing
        if (checkType == MEM_CHECK || checkType == MPQ_CHECK || checkType == LUA_STR_CHECK || checkType == DRIVER_CHECK || checkType == MODULE_CHECK)
            wardenCheck->str = str;

        checkStore[build].push_back(wardenCheck);

        if (checkType == MPQ_CHECK || checkType == MEM_CHECK || checkType == PAGE_CHECK_A || checkType == PAGE_CHECK_B
            || checkType == DRIVER_CHECK || checkType == MODULE_CHECK || checkType == INTERNAL_CHECK)
        {
            WardenCheckResult *wr = new WardenCheckResult();
            wr->checkId = id;
            wr->groupId = groupId;
            wr->result = checkResult;
            wr->result2 = checkExResult;

            checkResultStore[build].push_back(wr);
        }

        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u warden checks.", count);
    sLog.outString();
}

void WardenStorageMgr::LoadWardenPenalty()
{
    QueryResult* result = LoginDatabase.Query("SELECT groupId, action, banTime, reason FROM warden_penalty");

    if (!result)
    {
        sLog.outString(">> Loaded 0 Warden penalties. DB table `warden_penalty` is empty!");
        sLog.outString();
        return;
    }

    uint32 count = 0;

    ACE_WRITE_GUARD(ACE_RW_Mutex, g, _checkStoreLock);

    if (checkStore.empty())
        return;

    do
    {
        Field* fields = result->Fetch();

        uint16 groupId = fields[0].GetUInt16();
        uint8  action  = fields[1].GetUInt8();
        uint32  banTime  = fields[2].GetUInt32();
        std::string reason = fields[3].GetCppString();

        for (CheckContainer::const_iterator itr = checkStore.begin(); itr != checkStore.end(); ++itr)
        {
            std::vector<WardenCheck*> checkGroup = itr->second;
            for (std::vector<WardenCheck*>::const_iterator itr2 = checkGroup.begin(); itr2 != checkGroup.end(); ++itr2)
            {
                if ((*itr2) && (*itr2)->groupId == groupId)
                {
                    (*itr2)->action = action;
                    (*itr2)->reason = reason;
                    (*itr2)->banTime = banTime;
                }
            }
        }

        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u warden penalties.", count);
    sLog.outString();
}

WardenCheck* WardenStorageMgr::GetCheckDataById(uint16 Id, uint16 build, bool usesGroup)
{
    CheckContainer::const_iterator itr = checkStore.find(build);
    if (itr != checkStore.end())
    {
        std::vector<WardenCheck*> checkGroup = itr->second;
        for (std::vector<WardenCheck*>::const_iterator itr2 = checkGroup.begin(); itr2 != checkGroup.end(); ++itr2)
        {
            if (usesGroup)
            {
                if ((*itr2) && (*itr2)->groupId == Id)
                    return (*itr2);
            }
            else
            {
                if ((*itr2) && (*itr2)->checkId == Id)
                    return (*itr2);
            }
        }
    }

    return NULL;
}

WardenCheckResult* WardenStorageMgr::GetCheckResultById(uint16 Id, uint16 build, bool usesGroup)
{
    CheckResultContainer::const_iterator itr = checkResultStore.find(build);
    if (itr != checkResultStore.end())
    {
        std::vector<WardenCheckResult*> checkResultGroup = itr->second;
        for (std::vector<WardenCheckResult*>::const_iterator itr2 = checkResultGroup.begin(); itr2 != checkResultGroup.end(); ++itr2)
        {
            if (usesGroup)
            {
                if ((*itr2) && (*itr2)->groupId == Id)
                    return (*itr2);
            }
            else
            {
                if ((*itr2) && (*itr2)->checkId == Id)
                    return (*itr2);
            }
        }
    }

    return NULL;
}

/*bool WardenStorageMgr::LoadSecurityData()
{
    WardenCore* core = sWorld.getWardenCore();

    if (!core)
    {
        sLog.outString(">> Warden core loading failed!");
        sLog.outString();
        return false;
    }

    QueryResult * result = LoginDatabase.Query("SELECT * FROM security_data");

    if (!result)
    {
        sLog.outString(">> Load of Security Data is failed! Warden core is not activated!");
        sLog.outString();
        return false;
    }

    Field* fields = result->Fetch();

    std::string str_data[8];

    // get raw data
    for (uint8 i = 0; i < 8; ++i)
    {
        str_data[i] = fields[i].GetCppString();

        if (str_data[i] == "")
        {
            sLog.outString(">> Security Data %u not found! Warden core is not activated!", i);
            sLog.outString();
            return false;
        }
    }

    SecurityData * data = new SecurityData;
    GenerateSecurityData(data, str_data);
    core->AddSecurityData(data);

    return true;
}

void WardenStorageMgr::GenerateSecurityData(SecurityData * ss_data, std::string data[])
{
    // create raw hex string
    for (int i = 0; i < 8; ++i)
    {
        ss_data->data[i].SetHexStr(data[i].c_str());

        uint32 len = data[i].size() / 2;
        uint8 *temp = new uint8[len];
        memset(temp, 0, len);
        memcpy(temp, ss_data->data[i].AsByteArray(), ss_data->data[i].GetNumBytes());
        std::reverse(temp, temp + len);
        ss_data->data[i].SetBinary((uint8*)temp, len);
        delete [] temp;
    }
}*/
