
#include "libanticheat.h"

#include "Policies/SingletonImp.h"

#include "World.h"
#include "Player.h"
#include "WorldSession.h"
#include "AccountMgr.h"
#include "MasterPlayer.h"
#include "ObjectAccessor.h"
#include "Channel.h"
#include "ChannelMgr.h"

#include "Antispam/Antispam.h"
#include "MovementAnticheat/Cheat.h"
#include "WardenAnticheat/Warden.h"
#include "WardenAnticheat/WardenWin.h"
#include "WardenAnticheat/WardenMac.h"

INSTANTIATE_SINGLETON_1(AnticheatConfig);

void NostalriusAnticheatLib::LoadAnticheatData()
{
    sLog.outString("Loading antispam system ...");
    sAntispam->loadFromDB();

    sLog.outString("Loading anticheat system ...");
    sCheatsMgr->LoadFromDB();

    sLog.outString("Loading warden checks...");
    _wardenStorage.LoadWardenChecks();

    sLog.outString("Loading warden penalties...");
    _wardenStorage.LoadWardenPenalty();
}

void NostalriusAnticheatLib::LoadConfig()
{
    if (!sAcConfig.SetSource(_LIB_ANTICHEAT_CONFIG))
        sLog.outError("[Anticheat] Could not find configuration file %s.", _LIB_ANTICHEAT_CONFIG);

    sAcConfig.loadConfigSettings();
    sCheatsMgr->LoadConfig();
    sAntispam->loadConfig();
}

PlayerAnticheatInterface* NostalriusAnticheatLib::CreateAnticheatFor(Player* player)
{
    PlayerCheatData* cd = new PlayerCheatData(player);
    cd->Init();
    return cd;
}

WardenInterface* NostalriusAnticheatLib::CreateWardenFor(WorldSession* client, BigNumber* K)
{
    Warden* _warden;
    ClientOSType os = client->GetOS();

    if (os == CLIENT_OS_MAC)
        _warden = new WardenMac();
    else if (os == CLIENT_OS_WIN)
        _warden = new WardenWin();
    else
        return nullptr;

    _warden->Init(client, K);

    return _warden;
}

NostalriusAnticheatLib* NostalriusAnticheatLib::instance()
{
    static NostalriusAnticheatLib i;
    return &i;
}

AntispamInterface* NostalriusAnticheatLib::GetAntispam() const
{
    return sAntispam;
}

AnticheatConfig::AnticheatConfig()
{
    for (int i = 0; i < CONFIG_INT32_AC_COUNT; ++i)
        m_configInt32Values[i] = 0;

    for (int i = 0; i < CONFIG_UINT32_AC_COUNT; ++i)
        m_configUInt32Values[i] = 0;

    for (int i = 0; i < CONFIG_BOOL_AC_COUNT; ++i)
        m_configBoolValues[i] = false;
}

void AnticheatConfig::loadConfigSettings()
{
    setConfig(CONFIG_BOOL_AC_ANTICHEAT_ENABLED, "Anticheat.Enable", false);
    setConfig(CONFIG_BOOL_AC_ANTICHEAT_ANTI_MULTI_JUMP_ENABLED, "Anticheat.AntiMultiJump.Enable", false);
    setConfig(CONFIG_BOOL_AC_ANTICHEAT_ANTI_SPEED_HACK_ENABLED, "Anticheat.AntiSpeedHack.Enable", false);
    setConfig(CONFIG_BOOL_AC_ANTICHEAT_USE_INTERPOLATION, "Anticheat.AntiSpeedHack.UseInterpolation", false);
    setConfig(CONFIG_BOOL_AC_ANTICHEAT_ANTI_WALL_CLIMB_ENABLED, "Anticheat.AntiWallClimb.Enable", false);
    setConfig(CONFIG_BOOL_AC_ANTICHEAT_ANTI_WATER_WALK_ENABLED, "Anticheat.AntiWaterWalk.Enable", false);
    setConfig(CONFIG_BOOL_AC_ANTICHEAT_NOTIFY_CHEATERS, "Anticheat.NotifyCheaters", false);
    setConfig(CONFIG_BOOL_AC_ANTICHEAT_LOG_DATA, "Anticheat.LogData", false);
    setConfig(CONFIG_BOOL_AC_ANTISPAM_ENABLED, "Antispam.Enable", false);
    setConfig(CONFIG_BOOL_AC_ANTISPAM_BAN_ENABLED, "Antispam.BanEnable", false);
    setConfig(CONFIG_BOOL_AC_ANTISPAM_MERGE_ALL_WHISPERS, "Antispam.MergeAllWhispers", false);

    setConfig(CONFIG_INT32_AC_ANTICHEAT_MAX_ALLOWED_DESYNC, "Anticheat.MaxAllowedDesync", 0);

    setConfig(CONFIG_UINT32_AC_ANTICHEAT_GM_ANNOUNCE_MASK, "Anticheat.GMAnnounceMask", 0);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_MAX_RESTRICTION_LEVEL, "Antispam.MaxRestrictionLevel", 20);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_ORIGINAL_NORMALIZE_MASK, "Antispam.OriginalNormalizeMask", 0);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_FULLY_NORMALIZE_MASK, "Antispam.FullyNormalizeMask", 0);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_SCORE_THRESHOLD, "Antispam.ScoreThreshold", 4);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_MUTETIME, "Antispam.Mutetime", 3600);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_CHAT_MASK, "Antispam.ChatMask", 0);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_DETECT_THRESHOLD, "Antispam.DetectThreshold", 3);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_REPEAT_COUNT, "Antispam.RepeatCount", 5);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_UPDATE_TIMER, "Antispam.UpdateTimer", 60000);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_MESSAGE_BLOCK_SIZE, "Antispam.MessageBlockSize", 5);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_FREQUENCY_TIME, "Antispam.FrequencyTime", 3);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_FREQUENCY_COUNT, "Antispam.FrequencyCount", 3);
    setConfig(CONFIG_UINT32_AC_WARDEN_MEM_CHECKS_COUNT, "Warden.MemChecksCount", 10);
    setConfig(CONFIG_UINT32_AC_WARDEN_OTHER_CHECKS_COUNT, "Warden.OtherChecksCount", 6);
}

void AnticheatConfig::setConfig(AnticheatConfigInt32Values index, char const* fieldname, int32 defvalue)
{
    setConfig(index, sAcConfig.GetIntDefault(fieldname, defvalue));
}

void AnticheatConfig::setConfig(AnticheatConfigUInt32Values index, char const* fieldname, uint32 defvalue)
{
    setConfig(index, sAcConfig.GetIntDefault(fieldname, defvalue));
}

void AnticheatConfig::setConfig(AnticheatConfigBoolValues index, char const* fieldname, bool defvalue)
{
    setConfig(index, sAcConfig.GetBoolDefault(fieldname, defvalue));
}

AnticheatLibInterface* GetAnticheatLib()
{
    return NostalriusAnticheatLib::instance();
}
