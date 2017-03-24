
#ifndef LIB_ANTICHEAT_H
#define LIB_ANTICHEAT_H

#include "Config/Config.h"
#include "Anticheat.h"

#if PLATFORM == PLATFORM_WINDOWS
  #define _LIB_ANTICHEAT_CONFIG  "anticheat.conf"
#else
  #define _LIB_ANTICHEAT_CONFIG  SYSCONFDIR "anticheat.conf"
#endif

class NostalriusAnticheatLib: public AnticheatLibInterface
{
    public:
        NostalriusAnticheatLib() {}
        void LoadAnticheatData();

        void LoadConfig();
        PlayerAnticheatInterface* CreateAnticheatFor(Player* player);
        WardenInterface * CreateWardenFor(WorldSession* client, BigNumber* K);

        AntispamInterface* GetAntispam() const;

        static NostalriusAnticheatLib* instance();
};

enum AnticheatConfigInt32Values
{
    CONFIG_INT32_AC_ANTICHEAT_MAX_ALLOWED_DESYNC,
    CONFIG_INT32_AC_COUNT
};

enum AnticheatConfigUInt32Values
{
    CONFIG_UINT32_AC_ANTICHEAT_GM_ANNOUNCE_MASK,
    CONFIG_UINT32_AC_ANTISPAM_MAX_RESTRICTION_LEVEL,
    CONFIG_UINT32_AC_ANTISPAM_ORIGINAL_NORMALIZE_MASK,
    CONFIG_UINT32_AC_ANTISPAM_FULLY_NORMALIZE_MASK,
    CONFIG_UINT32_AC_ANTISPAM_SCORE_THRESHOLD,
    CONFIG_UINT32_AC_ANTISPAM_MUTETIME,
    CONFIG_UINT32_AC_ANTISPAM_CHAT_MASK,
    CONFIG_UINT32_AC_ANTISPAM_DETECT_THRESHOLD,
    CONFIG_UINT32_AC_ANTISPAM_REPEAT_COUNT,
    CONFIG_UINT32_AC_ANTISPAM_UPDATE_TIMER,
    CONFIG_UINT32_AC_ANTISPAM_MESSAGE_BLOCK_SIZE,
    CONFIG_UINT32_AC_ANTISPAM_FREQUENCY_TIME,
    CONFIG_UINT32_AC_ANTISPAM_FREQUENCY_COUNT,
    CONFIG_UINT32_AC_WARDEN_MEM_CHECKS_COUNT,
    CONFIG_UINT32_AC_WARDEN_OTHER_CHECKS_COUNT,
    CONFIG_UINT32_AC_COUNT
};

enum AnticheatConfigBoolValues
{
    CONFIG_BOOL_AC_ANTICHEAT_ENABLED,
    CONFIG_BOOL_AC_ANTICHEAT_ANTI_MULTI_JUMP_ENABLED,
    CONFIG_BOOL_AC_ANTICHEAT_ANTI_SPEED_HACK_ENABLED,
    CONFIG_BOOL_AC_ANTICHEAT_USE_INTERPOLATION,
    CONFIG_BOOL_AC_ANTICHEAT_ANTI_WALL_CLIMB_ENABLED,
    CONFIG_BOOL_AC_ANTICHEAT_ANTI_WATER_WALK_ENABLED,
    CONFIG_BOOL_AC_ANTICHEAT_NOTIFY_CHEATERS,
    CONFIG_BOOL_AC_ANTICHEAT_LOG_DATA,
    CONFIG_BOOL_AC_ANTISPAM_ENABLED,
    CONFIG_BOOL_AC_ANTISPAM_BAN_ENABLED,
    CONFIG_BOOL_AC_ANTISPAM_MERGE_ALL_WHISPERS,
    CONFIG_BOOL_AC_COUNT
};

class AnticheatConfig : public Config
{
    public:
        AnticheatConfig();
        void loadConfigSettings();

        int32       getConfig(AnticheatConfigInt32Values index) const { return m_configInt32Values[index]; }
        uint32      getConfig(AnticheatConfigUInt32Values index) const { return m_configUInt32Values[index]; }
        bool        getConfig(AnticheatConfigBoolValues index) const { return m_configBoolValues[index]; }
        void        setConfig(AnticheatConfigInt32Values index, int32 value) { m_configInt32Values[index] = value; }
        void        setConfig(AnticheatConfigUInt32Values index, uint32 value) { m_configUInt32Values[index] = value; }
        void        setConfig(AnticheatConfigBoolValues index, bool value) { m_configBoolValues[index] = value; }

    private:
        void setConfig(AnticheatConfigInt32Values index, char const* fieldname, int32 defvalue);
        void setConfig(AnticheatConfigUInt32Values index, char const* fieldname, uint32 defvalue);
        void setConfig(AnticheatConfigBoolValues index, char const* fieldname, bool defvalue);

        int32 m_configInt32Values[CONFIG_INT32_AC_COUNT];
        uint32 m_configUInt32Values[CONFIG_UINT32_AC_COUNT];
        bool m_configBoolValues[CONFIG_BOOL_AC_COUNT];
};

#define sAcConfig (*ACE_Singleton<AnticheatConfig, ACE_Recursive_Thread_Mutex>::instance())

#endif
