
#include <zlib/zlib.h>
#include "WardenKeyGeneration.h"
#include "Common.h"
#include "Player.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "World.h"
#include "Log.h"
#include "SpellAuras.h"
#include "Opcodes.h"
#include "ByteBuffer.h"
#include <openssl/md5.h>
#include <openssl/sha.h>
#include "Database/DatabaseEnv.h"
#include "Policies/SingletonImp.h"
#include "Auth/BigNumber.h"
#include "Auth/HMACSHA1.h"
#include "WardenWin.h"
#include "WardenStorageMgr.h"
#include "WardenModuleWin.h"
#include "Util.h"

#include "MoveSpline.h"

WardenWin::WardenWin() : Warden()
{
    ClearAlerts();
    ClearOffsets();
}

WardenWin::~WardenWin()
{
}

void WardenWin::Init(WorldSession* pClient, BigNumber* k)
{
    // set session, build and module
    _session = pClient;
    _module = GetModuleForClient();
    clientBuild = _session->GetGameBuild();

    if (!_module)
    {
        sLog.outWarden("SERVER WARDEN: Current module is failed. Abort Warden! Account %u (%s)", _session->GetAccountId(), _session->GetUsername().c_str());
        return;
    }

    auto kBytes = k->AsByteArray();
    SHA1Randx WK(kBytes.data(), kBytes.size());
    WK.Generate(_clientKeySeed, 16);
    WK.Generate(_serverKeySeed, 16);

    _inputCrypto.Init(_clientKeySeed);
    _outputCrypto.Init(_serverKeySeed);

    sLog.outDebug("SERVER WARDEN: Serverside warden for client %u initializing...", _session->GetAccountId());
    sLog.outDebug("SERVER WARDEN: Loading Module...");

	// send packet request to client
    RequestModule();
}

WardenModule* WardenWin::GetModuleForClient()
{
    WardenModule *mod = new WardenModule;

    uint32 length = sizeof(WinModule.Module);

    // data assign
    mod->CompressedSize = length;
    mod->CompressedData = new uint8[length];
    memcpy(mod->CompressedData, WinModule.Module, length);
    memcpy(mod->Key, WinModule.ModuleKey, 16);

    // md5 hash
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, mod->CompressedData, length);
    MD5_Final((uint8*)&mod->ID, &ctx);

    return mod;
}

void WardenWin::InitializeModule()
{
    BigNumber n;
    switch(clientBuild)
    {
        // 1.12.1
        case 5875:
        {
            n.SetHexStr("01000200A0772400F08724006084240030872400040000F03B30000001010010C0020001");
            break;
        }
        // 1.12.2
        case 6005:
        {
            n.SetHexStr("01000200A0772400F08724006084240030872400040000203C30000001010010C0020001");
            break;
        }
        // 1.12.3
        case 6141:
        {
            n.SetHexStr("01000200409B240090AB240000A82400D0AA2400040000C05F30000001010010C0020001");
            break;
        }
        default:
        {
            sLog.outWarden("SERVER WARDEN: Right client build are not detected! Account kicked! Maybe cheating!");
            _session->KickPlayer();
            return;
        }
    }

    sLog.outDebug("SERVER WARDEN: Initialize module...");

    // hex string -> byte[]
    ByteBuffer buff_b, buff_c;
    auto temp = n.AsByteArray(0, false);
    buff_c.append(temp);

    buff_b << uint8(0x03);
    buff_b << uint16(20);
    buff_b << uint32(BuildChecksum(buff_c.contents(), 20));
    buff_b.append(buff_c.contents(), 20);

    buff_b << uint8(0x03);
    buff_b << uint16(8);
    buff_b << uint32(BuildChecksum(buff_c.contents() + 20, 8));
    buff_b.append(buff_c.contents() + 20, 8);

    buff_b << uint8(0x03);
    buff_b << uint16(8);
    buff_b << uint32(BuildChecksum(buff_c.contents() + 28, 8));
    buff_b.append(buff_c.contents() + 28, 8);

    // Encrypt with warden RC4 key.
    EncryptData(const_cast<uint8*>(buff_b.contents()), buff_b.size());

    WorldPacket pkt(SMSG_WARDEN_DATA, buff_b.size());
    pkt.append(buff_b);
    _session->SendPacket(&pkt);
}

void WardenWin::TestInit()
{
    BigNumber n;
    n.SetHexStr("01000200A0772400F08724006084240030872400040000409D41000101010010C0020001");

    //sLog.outDebug("SERVER WARDEN: Initialize module
    //uint32 callFunc = s_currentModule + 0x5CBC;
    uint32 callFunc = 0x5ABE10;
    callFunc = callFunc - 0x400000;
    uint8 result[8];
    
    result[0] = 0x01;
    result[1] = 0x01;
    result[2] = 0x00;
    result[3] = (callFunc & 0x000000ff);
    result[4] = (callFunc & 0x0000ff00) >> 8;
    result[5] = (callFunc & 0x00ff0000) >> 16;
    result[6] = (callFunc & 0xff000000) >> 24;
    result[7] = 0x01;

    // hex string -> byte[]
    ByteBuffer buff_b, buff_c;
    auto temp = n.AsByteArray(0, false);
    buff_c.append(temp);

    buff_b << uint8(0x03);
    buff_b << uint16(20);
    buff_b << uint32(BuildChecksum(buff_c.contents(), 20));
    buff_b.append(buff_c.contents(), 20);

    buff_b << uint8(0x03);
    buff_b << uint16(8);
    buff_b << uint32(BuildChecksum(buff_c.contents() + 20, 8));
    buff_b.append(buff_c.contents() + 20, 8);

    buff_b << uint8(0x03);
    buff_b << uint16(8);
    buff_b << uint32(BuildChecksum(result, 8));
    buff_b.append(result, 8);

    // Encrypt with warden RC4 key.
    EncryptData(const_cast<uint8*>(buff_b.contents()), buff_b.size());

    WorldPacket pkt(SMSG_WARDEN_DATA, buff_b.size());
    pkt.append(buff_b);
    _session->SendPacket(&pkt);
}

void WardenWin::RequestHash(bool reInitialize)
{
    sLog.outDebug("SERVER WARDEN: Initial Warden Request Hash...");

    if (reInitialize)
        sLog.outWarden("CLIENT WARDEN: Player %s (account: %u) re-initialize module", _session->GetPlayerName(), _session->GetAccountId());

    // Create packet structure
    WardenHashRequest Request;
    Request.Command = 0x05;
    memcpy(Request.Seed, WinModule.Seed, 16);

    // Encrypt with warden RC4 key.
    EncryptData((uint8*)&Request, sizeof(WardenHashRequest));

    WorldPacket pkt(SMSG_WARDEN_DATA, sizeof(WardenHashRequest));
    pkt.append((uint8*)&Request, sizeof(WardenHashRequest));
    _session->SendPacket(&pkt);
}

void WardenWin::HandleHashResult(ByteBuffer &buff)
{
    buff.rpos(buff.wpos());

    // Verify key
    if (memcmp(buff.contents() + 1, WinModule.ClientKeySeedHash, 20) != 0)
    {
        sLog.outWarden("SERVER WARDEN: Hash check failed! Account %u kicked! Maybe cheating!", _session->GetAccountId());
        _session->KickPlayer();
    }

    // Change keys here
    memcpy(_clientKeySeed, WinModule.ClientKeySeed, 16);
    memcpy(_serverKeySeed, WinModule.ServerKeySeed, 16);

    _inputCrypto.Init(_clientKeySeed);
    _outputCrypto.Init(_serverKeySeed);

    sLog.outDebug("SERVER WARDEN: Keys are successfully changed!");

	_initialized = true;
}

void WardenWin::RequestData()
{
    sLog.outDebug("SERVER WARDEN: Cheat base checks was STARTED!");

    // If all checks were done, fill the todo list again
    if (_memChecksTodo.empty())
        _memChecksTodo.assign(_wardenStorage.MemChecksIdPool[clientBuild].begin(), _wardenStorage.MemChecksIdPool[clientBuild].end());

    if (_otherChecksTodo.empty())
        _otherChecksTodo.assign(_wardenStorage.OtherChecksIdPool[clientBuild].begin(), _wardenStorage.OtherChecksIdPool[clientBuild].end());

    _serverTicks = WorldTimer::getMSTime();

    uint16 id;
    uint8 type;
    WardenCheck* wd;

    _currentChecks.clear();

    ByteBuffer buff;
    buff << uint8(0x02);

    // Build check request
    for (uint8 i = 0; i < m_memChecksCount; ++i)
    {
        // If todo list is done break loop (will be filled on next Update() run)
        if (_memChecksTodo.empty())
            break;

        // Get check id from the random and remove it from todo
        std::vector<uint16>::iterator itr = GetRandomCheckFromList(_memChecksTodo.begin(), _memChecksTodo.end());
        id = (*itr);
        _memChecksTodo.erase(itr);

        // Add the id to the list sent in this cycle
        _currentChecks.push_back(id);

        wd = _wardenStorage.GetCheckDataById(id, clientBuild);

        if (wd->type == MPQ_CHECK)
        {
            buff << uint8(wd->str.size());
            buff.append(wd->str.c_str(), wd->str.size());
        }
    }

    ACE_READ_GUARD(ACE_RW_Mutex, g, _wardenStorage._checkStoreLock);

    for (uint8 i = 0; i < m_otherChecksCount; ++i)
    {
        // If todo list is done break loop (will be filled on next Update() run)
        if (_otherChecksTodo.empty())
            break;

        // Get check id from random and remove it from todo
        std::vector<uint16>::iterator itr = GetRandomCheckFromList(_otherChecksTodo.begin(), _otherChecksTodo.end());
        id = (*itr);
        _otherChecksTodo.erase(itr);

        // Add the id to the list sent in this cycle
        _currentChecks.push_back(id);

        wd = _wardenStorage.GetCheckDataById(id, clientBuild);

        switch (wd->type)
        {
            case LUA_STR_CHECK:
            case DRIVER_CHECK:
                buff << uint8(wd->str.size());
                buff.append(wd->str.c_str(), wd->str.size());
                break;
            default:
                break;
        }
    }

    uint8 xorByte = _clientKeySeed[0];

    // Add TIMING_CHECK
    buff << uint8(0x00);
    buff << uint8(TIMING_CHECK ^ xorByte);

    uint8 index = 1;

    // header
    wd = _wardenStorage.GetCheckDataById(1, clientBuild, true);

    buff << uint8(MEM_CHECK ^ xorByte);
    buff << uint8(0x00);
    buff << uint32(wd->address);
    buff << uint8(wd->length);

    // test native calls of module function - DISABLED IT
    /*if (s_currentModule == 0x00 && s_currentModulePtr == 0x00)
    {
        buff << uint8(MEM_CHECK ^ xorByte);
        buff << uint8(0x00);
        buff << uint32(0x00CE8978);
        buff << uint8(4);
    }
    else if (s_currentModule == 0x00)
    {
        buff << uint8(MEM_CHECK ^ xorByte);
        buff << uint8(0x00);
        buff << uint32(s_currentModulePtr);
        buff << uint8(4);
    }*/

    for (std::list<uint16>::iterator itr = _currentChecks.begin(); itr != _currentChecks.end(); ++itr)
    {
        wd = _wardenStorage.GetCheckDataById(*itr, clientBuild);

        type = wd->type;
        buff << uint8(type ^ xorByte);
        switch (type)
        {
            case MEM_CHECK:
            {
                buff << uint8(0x00);
                buff << uint32(wd->address);
                buff << uint8(wd->length);
                break;
            }
            case PAGE_CHECK_A:
            case PAGE_CHECK_B:
            {
                auto data = wd->data.AsByteArray(0, false);
                buff.append(data);
                buff << uint32(wd->address);
                buff << uint8(wd->length);
                break;
            }
            case MPQ_CHECK:
            case LUA_STR_CHECK:
            {
                buff << uint8(index++);
                break;
            }
            case DRIVER_CHECK:
            {
                auto data = wd->data.AsByteArray(0, false);
                buff.append(data);
                buff << uint8(index++);
                break;
            }
            case MODULE_CHECK:
            {
                uint32 seed = static_cast<uint32>(rand32());
                buff << uint32(seed);
                HMACSHA1 hmac(4, (uint8*)&seed);
                hmac.UpdateData(wd->str);
                hmac.Finalize();
                buff.append(hmac.GetDigest(), hmac.GetLength());
                break;
            }
            /*case PROC_CHECK:
            {
                buff.append(wd->data.AsByteArray(0, false), wd->data.GetNumBytes());
                buff << uint8(index++);
                buff << uint8(index++);
                buff << uint32(wd->address);
                buff << uint8(wd->length);
                break;
            }*/
            default:
                break;                                      // Should never happen
        }
    }
    buff << uint8(xorByte);

    // Encrypt with warden RC4 key
    EncryptData(const_cast<uint8*>(buff.contents()), buff.size());

    WorldPacket pkt(SMSG_WARDEN_DATA, buff.size());
    pkt.append(buff);
    _session->SendPacket(&pkt);

    ++m_sendLastPacketCount;

    ++m_synIndex;
    m_reqAckIndex = m_synIndex;
    //sLog.outWarden("server packet number - %u, required client packet number - %u", m_PacketNumber_S, m_reqClientPacketNumber);

    std::stringstream stream;
    stream << "WARDEN: Sent check id's: ";
    for (std::list<uint16>::iterator itr = _currentChecks.begin(); itr != _currentChecks.end(); ++itr)
        stream << *itr << " ";

    //sLog.outWarden("%s", stream.str().c_str());
}

void WardenWin::RequestData2()
{
    ByteBuffer buff;
    buff << uint8(0x02);

    uint8 xorByte = _clientKeySeed[0];

    buff << uint8(0x00);
    buff << uint8(TIMING_CHECK ^ xorByte);                  // check TIMING_CHECK

    // header
    WardenCheck* wd = _wardenStorage.GetCheckDataById(2, clientBuild, true);

    buff << uint8(MEM_CHECK ^ xorByte);
    buff << uint8(0x00);
    buff << uint32(wd->address);
    buff << uint8(wd->length);

    // construct dynamic part of packet
    buff << uint8(MEM_CHECK ^ xorByte);
    buff << uint8(0x00);

    bool dataCreate = false;

    // first packet in chain
    if (playerBase == 0x00 && offset == 0x00 && playerMovementBase == 0x00)
    {
        wd = _wardenStorage.GetCheckDataById(3, clientBuild, true);
        buff << uint32(wd->address);
        buff << uint8(wd->length);
        dataCreate = true;
    }
    // second packet in chain
    else if (playerBase != 0x00 && offset == 0x00 && playerMovementBase == 0x00)
    {
        wd = _wardenStorage.GetCheckDataById(4, clientBuild, true);
        playerBase += wd->address;
        buff << uint32(playerBase);
        buff << uint8(wd->length);
        dataCreate = true;
    }
    // third packet in chain
    else if (playerBase != 0x00 && offset != 0x00 && playerMovementBase == 0x00)
    {
        wd = _wardenStorage.GetCheckDataById(5, clientBuild, true);
        offset += wd->address;
        buff << uint32(offset);
        buff << uint8(wd->length);
        dataCreate = true;
    }
    else if (playerBase != 0x00 && offset != 0x00 && playerMovementBase != 0x00)
    {
        // sended for correct select of base packet
        buff << uint32(playerMovementBase);
        buff << uint8(0x04);

        uint32 m_flags = playerMovementBase + 0x9E8;
        buff << uint8(MEM_CHECK ^ xorByte);
        buff << uint8(0x00);
        buff << uint32(m_flags);
        buff << uint8(0x04);

        uint32 speed = playerMovementBase + 0xA2C;
        buff << uint8(MEM_CHECK ^ xorByte);
        buff << uint8(0x00);
        buff << uint32(speed);
        buff << uint8(0x04);

        uint32 speed_r = playerMovementBase + 0xA34;
        buff << uint8(MEM_CHECK ^ xorByte);
        buff << uint8(0x00);
        buff << uint32(speed_r);
        buff << uint8(0x04);

        uint32 wall = playerMovementBase + 0xA60;
        buff << uint8(MEM_CHECK ^ xorByte);
        buff << uint8(0x00);
        buff << uint32(wall);
        buff << uint8(0x04);

        uint32 plHealth = playerMovementBase + 0x1DC8;
        buff << uint8(MEM_CHECK ^ xorByte);
        buff << uint8(0x00);
        buff << uint32(plHealth);
        buff << uint8(0x04);

        uint32 falltime = playerMovementBase + 0xA20;
        buff << uint8(MEM_CHECK ^ xorByte);
        buff << uint8(0x00);
        buff << uint32(falltime);
        buff << uint8(0x04);

        // tracking check temporary disabled
        /*uint32 track = checkedClient->GetPlayerStates(2) + 0x2EB0;
        data << uint8(MEM_CHECK ^ xorByte);
        data << uint8(0x00);
        data << uint32(track);
        data << uint8(0x08);*/

        dataCreate = true;
    }

    if (dataCreate)
    {
        buff << uint8(xorByte);

        // Encrypt with warden RC4 key.
        EncryptData(const_cast<uint8*>(buff.contents()), buff.size());

        WorldPacket pkt(SMSG_WARDEN_DATA, buff.size());
        pkt.append(buff);
        _session->SendPacket(&pkt);

        // inc packet count
        ++m_sendLastPacketCount2;

        //sLog.outWarden("server packet number 2 - %u, required client packet number 2 - %u", m_PacketNumber2_S, m_reqClientPacketNumber2);
    }
}

std::string WardenWin::Penalty(WardenCheck* rd, std::string realData, std::string packetData)
{
    uint8 action = rd->action;
    if (rd->reason == "")
        rd->reason = "Unknown check";

    // ----> to Warden.log
    sLog.outWarden("Penalty: %s (accountID - %u, accountName - %s, playerName - %s, clientBuild - %u, groupId - %u, action - %u, realData - %s, failedData - %s)", 
        rd->reason.c_str(), _session->GetAccountId(), _session->GetUsername().c_str(), _session->GetPlayerName(), clientBuild, rd->groupId, rd->action, realData.c_str(), packetData.c_str());

    switch (action)
    {
        case 0: return "Log";
        case 1:
        {
            _session->KickPlayer();
            return "Kick";
        }
        case 2:
        {
            std::string accountName = _session->GetUsername();
            std::ostringstream banReason;
            banReason << "Cheat detected";
            //_session->UpdateBanAttempts(_session->GetAccountId());
            sWorld.BanAccount(BAN_ACCOUNT, accountName, 0, banReason.str(), "Warden AntiCheat");
            return "Ban";
        }
    }

    return "Undefined";
}

void WardenWin::HandleData(ByteBuffer &buff)
{
    sLog.outDebug("CLIENT WARDEN: Raw Packet Data");

    uint16 length;
    uint32 checksum;
    buff >> length;
    buff >> checksum;

    if (!IsValidCheckSum(checksum, buff.contents() + buff.rpos(), length))
    {
        buff.rpos(buff.wpos());
        sLog.outWarden("CLIENT WARDEN : Packet checksum FAIL! Abort parsing! Account %u" , _session->GetAccountId());
        _session->KickPlayer();
        return;
    }

    //TIMING_CHECK
    uint8 result;
    buff >> result;

    if (result == 0x00)
    {
        sLog.outWarden("WARDEN: timing check failed! Account %u", _session->GetAccountId());
        _session->KickPlayer();
        return;
    }

    uint32 newClientTicks;
    buff >> newClientTicks;

    uint32 ticksNow = WorldTimer::getMSTime();
    uint32 ourTicks = newClientTicks + (ticksNow - _serverTicks);

    // read header
    uint8 Head_Result;
    buff >> Head_Result;

    if (Head_Result != 0)
    {
        buff.rpos(buff.wpos());
        sLog.outWarden("WARDEN: Player %s (account: %u) failed read Warden packet header. Player kicked", _session->GetPlayerName(), _session->GetAccountId());
        _session->KickPlayer();
        return;
    }

    uint8 sign[6];
    buff.read(sign, 6);
    std::string packetSign = ConvertPacketDataToString(sign, 6);

    WardenCheckResult* wr = _wardenStorage.GetCheckResultById(1, clientBuild, true);
    std::string staticCheckSign = wr->result;

    // static checks: verify header not equal kick player
    if (!strcmp(packetSign.c_str(), staticCheckSign.c_str()))
    {
        HandleData2(buff);
        return;
    }

    wr = _wardenStorage.GetCheckResultById(2, clientBuild, true);
    std::string dynamicCheckSign = wr->result;

    // dynamic checks: verify header not equal kick player
    if (!strcmp(packetSign.c_str(), dynamicCheckSign.c_str()))
    {
        HandleData3(buff);
        return;
    }

    buff.rpos(buff.wpos());
    sLog.outWarden("WARDEN: Player %s (account: %u) failed check Warden packet header. Player kicked",
        _session->GetPlayerName(), _session->GetAccountId());
    _session->KickPlayer();
}

void WardenWin::HandleData2(ByteBuffer &buff)
{
    ++m_ackIndex;
    //sLog.outWarden("client packet number - %u, required client packet number - %u", m_ackIndex, m_reqAckIndex);
    m_sendLastPacketCount = 0;

    if (m_ackIndex != m_reqAckIndex)
    {
        buff.rpos(buff.wpos());
        _checkTimer += 30*IN_MILLISECONDS;
        //sLog.outWarden("Player %s (account: %u) (latency: %u, IP: %s) failed sync static packets current: %u (req: %u)",
            //_session->GetPlayerName(), _session->GetAccountId(), _session->GetLatency(), _session->GetRemoteAddress().c_str(), m_ackIndex, m_reqAckIndex);
        ++m_failedSyncPacketCount;
        return;
    }

    WardenCheckResult *rs;
    WardenCheck *rd;
    uint8 type;

    // test native calls of module function - DISABLED IT
    /*if (s_currentModule == 0x00 && s_currentModulePtr == 0x00)
    {
        uint8 memResult;
        buff >> memResult;

        if (memResult != 0)
        {
            //sLog.outWarden("RESULT MEM_CHECK not 0x00, CheckId %u account Id %u", *itr, _session->GetAccountId());
            buff.rpos(buff.wpos());
            //sLog.outWarden("Player %s (account: %u) failed Warden check %u. Action: %s", _session->GetPlayerName(), _session->GetAccountId(), *itr, Penalty(rd).c_str());
            _session->KickPlayer();
            return;
        }

        buff >> s_currentModulePtr;
    }
    else if (s_currentModule == 0x00)
    {
        uint8 memResult;
        buff >> memResult;

        if (memResult != 0)
        {
            //sLog.outWarden("RESULT MEM_CHECK not 0x00, CheckId %u account Id %u", *itr, _session->GetAccountId());
            buff.rpos(buff.wpos());
            //sLog.outWarden("Player %s (account: %u) failed Warden check %u. Action: %s", _session->GetPlayerName(), _session->GetAccountId(), *itr, Penalty(rd).c_str());
            _session->KickPlayer();
            return;
        }

        buff >> s_currentModule;
    }*/

    ACE_READ_GUARD(ACE_RW_Mutex, g, _wardenStorage._checkStoreLock);
    for (std::list<uint16>::iterator itr = _currentChecks.begin(); itr != _currentChecks.end(); ++itr)
    {
        rd = _wardenStorage.GetCheckDataById(*itr, clientBuild);
        rs = _wardenStorage.GetCheckResultById(*itr, clientBuild);

        type = rd->type;
        switch (type)
        {
            case MEM_CHECK:
            {
                uint8 memResult;
                buff >> memResult;

                if (memResult != 0)
                {
                    sLog.outWarden("RESULT MEM_CHECK not 0x00, CheckId %u account Id %u", *itr, _session->GetAccountId());
                    buff.rpos(buff.wpos());
                    //sLog.outWarden("Player %s (account: %u) failed Warden check %u. Action: %s", _session->GetPlayerName(), _session->GetAccountId(), *itr, Penalty(rd).c_str());
                    _session->KickPlayer();
                    return;
                }

                std::string packet_data = ConvertPacketDataToString(buff.contents() + buff.rpos(), rd->length);
                std::string result = rs->result;
                std::string exResult = rs->result2;

                bool pen = false;

                if (exResult != "")
                {
                    // only equal result, strict compare
                    if (!strcmp(exResult.c_str(), packet_data.c_str()))
                        pen = true;
                }
                else if (exResult == "")
                {
                    // not equal result, soft compare
                    if (strcmp(result.c_str(), packet_data.c_str()))
                        pen = true;
                }

                if (pen)
                {
                    sLog.outWarden("RESULT MEM_CHECK fail CheckId %u account Id %u", *itr, _session->GetAccountId());
                    buff.rpos(buff.wpos());
                    sLog.outWarden("Player %s (account: %u) failed Warden check %u. Action: %s", _session->GetPlayerName(), _session->GetAccountId(), *itr, Penalty(rd, result, packet_data).c_str());
                    return;
                }

                buff.rpos(buff.rpos() + rd->length);
                break;
            }
            case PAGE_CHECK_A:
            case PAGE_CHECK_B:
            case DRIVER_CHECK:
            case MODULE_CHECK:
            {
                std::string packet_data = ConvertPacketDataToString(buff.contents() + buff.rpos(), 1);
                std::string result = rs->result;
                if (strcmp(result.c_str(), packet_data.c_str()))
                {
                    if (type == PAGE_CHECK_A || type == PAGE_CHECK_B)
                        sLog.outWarden("RESULT PAGE_CHECK fail, CheckId %u, account Id %u", *itr, _session->GetAccountId());
                    if (type == MODULE_CHECK)
                        sLog.outWarden("RESULT MODULE_CHECK fail, CheckId %u, account Id %u", *itr, _session->GetAccountId());
                    if (type == DRIVER_CHECK)
                        sLog.outWarden("RESULT DRIVER_CHECK fail, CheckId %u, account Id %u", *itr, _session->GetAccountId());

                    buff.rpos(buff.wpos());
                    sLog.outWarden("Player %s (account: %u) failed Warden check %u. Action: %s", _session->GetPlayerName(), _session->GetAccountId(), *itr, Penalty(rd, result, packet_data).c_str());
                    return;
                }

                buff.rpos(buff.rpos() + 1);
                break;
            }
            case LUA_STR_CHECK:
            {
                uint8 luaResult;
                buff >> luaResult;

                if (luaResult != 0)
                    sLog.outWarden("unk byte in LUA_STR_CHECK is equal 1, CheckId %u account Id %u", *itr, _session->GetAccountId());

                uint8 luaStrLen;
                buff >> luaStrLen;

                if (luaStrLen != 0)
                {
                    char *str = new char[luaStrLen + 1];
                    memset(str, 0, luaStrLen + 1);
                    memcpy(str, buff.contents() + buff.rpos(), luaStrLen);
                    sLog.outWarden("RESULT LUA_STR_CHECK fail, CheckId %u account Id %u", *itr, _session->GetAccountId());
                    sLog.outWarden("Lua string found: %s", str);
                    delete[] str;

                    buff.rpos(buff.wpos());
                    sLog.outWarden("Player %s (account: %u) failed Warden check %u. Action: %s", _session->GetPlayerName(), _session->GetAccountId(), *itr, Penalty(rd).c_str());
                    return;
                }

                break;
            }
            case MPQ_CHECK:
            {
                uint8 mpqResult;
                buff >> mpqResult;

                if (mpqResult != 0)
                {
                    sLog.outWarden("RESULT MPQ_CHECK not 0x00 account id %u", _session->GetAccountId());
                    buff.rpos(buff.wpos());
                    //sLog.outWarden("Player %s (account: %u) failed Warden check %u. Action: %s", _session->GetPlayerName(), _session->GetAccountId(), *itr, Penalty(rd).c_str());
                    _session->KickPlayer();
                    return;
                }

                std::string packet_data = ConvertPacketDataToString(buff.contents() + buff.rpos(), 20);
                std::string result = rs->result;
                if (strcmp(rs->result.c_str(), packet_data.c_str()))
                {
                    sLog.outWarden("RESULT MPQ_CHECK fail, CheckId %u account Id %u", *itr, _session->GetAccountId());
                    buff.rpos(buff.wpos());
                    sLog.outWarden("Player %s (account: %u) failed Warden check %u. Action: %s", _session->GetPlayerName(), _session->GetAccountId(), *itr, Penalty(rd, result, packet_data).c_str());
                    return;
                }

                buff.rpos(buff.rpos() + 20);                // 20 bytes SHA1
                break;
            }
            default:                                        // Should never happen
                break;
        }
    }
}

void WardenWin::HandleData3(ByteBuffer &buff)
{
    //sLog.outWarden("client packet number 2 - %u, required client packet number 2 - %u", m_PacketNumber2_C, m_reqClientPacketNumber2);
    m_sendLastPacketCount2 = 0;

    uint8 Mem_Result;
    buff >> Mem_Result;
    
    if (Mem_Result != 0x00)
    {
        buff.rpos(buff.wpos());
        // for debug
        //sLog->outWarden("RESULT MEM_CHECK not 0x00(get base data), Special check failed on account Id %u", _session->GetAccountId());
        // nulled
        ClearOffsets();
        return;
    }

    // first packet in chain
    if (playerBase == 0x00 && offset == 0x00 && playerMovementBase == 0x00)
    {
        buff >> playerBase;
        _checkTimer2 = 1*IN_MILLISECONDS;
        return;
    }

    // second packet in chain
    if (playerBase != 0x00 && offset == 0x00 && playerMovementBase == 0x00)
    {
        buff >> offset;
        _checkTimer2 = 1*IN_MILLISECONDS;
        return;
    }
    
    // third packet in chain
    if (playerBase != 0x00 && offset != 0x00 && playerMovementBase == 0x00)
    {
        buff >> playerMovementBase;
        _checkTimer2 = 1*IN_MILLISECONDS;
        return;
    }

    // data from client
    if (playerBase != 0x00 && offset != 0x00 && playerMovementBase != 0x00)
    {
        uint32 data;
        buff >> data;

        // check some pointer from client
        if (data != 0x80AF78)
        {
            buff.rpos(buff.wpos());
            // for debug
            //sLog.outWarden("Check Data - %X on account %u, build - %u", check_data, _session->GetAccountId(), clientBuild);
            ClearOffsets();
            // forced timer
            _checkTimer2 = 1*IN_MILLISECONDS;
            return;
        }

        // initialize temp variables
        uint8 speedAlertsBase = m_currentSpeedHackAlert;
        uint8 speedExtAlertsBase = m_baseSpeedHackAlert;
        //uint8 moveFlagsAlertsBase = m_moveFlagsAlert;

        uint32 moveFlags = 0;
        if (!ReadMemChunk(buff, moveFlags))
            return;

        float curClientSpeed = 0.0f;
        if (!ReadMemChunk(buff, curClientSpeed))
            return;

        float baseRunClientSpeed = 0.0f;
        if (!ReadMemChunk(buff, baseRunClientSpeed))
            return;

        float wallClimb = 0.0f;
        if (!ReadMemChunk(buff, wallClimb))
            return;

        uint32 plHealth = 0;
        if (!ReadMemChunk(buff, plHealth))
            return;

        uint32 fallTime = 0;
        if (!ReadMemChunk(buff, fallTime))
            return;

        // TODO: move checks to server anticheat module, Warden for renew data OR compact checks this
        Player * plr = _session->GetPlayer();

        if (!plr || !plr->IsInWorld() || plr->IsBeingTeleported())
            return;

        // DoSpeedChecks()
        MoveType mtype = SelectSpeedType(moveFlags);

        if (mtype != MOVETYPE_NONE)
        {
            UnitMoveType m_type = UnitMoveType(mtype);
            // exclude taxi and spline - not needed
            if (!plr->IsTaxiFlying() && !plr->movespline->Finalized())
            {
                float serverSpeed = plr->GetSpeed(m_type);

                // log launched speed deviations
                if (plr->IsLaunched())
                {
                    if (plr->GetXYSpeed() > 0.0f)
                    {
                        if (curClientSpeed > plr->GetXYSpeed())
                            sLog.outWarden("CLIENT WARDEN: Player %s (account: %u) in launched state has over current speed %f (launched speed %f)", _session->GetPlayerName(), _session->GetAccountId(), curClientSpeed, plr->GetXYSpeed());
                    }
                }
                // check exlcude launched state
                else
                {
                    // check current speed
                    if (curClientSpeed > 7.0f && curClientSpeed > serverSpeed)
                    {
                        if (curClientSpeed >= 20.0f)
                        {
                            ++m_currentSpeedHackAlert;
                            sLog.outWarden("CLIENT WARDEN: Player %s (account: %u) warning(!!!) - current speed %f much more valid %f", _session->GetPlayerName(), _session->GetAccountId(), curClientSpeed, serverSpeed);
                            //_session->KickPlayer();
                            //sWorld.BanAccount(BAN_ACCOUNT_ID, _session->GetUsername(), 0, "Cheat detected (CheckId: 255)", "Warden AntiCheat", _session->GetAccountId());
                            //return;
                        }

                        //if (!(plr->m_movementInfo.GetMovementFlags() & (MOVEFLAG_FALLING | MOVEFLAG_FALLINGFAR)) && !plr->IsFalling())
                            //++m_currentSpeedHackAlert;

                        //if (m_currentSpeedHackAlert >= (int32)sWorld.getConfig(CONFIG_UINT32_WARDEN_SPEED_ALERT_COUNT))
                        if (m_currentSpeedHackAlert >= 3)
                        {
                            sLog.outWarden("CLIENT WARDEN: Player %s (account: %u) must be banned - speedhack type 2 (force change current speed) (current speed - %f, server base speed - %f, client base speed - %f), %u alerts", _session->GetPlayerName(), _session->GetAccountId(), curClientSpeed, serverSpeed, baseRunClientSpeed, 3/*sWorld.getConfig(CONFIG_UINT32_WARDEN_SPEED_ALERT_COUNT)*/);
                            //sLog.outWarden("List auras: %s", GetAuraList(plr).c_str());
                            ClearAlerts();
                            _session->KickPlayer();
                            //sWorld.BanAccount(BAN_ACCOUNT_ID, GetPlayerName(), 0, "Detect speedhack(d)", "Warden AntiCheat", GetAccountId());
                            return;
                        }
                    }

                    // check base client speed and only speed >= RUN_SPEED
                    if (baseRunClientSpeed > serverSpeed && baseRunClientSpeed >= 7.0f && serverSpeed >= 7.0f)
                    {
                        if (baseRunClientSpeed >= 20.0f)
                        {
                            ++m_baseSpeedHackAlert;
                            sLog.outWarden("CLIENT WARDEN: Player %s (account: %u) warning(!!!) - base speed %f much more valid %f", _session->GetPlayerName(), _session->GetAccountId(), curClientSpeed, serverSpeed);
                            //_session->KickPlayer();
                            //sWorld.BanAccount(BAN_ACCOUNT_ID, _session->GetUsername(), 0, "Cheat detected (CheckId: 255)", "Warden AntiCheat", _session->GetAccountId());
                            //return;
                        }

                        //if (!(plr->m_movementInfo.GetMovementFlags() & (MOVEFLAG_FALLING | MOVEFLAG_FALLINGFAR)) && !plr->IsFalling())
                            //++m_baseSpeedHackAlert;

                        //if (m_baseSpeedHackAlert >= (int32)sWorld.getConfig(CONFIG_UINT32_WARDEN_SPEED_ALERT_COUNT_EXT))
                        if (m_baseSpeedHackAlert >= 3)
                        {
                            sLog.outWarden("CLIENT WARDEN: Player %s (account: %u) must be banned - speedhack type 1 (force change base speed) (current speed - %f, server base speed - %f, client base speed - %f), %u alerts", _session->GetPlayerName(), _session->GetAccountId(), curClientSpeed, serverSpeed, baseRunClientSpeed, 3/*sWorld.getConfig(CONFIG_UINT32_WARDEN_SPEED_ALERT_COUNT)*/);
                            //sLog.outWarden("List auras: %s", GetAuraList(plr).c_str());
                            ClearAlerts();
                            _session->KickPlayer();
                            //sWorld.BanAccount(BAN_ACCOUNT_ID, GetPlayerName(), 0, "Detect speedhack(d)", "Warden AntiCheat", GetAccountId());
                            return;
                        }
                    }
                }
            }

            //sLog.outError("Current speed - %f, base speed - %f, server speed (%u) - %f, moving - %s, falling - %s, launched - %s,  launched speed - %f, spline - %s", curClientSpeed, baseRunClientSpeed, int8(m_type), serverSpeed, plr->isMoving() ? "true" : "false", plr->IsFalling() ? "true" : "false", plr->IsLaunched() ? "true" : "false", plr->GetXYSpeed(), plr->GetAnticheatMgr()->HasSpline() ? "true" : "false");
        }

        // DoCustomChecks()
        if (wallClimb >= 10.0f)
        {
            sLog.outWarden("CLIENT WARDEN: Player %s (account: %u) banned - unexpected v_delta - %f, possible wallhack", wallClimb, _session->GetPlayerName(), _session->GetAccountId());
            _session->KickPlayer();
            //sWorld.BanAccount(BAN_ACCOUNT_ID, _session->GetUsername(), 0, "Cheat detected (CheckId: 255)", "Warden AntiCheat", _session->GetAccountId());
            return;
        }

        // while kicked, player autorelease spirit and cheat don't worked
        if (!plr->isAlive() && !plr->HasAuraType(SPELL_AURA_GHOST) && !plr->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION) && plHealth == 9999)
        {
            sLog.outWarden("CLIENT WARDEN: Player %s (account: %u) kicked - set cheat health, but on server player is dead", _session->GetPlayerName(), _session->GetAccountId());
            _session->KickPlayer();
            return;
        }

        // while kicked, fall time are strange
        /*if (fallTime && fallTime == 0x64)
        {
            if (plr->GetAnticheatMgr()->GetLastFallTime() != fallTime)
                sLog.outWarden("CLIENT WARDEN: Player %s (account: %u) kicked - unexpected fall time %u, last fall time %u", _session->GetPlayerName(), _session->GetAccountId(), fallTime, plr->GetAnticheatMgr()->GetLastFallTime());
            //_session->KickPlayer();
            return;
        }*/

        /* DISABLED
        uint8 mr6;
        recv_data >> mr6;

        if (mr6 != 0x00)
        {
            recv_data.rpos(recv_data.wpos());
            sLog.outError("CLIENT WARDEN : Get client data (dynamic - mr6) is failed, account ID - %u, account name - %s", client->GetAccountId(), client->GetUsername().c_str());
            // nulled
            ClearPlayerPointers();
            ClearAlerts();
            return;
        }

        uint32 trackState_c;
        recv_data >> trackState_c;

        uint32 trackState_r;
        recv_data >> trackState_r;

        if (trackState_c || trackState_r)
        {
            // check everything mask and ban
            if (trackState_c == TRACK_ALL || trackState_r == TRACK_ALL)
            {
                //recv_data.rpos(recv_data.wpos());
                //sWorld.BanAccount(BAN_ACCOUNT_ID, client->GetPlayerName(), 0, "Detect unexpected tracking state(d)", "Warden AntiCheat", client->GetAccountId());
                //return;
                sLog.outError("Detect unexpected tracking state(ALL), player - %s", client->GetPlayerName());
            }

            // not tracking aura creature - ban
            if (client->GetPlayer() && !client->GetPlayer()->HasAuraType(SPELL_AURA_TRACK_CREATURES) && trackState_c)
            {
                //recv_data.rpos(recv_data.wpos());
                sLog.outWarden("Detect unexpected tracking state(creatures), state - %X, player - %s", trackState_c, client->GetPlayerName());
                //sWorld.BanAccount(BAN_ACCOUNT_ID, client->GetPlayerName(), 0, "Detect unexpected tracking state(d)", "Warden AntiCheat", client->GetAccountId());
                //return;
            }

            // not tracking aura resources - ban
            if (client->GetPlayer() && !client->GetPlayer()->HasAuraType(SPELL_AURA_TRACK_RESOURCES) && trackState_r)
            {
                //recv_data.rpos(recv_data.wpos());
                sLog.outWarden("Detect unexpected tracking state(resources), state - %X, player - %s", trackState_r, client->GetPlayerName());
                //sWorld.BanAccount(BAN_ACCOUNT_ID, client->GetPlayerName(), 0, "Detect unexpected tracking state(d)", "Warden AntiCheat", client->GetAccountId());
                //return;
            }

            //sLog.outError("Track creatures - %X, Track resources - %X, player - %s", trackState_c, trackState_r, client->GetPlayerName());
        } DISABLED */

        // decrease alert if not activated
        if (m_currentSpeedHackAlert == speedAlertsBase)
        {
            --m_currentSpeedHackAlert;

            if (m_currentSpeedHackAlert < 0)
                m_currentSpeedHackAlert = 0;
        }
        else if (m_baseSpeedHackAlert == speedExtAlertsBase)
        {
            --m_baseSpeedHackAlert;

            if (m_baseSpeedHackAlert < 0)
                m_baseSpeedHackAlert = 0;
        }
        /*else if (m_moveFlagsAlert == moveFlagsAlertsBase)
        {
            --m_moveFlagsAlert;

            if (m_moveFlagsAlert < 0)
                m_moveFlagsAlert = 0;
        }*/
    }
}

/*void WorldSession::SystemCheckHandler(WorldPacket &recv_data)
{
    // special pack of check
    for (int i = 0; i < sWardenMgr.GetDataSize(CHECK_CLASS_SYSTEM); ++i)
    {
        uint8 imp_res;
        recv_data >> imp_res;

        if (imp_res != 0)
        {
            sLog.outWarden("CLIENT WARDEN : System request failed, account ID - %u, account name - %s", GetAccountId(), GetUsername().c_str());
            recv_data.warden_hexlike();
            return;
        }

        uint32 func_addr;
        recv_data >> func_addr;

        if (func_addr == 0)
        {
            sLog.outWarden("CLIENT WARDEN : Function get address is failed, account ID - %u, account name - %s", GetAccountId(), GetUsername().c_str());
            recv_data.warden_hexlike();
            return;
        }

        SetFuncAddress(SystemFunctions(i), func_addr);
    }
}*/

std::string WardenWin::MovementFlagsCheck(uint32 moveFlags, std::string &reason, bool /*extended*/)
{
    // get player
    Player * plr = _session->GetPlayer();

    if (!plr)
        return "NONE";

    // generate flag mask at moving
    MovementFlags dis_mask = MovementFlags(MOVEFLAG_LEVITATING | MOVEFLAG_FIXED_Z |
        MOVEFLAG_JUMPING | MOVEFLAG_FALLINGFAR | MOVEFLAG_FLYING | MOVEFLAG_ONTRANSPORT | MOVEFLAG_SPLINE_ELEVATION | MOVEFLAG_SPLINE_ENABLED | MOVEFLAG_HOVER);

    bool incorrect = false;

    // this combine of flags + some other = 100% flyhack
    if ((moveFlags & MOVEFLAG_WALK_MODE) && (moveFlags & MOVEFLAG_FIXED_Z) && (moveFlags & MOVEFLAG_SWIMMING) && (moveFlags & MOVEFLAG_HOVER)
        && (moveFlags & MOVEFLAG_FALLINGFAR) && (moveFlags & MOVEFLAG_FLYING) && (moveFlags & MOVEFLAG_ONTRANSPORT))
    {
        reason += "detect flyhack";
        return "BAN";
    }

    if (moveFlags & MOVEFLAG_LEVITATING)
    {
        incorrect = true;
        reason += "detect incorrect creature flying state";
    }

    if (moveFlags & MOVEFLAG_FIXED_Z)
    {
        incorrect = true;
        reason += " & detect incorrect freeze Z axis";
    }

    if (moveFlags & MOVEFLAG_HOVER)
    {
        if (!plr->HasAuraType(SPELL_AURA_HOVER))
        {
            incorrect = true;
            reason += " & detect incorrect levitating state";
        }
    }

    if ((moveFlags & MOVEFLAG_SWIMMING) && (moveFlags & dis_mask))
    {
        incorrect = true;

        // detect new flyhack
        if ((moveFlags & MOVEFLAG_SWIMMING) && (moveFlags & MOVEFLAG_LEVITATING))
        {
            reason += " & possible flyhack";
            //SendControlMovementPacket(MSG_MOVE_STOP_SWIM, MOVEFLAG_LEVITATING, true);
        }
        // some weird flags with swim state
        else
            reason += " & weird flags with swimming state";
    }

    if (moveFlags & MOVEFLAG_WATERWALKING)
    {
        if (!plr->HasAuraType(SPELL_AURA_GHOST) && !plr->HasAuraType(SPELL_AURA_WATER_WALK))
        {
            incorrect = true;
            reason += " & detect incorrect waterwalking state";
            //SendControlMovementPacket(MSG_MOVE_STOP, MOVEFLAG_WATERWALKING, true);
        }
    }

    if (incorrect)
        return "KICK";
    else
        return "NONE";
}

std::string WardenWin::GetMovementFlagInfo(uint32 moveFlags)
{
    std::string info = "";

    if (moveFlags & MOVEFLAG_FORWARD)
        info += "moveflag_forward";

    if (moveFlags & MOVEFLAG_BACKWARD)
        info += " | moveflag_backward";

    if (moveFlags & MOVEFLAG_STRAFE_LEFT)
        info += " | moveflag_strafe_left";

    if (moveFlags & MOVEFLAG_STRAFE_RIGHT)
        info += " | moveflag_strafe_right";

    if (moveFlags & MOVEFLAG_TURN_LEFT)
        info += " | moveflag_turn_left";

    if (moveFlags & MOVEFLAG_TURN_RIGHT)
        info += " | moveflag_turn_right";

    if (moveFlags & MOVEFLAG_PITCH_UP)
        info += " | moveflag_pitch_up";

    if (moveFlags & MOVEFLAG_PITCH_DOWN)
        info += " | moveflag_pitch_down";

    if (moveFlags & MOVEFLAG_WALK_MODE)
        info += " | moveflag_walk_mode";

    if (moveFlags & MOVEFLAG_LEVITATING)
        info += " | moveflag_levitating";

    if (moveFlags & MOVEFLAG_FIXED_Z)
        info += " | moveflag_fixed_Z";

    if (moveFlags & MOVEFLAG_ROOT)
        info += " | moveflag_root";

    if (moveFlags & MOVEFLAG_JUMPING)
        info += " | moveflag_jumping";

    if (moveFlags & MOVEFLAG_FALLINGFAR)
        info += " | moveflag_falling_far";

    if (moveFlags & MOVEFLAG_SWIMMING)
        info += " | moveflag_swim";

    // ???
    if (moveFlags & MOVEFLAG_FLYING)
        info += " | moveflag_flying(?)";

    if (moveFlags & MOVEFLAG_ONTRANSPORT)
        info += " | moveflag_on_transport";

    if (moveFlags & MOVEFLAG_SPLINE_ELEVATION)
        info += " | moveflag_spilne_elevation";

    if (moveFlags & MOVEFLAG_SPLINE_ENABLED)
        info += " | moveflag_spilne_enabled";

    if (moveFlags & MOVEFLAG_WATERWALKING)
        info += " | moveflag_waterwalking";

    if (moveFlags & MOVEFLAG_SAFE_FALL)
        info += " | moveflag_safe_fall";

    if (moveFlags & MOVEFLAG_HOVER)
        info += " | moveflag_hover";

    if (moveFlags & MOVEFLAG_INTERNAL)
        info += " | moveflag_internal";

    // cut mistake begin
    std::string new_info = "";
    int i = -1;
    for(std::string::iterator itr = info.begin(); itr != info.end(); ++itr)
    {
        ++i;

        if (i < 3 && (*itr == ' ' || *itr == '|'))
            continue;

        new_info += *itr;
    }

    if (new_info == "")
        new_info = "none";

    return new_info;
}

MoveType WardenWin::SelectSpeedType(uint32 moveFlags)
{
    // get player
    Player * plr = _session->GetPlayer();

    if (!plr)
        return MOVETYPE_NONE;

    if (moveFlags & MOVEFLAG_SWIMMING)
    {
        if (moveFlags & MOVEFLAG_BACKWARD)
            return MOVETYPE_SWIM_BACK;
        else
            return MOVETYPE_SWIM;
    }
    else if (moveFlags & MOVEFLAG_WALK_MODE)
        return MOVETYPE_WALK;
    else if (moveFlags & MOVEFLAG_BACKWARD)
        return MOVETYPE_RUN_BACK;

    return MOVETYPE_RUN;
}

void WardenWin::SendControlMovementPacket(uint32 opcode, uint32 moveFlags, bool remove)
{
    Player * plr = _session->GetPlayer();

    if (!plr)
        return;

    if (moveFlags)
    {
        MovementInfo mInfo = plr->m_movementInfo;

        if (remove)
            mInfo.RemoveMovementFlag(MovementFlags(moveFlags));
        else
            mInfo.AddMovementFlag(MovementFlags(moveFlags));

        WorldPacket data(OpcodesList(opcode), 200);
        data << plr->GetPackGUID();
        mInfo.Write(data);
        plr->SendMessageToSet(&data, true);
    }
}

template<class T>
bool WardenWin::ReadMemChunk(ByteBuffer &buff, T &data)
{
    uint8 mem_result;
    buff >> mem_result;

    if (mem_result != 0x00)
    {
        buff.rpos(buff.wpos());
        ClearOffsets();
        return false;
    }

    buff >> data;

    return true;
}

template <typename I>
I WardenWin::GetRandomCheckFromList(I begin, I end)
{
    const unsigned long n = std::distance(begin, end);
    const unsigned long divisor = (RAND_MAX + 1) / n;

    unsigned long k;
    do { k = std::rand() / divisor; } while (k >= n);

    std::advance(begin, k);
    return begin;
}

std::string WardenWin::GetAuraList(Player* plr)
{
    if (!plr)
        return "";

    Unit::SpellAuraHolderMap const& uAuras = plr->GetSpellAuraHolderMap();
    std::ostringstream ss_name;

    for (Unit::SpellAuraHolderMap::const_iterator itr = uAuras.begin(); itr != uAuras.end(); ++itr)
    {
        bool talent = GetTalentSpellCost(itr->second->GetId()) > 0;

        SpellAuraHolder *holder = itr->second;
        char const* name = holder->GetSpellProto()->SpellName[0];

        for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        {
            Aura *aur = holder->GetAuraByEffectIndex(SpellEffectIndex(i));
            if (!aur)
                continue;

            ss_name << itr->second->GetId() << "[" << name << "][" << i << "] ";
        }
    }

    return ss_name.str();
}
