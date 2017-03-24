
#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "Config/Config.h"

#include "module/libanticheat.h"
#include "Cheat.h"
#include "Chat.h"
#include "Player.h"
#include "GameObject.h"
#include "WorldSession.h"
#include "World.h"
#include "Language.h"

#include "MoveSpline.h"

namespace Movement
{
extern float computeFallElevation(float time, bool safeFall, float initialSpeed);
}

const char* GetCheatTypeNameFromFlag(CheatType flagId)
{
    switch (flagId)
    {
        case CHEAT_TYPE_WALL_CLIMB:
            return "WallClimb";
        case CHEAT_TYPE_WATER_WALK:
            return "WaterWalk";
        case CHEAT_TYPE_FORBIDDEN:
            return "AccessForbidden";
        case CHEAT_TYPE_BG_NOT_STARTED:
            return "BgNotStarted";
        case CHEAT_TYPE_MULTIJUMP:
            return "MultiJump";
        case CHEAT_TYPE_FALL_UP:
            return "FakeFall";
        case CHEAT_TYPE_PVE_FLYHACK:
            return "PvEUnreachable";
        case CHEAT_TYPE_TIME_BACK:
            return "ReverseTime";
        case CHEAT_TYPE_OVERSPEED_JUMP:
            return "OverspeedJump";
        case CHEAT_TYPE_JUMP_SPEED_CHANGE:
            return "JumpSpeedChange";
        case CHEAT_TYPE_FLY_HACK_SWIM:
            return "FlyHackSwim";
        case CHEAT_TYPE_ROOT_MOVE:
            return "MovementRooted";
        case CHEAT_TYPE_ROOT_IGNORED:
            return "Unstunnable";
        case CHEAT_TYPE_OVERSPEED_DIST:
            return "OverspeedDist";
        case CHEAT_TYPE_DESYNC:
            return "TimeDesync";
        case CHEAT_TYPE_EXPLORE:
            return "Exploration";
        case CHEAT_TYPE_EXPLORE_HIGH_LEVEL:
            return "ExploreHighLevelArea";
        case CHEAT_TYPE_OVERSPEED_Z:
            return "OverspeedZ";
        case CHEAT_TYPE_SKIPPED_HEARTBEATS:
            return "SkippedHeartbeats";
        case CHEAT_TYPE_NUM_DESYNC:
            return "NumDesyncs";
        case CHEAT_TYPE_FAKE_TRANSPORT:
            return "FakeTransport";
        case CHEAT_TYPE_TELE_TO_TRANSPORT:
            return "TeleToTransport";
        default:
            return "UnknownCheat";
    }
}

/// PlayerCheatsMgr
void PlayerCheatsMgr::LoadFromDB()
{
    sLog.outString("Loading table 'cheat_sanctions'");
    _sanctions.clear();

    QueryResult* result = WorldDatabase.Query("SELECT flag, tick_max, tick_action, total_max, total_action, comment FROM cheat_sanctions;");
    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();
        CheatSanctions s;
        s.cheatType     = fields[0].GetUInt32();
        s.tickCount     = fields[1].GetUInt32();
        s.tickSanction  = fields[2].GetUInt32();
        s.totalCount    = fields[3].GetUInt32();
        s.totalSanction = fields[4].GetUInt32();
        s.comment       = fields[5].GetString();

        if (s.cheatType >= CHEATS_COUNT)
            sLog.outError("cheat_sanctions has record with invalid cheatType %u > CHEATS_COUNT (%u)", s.cheatType, CHEATS_COUNT);
        else if (s.tickSanction >= CHEAT_MAX_ACTIONS || s.totalSanction >= CHEAT_MAX_ACTIONS)
            sLog.outError("cheat_sanctions has record with invalid action (must be < %u)", CHEAT_MAX_ACTIONS);
        else
            _sanctions.emplace_back(s);
    }
    while (result->NextRow());
    delete result;

    sLog.outString(">> %u sanctions loaded", _sanctions.size());
    sLog.outString();
}

void PlayerCheatsMgr::LoadConfig()
{
    _enabled            = sAcConfig.getConfig(CONFIG_BOOL_AC_ANTICHEAT_ENABLED);
    _antiMultiJump      = sAcConfig.getConfig(CONFIG_BOOL_AC_ANTICHEAT_ANTI_MULTI_JUMP_ENABLED);
    _antiSpeedHack      = sAcConfig.getConfig(CONFIG_BOOL_AC_ANTICHEAT_ANTI_SPEED_HACK_ENABLED);
    _antiSpeedHackInterp = sAcConfig.getConfig(CONFIG_BOOL_AC_ANTICHEAT_USE_INTERPOLATION);
    _antiWallClimb      = sAcConfig.getConfig(CONFIG_BOOL_AC_ANTICHEAT_ANTI_WALL_CLIMB_ENABLED);
    _announceCheatMask  = sAcConfig.getConfig(CONFIG_UINT32_AC_ANTICHEAT_GM_ANNOUNCE_MASK);
    _maxAllowedDesync   = sAcConfig.getConfig(CONFIG_INT32_AC_ANTICHEAT_MAX_ALLOWED_DESYNC);
    _notifyCheaters     = sAcConfig.getConfig(CONFIG_BOOL_AC_ANTICHEAT_NOTIFY_CHEATERS);
    _logDatas           = sAcConfig.getConfig(CONFIG_BOOL_AC_ANTICHEAT_LOG_DATA);
}

CheatAction PlayerCheatsMgr::ComputeCheatAction(PlayerCheatData* cheatData, std::stringstream& reason) const
{
    uint32 action = CHEAT_ACTION_NONE;

    for (auto const &s : _sanctions)
    {
        uint32 tickCount = 0;
        uint32 totalCount = 0;
        cheatData->CountCheatOccur(s.cheatType, tickCount, totalCount);

        if (s.tickCount && tickCount >= s.tickCount)
        {
            if (reason.str() != "")
                reason << "/";
            reason << s.comment << "(Tick:" << tickCount << ")";
            action |= s.tickSanction;
        }

        if (s.totalCount && totalCount >= s.totalCount)
        {
            if (reason.str() != "")
                reason << "/";
            reason << s.comment << "(Total:" << totalCount << ")";
            action |= s.totalSanction;
        }
    }

    return CheatAction(action);
}

/// PlayerCheatData
void PlayerCheatData::Init()
{
    memset(cheatOccuranceTick, 0, sizeof(cheatOccuranceTick));
    memset(cheatOccuranceTotal, 0, sizeof(cheatOccuranceTotal));

    _overspeedDistance  = 0.f;
    _maxOverspeedDistance = 0.f;
    _clientDesynchro    = 0;
    _maxClientDesynchro = 0;

    _jumpInitialSpeed   = 0.f;

    _storeCheatFlags    = 0;
    _jumpcount = 0;
    updateCheckTimer = CHEATS_UPDATE_INTERVAL;
    _inKnockBack = false;

    _orders =
    {
        { SMSG_FORCE_WALK_SPEED_CHANGE, CMSG_FORCE_WALK_SPEED_CHANGE_ACK },
        { SMSG_FORCE_RUN_SPEED_CHANGE, CMSG_FORCE_RUN_SPEED_CHANGE_ACK },
        { SMSG_FORCE_RUN_BACK_SPEED_CHANGE, CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK },
        { SMSG_FORCE_SWIM_SPEED_CHANGE, CMSG_FORCE_SWIM_SPEED_CHANGE_ACK },
        { SMSG_FORCE_SWIM_BACK_SPEED_CHANGE, CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK },
        { SMSG_FORCE_TURN_RATE_CHANGE, CMSG_FORCE_TURN_RATE_CHANGE_ACK },
        { SMSG_FORCE_MOVE_ROOT, CMSG_FORCE_MOVE_ROOT_ACK },
        { SMSG_FORCE_MOVE_UNROOT, CMSG_FORCE_MOVE_UNROOT_ACK },
        { SMSG_MOVE_FEATHER_FALL, SMSG_MOVE_NORMAL_FALL, CMSG_MOVE_FEATHER_FALL_ACK },
        { SMSG_MOVE_SET_HOVER, SMSG_MOVE_UNSET_HOVER, CMSG_MOVE_HOVER_ACK },
        { SMSG_MOVE_SET_FLIGHT, SMSG_MOVE_UNSET_FLIGHT, CMSG_MOVE_FLIGHT_ACK },
        { SMSG_MOVE_WATER_WALK, SMSG_MOVE_LAND_WALK, CMSG_MOVE_WATER_WALK_ACK }
    };
}

void PlayerCheatData::KnockBack(float speedxy, float speedz, float cos, float sin)
{
    GetLastMovementInfo().jump.startClientTime = WorldTimer::getMSTime() - GetLastMovementInfo().time + GetLastMovementInfo().ctime;
    GetLastMovementInfo().jump.start.x = me->GetPositionX();
    GetLastMovementInfo().jump.start.y = me->GetPositionY();
    GetLastMovementInfo().jump.start.z = me->GetPositionZ();
    GetLastMovementInfo().jump.cosAngle = cos;
    GetLastMovementInfo().jump.sinAngle = sin;
    GetLastMovementInfo().jump.xyspeed = speedxy;
    GetLastMovementInfo().moveFlags = MOVEFLAG_JUMPING | (GetLastMovementInfo().moveFlags & ~MOVEFLAG_MASK_MOVING_OR_TURN);
    _jumpInitialSpeed = speedz;
    _inKnockBack = true;
}

void PlayerCheatData::StoreCheat(uint32 type, uint32 count)
{
    cheatOccuranceTotal[type] += count;
    cheatOccuranceTick[type] += count;
}

void PlayerCheatData::HandleCommand(ChatHandler* handler) const
{
    handler->SendSysMessage("----- NOSTRIA ANTICHEAT -----");
    handler->SendSysMessage("_____ Orders/ACK");
    
    for (auto const &order : _orders)
        handler->PSendSysMessage("\t%s counter=%i", LookupOpcodeName(order.clientResp), order.counter);

    handler->SendSysMessage("_____ Cheats detected");
    for (uint32 i = 0; i < CHEATS_COUNT; ++i)
        if (cheatOccuranceTotal[i])
            handler->PSendSysMessage("%2u x %s (cheat %u - 0x%x)", cheatOccuranceTotal[i], GetCheatTypeNameFromFlag(CheatType(i)), i, 1 << i);

    handler->SendSysMessage("_____ Interpolation");
    handler->PSendSysMessage("MaxSpaceDesync=%f", _maxOverspeedDistance);
    handler->PSendSysMessage("MaxTimeDesync=%u", _maxClientDesynchro);
}

void PlayerCheatData::CountCheatOccur(uint32 type, uint32& tickCount, uint32& totalCount) const
{
    ASSERT(type < CHEATS_COUNT);
    tickCount = cheatOccuranceTick[type];
    totalCount = cheatOccuranceTotal[type];
}

CheatAction PlayerCheatData::Update(uint32 diff, std::stringstream& reason)
{
    if (!sCheatsMgr->EnableAnticheat())
        return CHEAT_ACTION_NONE;

    // Every X seconds, combine detected cheats
    if (updateCheckTimer >= diff)
    {
        updateCheckTimer -= diff;
        return CHEAT_ACTION_NONE;
    }

    return Finalize(reason);
}

CheatAction PlayerCheatData::Finalize(std::stringstream& reason)
{
    auto const now = WorldTimer::getMSTime();

    for (auto &order : _orders)
        if (order.counter > 0 && order.lastRcvd < order.lastSent && (now - order.lastSent) > ALLOWED_ACK_LAG)
            if (order.clientResp == CMSG_FORCE_MOVE_ROOT_ACK)
            {
                StoreCheat(CHEAT_TYPE_ROOT_IGNORED, order.counter);
                order.counter = 0;
            }

    if (_maxOverspeedDistance < fabs(_overspeedDistance))
        _maxOverspeedDistance = fabs(_overspeedDistance);
    if (_maxClientDesynchro < static_cast<uint32>(abs(_clientDesynchro)))
        _maxClientDesynchro = abs(_clientDesynchro);

    cheatOccuranceTick[CHEAT_TYPE_OVERSPEED_DIST] = uint32(fabs(_overspeedDistance));
    cheatOccuranceTick[CHEAT_TYPE_DESYNC] = abs(_clientDesynchro);

    DEBUG_UNIT(me, DEBUG_CHEAT, "Desynchro %ims / %fyards", _clientDesynchro, _overspeedDistance);
    updateCheckTimer = CHEATS_UPDATE_INTERVAL;

    /// Check detected cheats with DB rules
    CheatAction result = sCheatsMgr->ComputeCheatAction(this, reason);

    /// Log data
    if (sCheatsMgr->EnableDataLog() && me->IsInWorld())
    {
        CharacterDatabase.PExecute("INSERT INTO cheats_raw "
                                   "(account, guid, posx, posy, posz, map, desyncMs, desyncDist) VALUES "
                                   "(%u,      %u,     %f,   %f,   %f, %u,  %i,      %f);",
                                   me->GetSession()->GetAccountId(), me->GetGUIDLow(), me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(),
                                   me->GetMapId(), _clientDesynchro, _overspeedDistance);
    }

    /// Reset to zero tick counts
    memset(cheatOccuranceTick, 0, sizeof(cheatOccuranceTick));

    _clientDesynchro = 0;
    _overspeedDistance = 0.0f;
    return result;
}

void PlayerCheatData::OrderSent(WorldPacket const* data)
{
    for (auto &order : _orders)
        if (order.serverOpcode1 == data->GetOpcode() || order.serverOpcode2 == data->GetOpcode())
        {
            order.lastSent = WorldTimer::getMSTime();
            ++order.counter;
            break;
        }
}

void PlayerCheatData::CheckForOrderAck(uint32 opcode)
{
    for (auto &order : _orders)
        if (order.clientResp == opcode)
        {
            --order.counter;
            break;
        }
}

/// Movement processing anticheat main routine
bool PlayerCheatData::HandleAnticheatTests(MovementInfo& movementInfo, WorldSession* session, WorldPacket* packet)
{
    ASSERT(me);
    ASSERT(session);
    ASSERT(packet);

    /// TODO: Currently anticheat is disabled with Mind Controlled players!
    if (!sCheatsMgr->EnableAnticheat() || me != session->GetPlayer())
        return true;

    uint32 cheatType = 0x0;
#define APPEND_CHEAT(t) cheatType |= (1 << t)

    uint32 opcode = packet->GetOpcode();
    CheckForOrderAck(opcode);

    if (_inKnockBack && opcode != MSG_MOVE_FALL_LAND)
        movementInfo.jump = GetLastMovementInfo().jump;

    if (opcode == CMSG_MOVE_FEATHER_FALL_ACK)
    {
        GetLastMovementInfo().jump.startClientTime = movementInfo.jump.startClientTime = movementInfo.ctime;
        //GetLastMovementInfo().jump.start = movementInfo.jump.start = movementInfo.pos;
        _jumpInitialSpeed = std::max(_jumpInitialSpeed, 7.0f);
    }

    if (opcode == MSG_MOVE_JUMP && movementInfo.jump.xyspeed > (GetClientSpeed(MOVE_RUN) + 0.0001f))
        APPEND_CHEAT(CHEAT_TYPE_OVERSPEED_JUMP);

    // Not allowed to change jump speed while jumping
    if ((movementInfo.moveFlags & (MOVEFLAG_JUMPING | MOVEFLAG_FALLINGFAR)) && (GetLastMovementInfo().moveFlags & (MOVEFLAG_JUMPING | MOVEFLAG_FALLINGFAR)))
        if (fabs(movementInfo.jump.xyspeed - movementInfo.jump.xyspeed) > 0.0001f)
            if (fabs(movementInfo.jump.xyspeed - std::min(GetClientSpeed(MOVE_RUN), 2.5f)) > 0.0001f)
                APPEND_CHEAT(CHEAT_TYPE_JUMP_SPEED_CHANGE);

    if ((movementInfo.moveFlags & MOVEFLAG_ROOT) && (movementInfo.moveFlags & MOVEFLAG_MASK_MOVING_OR_TURN))
        APPEND_CHEAT(CHEAT_TYPE_ROOT_MOVE);

    if (sCheatsMgr->EnableAntiMultiJumpHack())
    {
        if (opcode == MSG_MOVE_JUMP)
        {
            _jumpcount++;
            if (_jumpcount > 2)
                APPEND_CHEAT(CHEAT_TYPE_MULTIJUMP);
        }
        else if (opcode == MSG_MOVE_FALL_LAND || opcode == MSG_MOVE_START_SWIM)
            _jumpcount = 0;
    }

    if (opcode == MSG_MOVE_STOP_SWIM && (movementInfo.moveFlags & MOVEFLAG_SWIMMING))
        APPEND_CHEAT(CHEAT_TYPE_FLY_HACK_SWIM);

    if (movementInfo.ctime == 0)
        APPEND_CHEAT(CHEAT_TYPE_NULL_CLIENT_TIME);

    // Dont accept movement packets while movement is controlled by server (fear, charge, etc..)
    if (!me->movespline->Finalized())
        return false;

    if (GetLastMovementInfo().ctime)
    {
        if (GetLastMovementInfo().moveFlags & MOVEFLAG_MASK_MOVING)
        {
            int32 currentDesync = (int32)WorldTimer::getMSTimeDiff(GetLastMovementInfo().ctime, movementInfo.ctime) - WorldTimer::getMSTimeDiff(GetLastMovementInfo().time, movementInfo.time);
            _clientDesynchro += currentDesync;
            if (currentDesync > 1000)
                APPEND_CHEAT(CHEAT_TYPE_NUM_DESYNC);
        }
        // Client going back in time ... ?!
        if (movementInfo.ctime < GetLastMovementInfo().ctime)
            APPEND_CHEAT(CHEAT_TYPE_TIME_BACK);
    }

    // Warsong Battleground - specific checks
    if (me->GetMapId() == 489)
    {
        // Too high - not allowed (but possible with some engineering items malfunction)
        if (!(movementInfo.moveFlags & (MOVEFLAG_FALLINGFAR | MOVEFLAG_JUMPING)) && movementInfo.pos.z > 380.0f)
            APPEND_CHEAT(CHEAT_TYPE_FORBIDDEN);
        if (BattleGround* bg = me->GetBattleGround())
        {
            if (bg->GetStatus() == STATUS_WAIT_JOIN)
            {
                // Battleground not started. Players should be in their starting areas.
                if (me->GetTeam() == ALLIANCE && movementInfo.pos.x < 1490.0f)
                    APPEND_CHEAT(CHEAT_TYPE_BG_NOT_STARTED);
                if (me->GetTeam() == HORDE && movementInfo.pos.x > 957.0f)
                    APPEND_CHEAT(CHEAT_TYPE_BG_NOT_STARTED);
            }
        }
    }

    // Minimal checks on transports
    if (movementInfo.moveFlags & MOVEFLAG_ONTRANSPORT)
    {
        // To transport tele hack detection
        if (GetLastMovementInfo().ctime && !(GetLastMovementInfo().moveFlags & MOVEFLAG_ONTRANSPORT))
        {
            float dist2d = (movementInfo.pos.x - GetLastMovementInfo().pos.x) * (movementInfo.pos.x - GetLastMovementInfo().pos.x);
            dist2d += (movementInfo.pos.y - GetLastMovementInfo().pos.y) * (movementInfo.pos.y - GetLastMovementInfo().pos.y);
            if (dist2d > 100*100)
                APPEND_CHEAT(CHEAT_TYPE_TELE_TO_TRANSPORT);
        }
    }

    // Distance computation related
    if (!me->IsTaxiFlying() && !(movementInfo.moveFlags & MOVEFLAG_ONTRANSPORT) && sCheatsMgr->EnableAntiSpeedHack())
    {
        float allowedDXY = 0.0f;
        float allowedDZ  = 0.0f;
        float realDistance2D_sq = 0.0f;

        int32 dt = movementInfo.ctime - GetLastMovementInfo().ctime;
        if (sCheatsMgr->GetMaxAllowedDesync() && dt > sCheatsMgr->GetMaxAllowedDesync())
            dt = sCheatsMgr->GetMaxAllowedDesync();

        // Check vs interpolation
        if (sCheatsMgr->EnableAntiSpeedHackInterpolation())
        {
            float intX, intY, intZ, intO;

            if (InterpolateMovement(GetLastMovementInfo(), dt, intX, intY, intZ, intO))
            {
                auto const intDX = intX - movementInfo.pos.x;
                auto const intDY = intY - movementInfo.pos.y;
                auto const intDZ = intZ - movementInfo.pos.z;

                auto interpolDist = pow(intDX, 2) + pow(intDY, 2);
                if ((movementInfo.moveFlags | GetLastMovementInfo().moveFlags) & MOVEFLAG_JUMPING)
                    interpolDist += pow(intDZ, 2);
                interpolDist = sqrt(interpolDist);

                float allowedDX = pow(intX - GetLastMovementInfo().pos.x, 2);
                float allowedDY = pow(intY - GetLastMovementInfo().pos.y, 2);
                allowedDXY = sqrt(allowedDX + allowedDY);
                realDistance2D_sq = pow(movementInfo.pos.x - GetLastMovementInfo().pos.x, 2) + pow(movementInfo.pos.y - GetLastMovementInfo().pos.y, 2);

                if (realDistance2D_sq > (allowedDY + allowedDX) * 1.1f)
                    _overspeedDistance += sqrt(realDistance2D_sq) - sqrt(allowedDY + allowedDX);

                DEBUG_UNIT(me, DEBUG_CHEAT, "[Opcode:%u:0x%x] Flags 0x%x [DT=%u:DR=%.2f]", opcode, opcode, movementInfo.moveFlags, movementInfo.ctime - GetLastMovementInfo().ctime, interpolDist);
            }
        }
        else if (GetMaxAllowedDist(GetLastMovementInfo(), dt, allowedDXY, allowedDZ))
        {
            // Allow some margin
            allowedDXY += 0.5f;
            allowedDZ  += 0.5f;
            realDistance2D_sq = pow(movementInfo.pos.x - GetLastMovementInfo().pos.x, 2) + pow(movementInfo.pos.y - GetLastMovementInfo().pos.y, 2);
            if (realDistance2D_sq > allowedDXY * allowedDXY * 1.1f)
                _overspeedDistance += sqrt(realDistance2D_sq) - allowedDXY;
            if (fabs(movementInfo.pos.z - GetLastMovementInfo().pos.z) > allowedDZ)
                APPEND_CHEAT(CHEAT_TYPE_OVERSPEED_Z);

            DEBUG_UNIT(me, DEBUG_CHEAT, "[Opcode:%u:0x%x] Flags 0x%x [ctime=%u]", opcode, opcode, movementInfo.moveFlags, movementInfo.ctime);
        }

        // Client should send heartbeats every 500ms
        if (dt > 1000 && GetLastMovementInfo().ctime && GetLastMovementInfo().moveFlags & MOVEFLAG_MASK_MOVING)
            APPEND_CHEAT(CHEAT_TYPE_SKIPPED_HEARTBEATS);

        if (realDistance2D_sq > 20.0f*20.0f)
            DEBUG_UNIT(me, DEBUG_CHEAT, "RealDist: %f AllowedXY: %f", sqrt(realDistance2D_sq), sqrt(allowedDXY));
    }

    //GetLastMovementInfo() = movementInfo;
    //GetLastMovementInfo().UpdateTime(WorldTimer::getMSTime());
    // This is required for proper movement interpolation
    if (opcode == MSG_MOVE_JUMP)
        _jumpInitialSpeed = 7.95797334f;
    else if (opcode == MSG_MOVE_FALL_LAND)
    {
        _jumpInitialSpeed = -9.645f;
        _inKnockBack = false;
    }

    AddCheats(cheatType);

    return true;
#undef APPEND_CHEAT
}

bool PlayerCheatData::HandleSpeedChangeAck(MovementInfo& movementInfo, WorldSession* session, WorldPacket* packet, float newSpeed)
{
    static char const* move_type_name[MAX_MOVE_TYPE] = {"Walk", "Run", "RunBack", "Swim", "SwimBack", "TurnRate"};
    UnitMoveType moveType;
    switch (packet->GetOpcode())
    {
        case CMSG_FORCE_WALK_SPEED_CHANGE_ACK:
            moveType = MOVE_WALK;
            break;
        case CMSG_FORCE_RUN_SPEED_CHANGE_ACK:
            moveType = MOVE_RUN;
            break;
        case CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK:
            moveType = MOVE_RUN_BACK;
            break;
        case CMSG_FORCE_SWIM_SPEED_CHANGE_ACK:
            moveType = MOVE_SWIM;
            break;
        case CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK:
            moveType = MOVE_SWIM_BACK;
            break;
        case CMSG_FORCE_TURN_RATE_CHANGE_ACK:
            moveType = MOVE_TURN_RATE;
            break;
        default:
            return false;
    }
    // Compare to server side speed.
    if (!me->GetTransport() && fabs(me->GetSpeed(moveType) - newSpeed) > 0.01f)
    {
        DETAIL_LOG("%sSpeedChange player %s incorrect. %f -> %f (instead of %f)",
                       move_type_name[moveType], me->GetName(), _clientSpeeds[moveType], newSpeed, me->GetSpeed(moveType));
        me->SetSpeedRate(moveType, me->GetSpeedRate(moveType), true);
    }
    // Compute anticheat generic checks - with old speed.
    HandleAnticheatTests(movementInfo, session, packet);
    _clientSpeeds[moveType] = newSpeed;
    return true;
}

void PlayerCheatData::InitSpeeds(Unit* unit)
{
    for (int i = 0; i < MAX_MOVE_TYPE; ++i)
        _clientSpeeds[i] = unit->GetSpeed(UnitMoveType(i));
}


void PlayerCheatData::Unreachable(Unit* attacker)
{
    if (IsInKnockBack())
        return;
    if (attacker->GetCharmerOrOwnerGuid() == me->GetObjectGuid())
        return;
    if (me->GetTransport())
        return;

    float waterLevel = (me->GetTerrain()->GetWaterLevel(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()) + 5.0f);
    if (me->GetPositionZ() < waterLevel)
        return;
    if (attacker->GetPositionZ() < waterLevel)
        return;

    AddCheats(1 << CHEAT_TYPE_PVE_FLYHACK);
}

MovementInfo& PlayerCheatData::GetLastMovementInfo()
{
    return me->m_movementInfo;
}

void PlayerCheatData::AddCheats(uint32 cheats, uint32 count)
{
    if (!cheats)
        return;

    std::string sName = me->GetName();

    // This is a new cheat detected for this player
    if (!(_storeCheatFlags & cheats))
    {
        _storeCheatFlags |= cheats;
        if (cheats & sCheatsMgr->AnnounceCheatMask())
        {
            std::string cheatInfos = "|cffffffff|Hplayer:" + sName + "|h[" + sName + "]|h|r : ";
            for (uint32 i = 1; i <= cheats; i = i * 2)
                if (cheats & i)
                    cheatInfos += "-[" + std::string(GetCheatTypeNameFromFlag(CheatType(i))) + "]";
            sWorld.SendGMText(LANG_GM_ANNOUNCE_COLOR, "[AntiCheatAlert]", cheatInfos.c_str());
        }
    }

    if (sCheatsMgr->NotifyCheaters())
        for (uint32 i = 1; i <= cheats; i = i * 2)
            if (cheats & i)
                ChatHandler(me).PSendSysMessage("[AntiCheat] Cheat : %s", GetCheatTypeNameFromFlag(CheatType(i)));
    for (uint32 i = 0; i < CHEATS_COUNT; ++i)
        if (cheats & (1 << i))
            StoreCheat(i, count);
}

bool PlayerCheatData::InterpolateMovement(MovementInfo const& mi, uint32 diffMs, float &x, float &y, float &z, float &outOrientation)
{
    // TODO: These cases are not handled in mvt interpolationo
    // - Transports
    if (mi.moveFlags & (MOVEFLAG_PITCH_UP | MOVEFLAG_PITCH_DOWN | MOVEFLAG_ONTRANSPORT))
        return false;
    // - Not correctly handled yet (issues regarding feather fall auras)
    if (mi.moveFlags & MOVEFLAG_FALLINGFAR)
        return false;
    // - Server side movement (should be easy to interpolate actually)
    if (!me->movespline->Finalized())
        return false;
    // Dernier paquet pas a jour (connexion, TP autre map ...)
    if (mi.ctime == 0)
        return false;
    x = mi.pos.x;
    y = mi.pos.y;
    z = mi.pos.z;
    outOrientation = mi.pos.o;
    float o = outOrientation;
    // Not allowed to move
    if (mi.moveFlags & MOVEFLAG_ROOT)
        return true;

    float speed = 0.0f;
    if (mi.moveFlags & MOVEFLAG_SWIMMING)
        speed = mi.moveFlags & (MOVEFLAG_BACKWARD) ? GetClientSpeed(MOVE_SWIM_BACK) : GetClientSpeed(MOVE_SWIM);
    else if (mi.moveFlags & MOVEFLAG_WALK_MODE)
        speed = GetClientSpeed(MOVE_WALK);
    else if (mi.moveFlags & MOVEFLAG_MASK_MOVING)
        speed = mi.moveFlags & (MOVEFLAG_BACKWARD) ? GetClientSpeed(MOVE_RUN_BACK) : GetClientSpeed(MOVE_RUN);
    if (mi.moveFlags & MOVEFLAG_BACKWARD)
        o += M_PI_F;
    else if (mi.moveFlags & MOVEFLAG_STRAFE_LEFT)
    {
        if (mi.moveFlags & MOVEFLAG_FORWARD)
            o += M_PI_F / 4;
        else
            o += M_PI_F / 2;
    }
    else if (mi.moveFlags & MOVEFLAG_STRAFE_RIGHT)
    {
        if (mi.moveFlags & MOVEFLAG_FORWARD)
            o -= M_PI_F / 4;
        else
            o -= M_PI_F / 2;
    }
    if (mi.moveFlags & MOVEFLAG_JUMPING)
    {
        float diffT = WorldTimer::getMSTimeDiff(mi.jump.startClientTime, diffMs + mi.ctime) / 1000.0f;
        x = mi.jump.start.x;
        y = mi.jump.start.y;
        z = mi.jump.start.z;
        // Fatal error. Avoid crashing here ...
        if (!x || !y || !z || diffT > 10000.0f)
            return false;
        x += mi.jump.cosAngle * mi.jump.xyspeed * diffT;
        y += mi.jump.sinAngle * mi.jump.xyspeed * diffT;
        z -= Movement::computeFallElevation(diffT, mi.moveFlags & MOVEFLAG_SAFE_FALL, -_jumpInitialSpeed);
    }
    else if (mi.moveFlags & (MOVEFLAG_TURN_LEFT | MOVEFLAG_TURN_RIGHT))
    {
        if (mi.moveFlags & MOVEFLAG_MASK_MOVING)
        {
            // Every 2 sec
            float T = 0.75f * (GetClientSpeed(MOVE_TURN_RATE)) * (diffMs / 1000.0f);
            float R = 1.295f * speed / M_PI * cos(mi.s_pitch);
            z += diffMs * speed / 1000.0f * sin(mi.s_pitch);
            // Find the center of the circle we are moving on
            if (mi.moveFlags & MOVEFLAG_TURN_LEFT)
            {
                x += R * cos(o + M_PI / 2);
                y += R * sin(o + M_PI / 2);
                outOrientation += T;
                T = T - M_PI / 2.0f;
            }
            else
            {
                x += R * cos(o - M_PI / 2);
                y += R * sin(o - M_PI / 2);
                outOrientation -= T;
                T = -T + M_PI / 2.0f;
            }
            x += R * cos(o + T);
            y += R * sin(o + T);
        }
        else
        {
            float diffO = GetClientSpeed(MOVE_TURN_RATE) * diffMs / 1000.0f;
            if (mi.moveFlags & MOVEFLAG_TURN_LEFT)
                outOrientation += diffO;
            else
                outOrientation -= diffO;
            return true;
        }
    }
    else if (mi.moveFlags & MOVEFLAG_MASK_MOVING)
    {
        float dist = speed * diffMs / 1000.0f;
        x += dist * cos(o) * cos(mi.s_pitch);
        y += dist * sin(o) * cos(mi.s_pitch);
        z += dist * sin(mi.s_pitch);
    }
    else // If we reach here, we did not move
        return true;

    if (!MaNGOS::IsValidMapCoord(x, y, z, o))
        return false;

    if (!(mi.moveFlags & (MOVEFLAG_JUMPING | MOVEFLAG_FALLINGFAR | MOVEFLAG_SWIMMING)))
        z = me->GetMap()->GetHeight(x, y, z);
    return me->GetMap()->isInLineOfSight(mi.pos.x, mi.pos.y, mi.pos.z + 0.5f, x, y, z + 0.5f);
}

bool PlayerCheatData::GetMaxAllowedDist(MovementInfo const& mi, uint32 diffMs, float &dxy, float &dz)
{
    dxy = dz = 0.001f; // Epsilon
    if (mi.moveFlags & MOVEFLAG_ONTRANSPORT)
        return false;
    if (!me->movespline->Finalized())
        return false;
    // Dernier paquet pas a jour (connexion, TP autre map ...)
    if (!mi.ctime)
        return false;

    // No mvt allowed
    if (mi.moveFlags & MOVEFLAG_ROOT || !(mi.moveFlags & MOVEFLAG_MASK_MOVING))
        return true;

    float speed = 0.0f;
    if (mi.moveFlags & MOVEFLAG_SWIMMING)
        speed = mi.moveFlags & (MOVEFLAG_BACKWARD) ? GetClientSpeed(MOVE_SWIM_BACK) : GetClientSpeed(MOVE_SWIM);
    else if (mi.moveFlags & MOVEFLAG_WALK_MODE)
        speed = GetClientSpeed(MOVE_WALK);
    else if (mi.moveFlags & MOVEFLAG_MASK_MOVING)
        speed = mi.moveFlags & (MOVEFLAG_BACKWARD) ? GetClientSpeed(MOVE_RUN_BACK) : GetClientSpeed(MOVE_RUN);

    if (mi.moveFlags & (MOVEFLAG_JUMPING | MOVEFLAG_FALLINGFAR))
    {
        dxy = mi.jump.xyspeed / 1000 * diffMs;
        static const float terminalVelocity = 60.148003f;
        static const float terminalSavefallVelocity = 7.f;
        dz = (mi.moveFlags & MOVEFLAG_SAFE_FALL) ? terminalSavefallVelocity : terminalVelocity;
        dz = dz / 1000 * diffMs;
        return true;
    }
    // TODO: Maximum dyx/dz (max climb angle) if not swimming.
    dxy = speed / 1000 * diffMs;
    dz = speed / 1000 * diffMs;
    return true;
}

void PlayerCheatData::OnExplore(AreaEntry const* p)
{
    AddCheats(1 << CHEAT_TYPE_EXPLORE);
    if (static_cast<int32>(me->getLevel() + 10) < p->AreaLevel)
        AddCheats(1 << CHEAT_TYPE_EXPLORE_HIGH_LEVEL);
}

void PlayerCheatData::OnTransport(Player* plMover, ObjectGuid transportGuid)
{
    // The anticheat is disabled on transports, so we need to be sure that the player is indeed on a transport.
    GameObject* transportGobj = plMover->GetMap()->GetGameObject(transportGuid);
    float maxDist2d = 70.0f; // Transports usually dont go far away.
    if (plMover->GetMapId() == 369) // Deeprun tram
        maxDist2d = 3000.0f;
    if (!transportGobj || !transportGobj->IsTransport() || !transportGobj->IsWithinDist(plMover, maxDist2d, false))
        AddCheats(1 << CHEAT_TYPE_FAKE_TRANSPORT);
}

// TEMPORARY!!!

bool PlayerCheatData::CheckTeleport(uint32 opcode, MovementInfo& movementInfo)
{
    // TODO These checks are unreliable and should be implemented in other way

    if (me->GetMover()->GetTypeId() != TYPEID_PLAYER)
        return true;

    Player *mover = (Player*)(me->GetMover());

    /* teleport hack check */
    if (!mover->m_transport && !mover->HasMovementFlag(MOVEFLAG_ONTRANSPORT) && !mover->IsTaxiFlying())
    {
        float distance;

        if (!IsTeleportAllowed(movementInfo, distance))
        {
            uint32 destZoneId = 0;
            uint32 destAreaId = 0;

            mover->GetTerrain()->GetZoneAndAreaId(destZoneId, destAreaId, movementInfo.pos.x, movementInfo.pos.y, movementInfo.pos.z);

            // get zone and area info
            MapEntry const* mapEntry = sMapStorage.LookupEntry<MapEntry>(mover->GetMapId());
            const auto *srcZoneEntry = AreaEntry::GetById(mover->GetZoneId());
            const auto *srcAreaEntry = AreaEntry::GetById(mover->GetAreaId());
            const auto *destZoneEntry = AreaEntry::GetById(destZoneId);
            const auto *destAreaEntry = AreaEntry::GetById(destAreaId);

            uint32 locale = sWorld.GetDefaultDbcLocale();

            const char *mapName = mapEntry ? mapEntry->name : "<unknown>";
            const char *srcZoneName = srcZoneEntry ? srcZoneEntry->Name : "<unknown>";
            const char *srcAreaName = srcAreaEntry ? srcAreaEntry->Name : "<unknown>";
            const char *destZoneName = destZoneEntry ? destZoneEntry->Name : "<unknown>";
            const char *destAreaName = destAreaEntry ? destAreaEntry->Name : "<unknown>";

            sLog.outWarden("SERVER WARDEN (Teleport hack): player %s, %s, %.2f yd\n"
                "    map %u \"%s\"\n"
                "    source: zone %u \"%s\" area %u \"%s\" %.2f, %.2f, %.2f\n"
                "    dest:   zone %u \"%s\" area %u \"%s\" %.2f, %.2f, %.2f",
                mover->GetName(), LookupOpcodeName(opcode), distance,
                mover->GetMapId(), mapName,
                mover->GetZoneId(), srcZoneName, mover->GetAreaId(), srcAreaName,
                mover->GetPositionX(), mover->GetPositionY(), mover->GetPositionZ(),
                destZoneId, destZoneName, destAreaId, destAreaName,
                movementInfo.pos.x, movementInfo.pos.y, movementInfo.pos.z);

            // ban for GM Island
            if (me->GetSession()->GetSecurity() == SEC_PLAYER && destZoneId == 876 && destAreaId == 876)
            {
                sWorld.BanAccount(BAN_ACCOUNT, me->GetSession()->GetUsername(), 0, "Infiltration on GM Island", "Warden AntiCheat");
                return false;
            }

            // save prevoius point
            Player::SavePositionInDB(mover->GetObjectGuid(), mover->GetMapId(), mover->m_movementInfo.pos.x, mover->m_movementInfo.pos.y, mover->m_movementInfo.pos.z, mover->m_movementInfo.pos.o, mover->GetZoneId());
            me->GetSession()->KickPlayer();
            /*if (distance > 150.0f)
                sWorld.BanAccount(BAN_ACCOUNT, me->GetSession()->GetUsername(), DAY, "Cheat detected", "Warden AntiCheat");
            else
                me->GetSession()->KickPlayer();*/
            return false;
        }
    }

    // in launched/falling/spline movement - maybe false positives
    if (!mover->IsLaunched() && !mover->IsFalling() && (!mover->movespline || mover->movespline->Finalized()))
    {
        auto const moveFlags = movementInfo.GetMovementFlags();
        uint32 removeMoveFlags = 0;

        if ((moveFlags & MOVEFLAG_WALK_MODE) && (moveFlags & MOVEFLAG_FIXED_Z) && (moveFlags & MOVEFLAG_SWIMMING) && (moveFlags & MOVEFLAG_HOVER)
            && (moveFlags & MOVEFLAG_FALLINGFAR) && (moveFlags & MOVEFLAG_FLYING) && (moveFlags & MOVEFLAG_ONTRANSPORT))
        {
            AddCheats(1 << CHEAT_TYPE_FLY_HACK_SWIM);
            removeMoveFlags |= moveFlags;
            sLog.outWarden("SERVER WARDEN (MovementFlags hack): player %s had old flyhack moveFlags mask", me->GetSession()->GetPlayerName());
        }

        // no need to check pending orders for this.  players should never levitate.
        if (moveFlags & MOVEFLAG_LEVITATING)
        {
            AddCheats(1 << CHEAT_TYPE_FLY_HACK_SWIM);
            removeMoveFlags |= MOVEFLAG_LEVITATING;
            sLog.outWarden("SERVER WARDEN (MovementFlags hack): player %s had MOVEFLAG_LEVITATING (only for creatures!)", me->GetSession()->GetPlayerName());
        }

        // detect new flyhack (these two flags should never happen at the same time)
        if ((moveFlags & MOVEFLAG_SWIMMING) && (moveFlags & MOVEFLAG_FLYING))
        {
            AddCheats(1 << CHEAT_TYPE_FLY_HACK_SWIM);
            removeMoveFlags |= MOVEFLAG_SWIMMING | MOVEFLAG_FLYING;
            sLog.outWarden("SERVER WARDEN (MovementFlags hack): player %s had MOVEFLAG_SWIMMING and MOVEFLAG_FLYING", me->GetSession()->GetPlayerName());
        }

        if (moveFlags & MOVEFLAG_FIXED_Z)
        {
            removeMoveFlags |= MOVEFLAG_FIXED_Z;
            sLog.outWarden("SERVER WARDEN (MovementFlags hack): player %s had MOVEFLAG_FIXED_Z", me->GetSession()->GetPlayerName());
        }

        // if water walking with no aura and no pending removal order, cheater
        if (moveFlags & MOVEFLAG_WATERWALKING && !me->HasAuraType(SPELL_AURA_WATER_WALK) && !me->HasAuraType(SPELL_AURA_GHOST))
        {
            for (auto const &order : _orders)
                if (order.clientResp == CMSG_MOVE_WATER_WALK_ACK)
                {
                    if (!order.counter)
                    {
                        sLog.outWarden("SERVER WARDEN (MovementFlags hack): player %s had MOVEFLAG_WATERWALKING with no water walk aura and no pending orders", me->GetSession()->GetPlayerName());
                        AddCheats(1 << CHEAT_TYPE_WATER_WALK);
                    }

                    break;
                }

            removeMoveFlags |= MOVEFLAG_WATERWALKING;
        }

        // if safe falling with no aura and no pending removal order, cheater
        if (moveFlags & MOVEFLAG_SAFE_FALL && !me->HasAuraType(SPELL_AURA_FEATHER_FALL))
        {
            for (auto const &order : _orders)
            {
                if (order.clientResp == CMSG_MOVE_FEATHER_FALL_ACK)
                {
                    if (!order.counter)
                    {
                        sLog.outWarden("SERVER WARDEN (MovementFlags hack): player %s had MOVEFLAG_SAFE_FALL with no slow fall aura and no pending orders", me->GetSession()->GetPlayerName());
                        AddCheats(1 << CHEAT_TYPE_SLOW_FALL);
                    }

                    break;
                }
            }

            removeMoveFlags |= MOVEFLAG_SAFE_FALL;
        }

        if (removeMoveFlags)
            movementInfo.RemoveMovementFlag(MovementFlags(removeMoveFlags));
    }

    return true;
}

bool PlayerCheatData::IsTeleportAllowed(MovementInfo const& movementInfo, float& distance)
{
    // check valid source coordinates
    if (me->GetPositionX() == 0.0f || me->GetPositionY() == 0.0f || me->GetPositionZ() == 0.0f)
        return true;

    // check valid destination coordinates
    if (movementInfo.GetPos()->x == 0.0f || movementInfo.GetPos()->y == 0.0f || movementInfo.GetPos()->z == 0.0f)
        return true;

    // if knockbacked
    if (me->IsLaunched())
        return true;

    // ignore valid teleport state
    if (me->IsBeingTeleported())
        return true;

    // some exclude zones - lifts and other, but..
    uint32 destZoneId = 0;
    uint32 destAreaId = 0;

    me->GetTerrain()->GetZoneAndAreaId(destZoneId, destAreaId, movementInfo.pos.x, movementInfo.pos.y, movementInfo.pos.z);

    // checks far teleports
    if (destZoneId == me->GetZoneId() && destAreaId == me->GetAreaId())
    {
        // Thousand Needles - Great Lift
        if (me->GetZoneId() == 2257 || (me->GetZoneId() == 400 && me->GetAreaId() == 485))
            return true;

        // Undercity Lift
        if (me->GetZoneId() == 1497 && me->GetAreaId() == 1497)
            return true;
    }

    float deltaX = me->GetPositionX() - movementInfo.pos.x;
    float deltaY = me->GetPositionY() - movementInfo.pos.y;
    float deltaZ = me->GetPositionZ() - movementInfo.pos.z;
    distance = sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);

    if (distance < 40.0f)
        return true;

    return false;
}
