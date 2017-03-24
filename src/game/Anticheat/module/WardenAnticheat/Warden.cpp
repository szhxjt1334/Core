
#include <zlib/zlib.h>
#include "WardenKeyGeneration.h"
#include "Common.h"
#include "Player.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "World.h"
#include "Log.h"
#include "Opcodes.h"
#include "ByteBuffer.h"
#include <openssl/md5.h>
#include <openssl/sha.h>
#include "Database/DatabaseEnv.h"
#include "Policies/SingletonImp.h"
#include "Auth/BigNumber.h"
#include "Warden.h"
#include "WardenStorageMgr.h"
#include "Util.h"
#include "module/libanticheat.h"

#ifndef _WIN32
#include <dirent.h>
#include <errno.h>
#endif

Warden::Warden(): _session(NULL), _module(NULL), clientBuild(0), _inputCrypto(16), _outputCrypto(16), _initialized(false), _checkTimer(20*IN_MILLISECONDS), _checkTimer2(8*IN_MILLISECONDS),
    s_currentModulePtr(0), s_currentModule(0)
{
    _lastTimestamp = WorldTimer::getMSTime();
    // sync
    m_synIndex = 0;
    m_ackIndex = 0;
    m_reqAckIndex = 0;
    m_sendLastPacketCount = 0;
    m_sendLastPacketCount2 = 0;

    m_failedSyncPacketCount = 0;

    m_memChecksCount = sAcConfig.getConfig(CONFIG_UINT32_AC_WARDEN_MEM_CHECKS_COUNT);
    m_otherChecksCount = sAcConfig.getConfig(CONFIG_UINT32_AC_WARDEN_OTHER_CHECKS_COUNT);
}

Warden::~Warden()
{
    if (_module)
        delete[] _module->CompressedData;

    delete _module;
    _module = NULL;
}

void Warden::RequestModule()
{
    sLog.outDebug("SERVER WARDEN: Request Module send...");

    // Create packet structure
    WardenModuleUse request;
    request.Command = 0x00;

    memcpy(request.ModuleId, _module->ID, 16);
    memcpy(request.ModuleKey, _module->Key, 16);
    request.Size = _module->CompressedSize;

    // Encrypt with warden RC4 key.
    EncryptData((uint8*)&request, sizeof(WardenModuleUse));

    WorldPacket pkt(SMSG_WARDEN_DATA, sizeof(WardenModuleUse));
    pkt.append((uint8*)&request, sizeof(WardenModuleUse));
    _session->SendPacket(&pkt);
}

void Warden::SendModuleToClient()
{
    sLog.outDebug("SERVER WARDEN: Sending module to client\n");

    // Create packet structure
    WardenModuleCache packet;

    uint32 sizeLeft = _module->CompressedSize;
    uint32 pos = 0;
    uint16 chunkSize;
    while (sizeLeft > 0)
    {
        chunkSize = sizeLeft < 500 ? sizeLeft : 500;
        packet.Command = 0x01;
        packet.DataSize = chunkSize;
        memcpy(packet.Data, &_module->CompressedData[pos], chunkSize);
        sizeLeft -= chunkSize;
        pos += chunkSize;

        uint16 packetSize = sizeof(uint8) + sizeof(uint16) + chunkSize;
        EncryptData((uint8*)&packet, packetSize);
        WorldPacket pkt(SMSG_WARDEN_DATA, packetSize);
        pkt.append((uint8*)&packet, packetSize);
        _session->SendPacket(&pkt);
    }

    sLog.outDebug("SERVER WARDEN: Module transfer complete\n");
}

void Warden::Update()
{
    if (_initialized)
    {
        uint32 currentTimestamp = WorldTimer::getMSTime();
        uint32 diff = currentTimestamp - _lastTimestamp;
        _lastTimestamp = currentTimestamp;

        // disabled warden requests for GMs, but warden module must be loaded
        //if (_session->->GetSecurity() > SEC_MODERATOR)
            //return;

        // new sync packets system
        // about 30/90 seconds on response
        if (m_sendLastPacketCount > 5 || m_sendLastPacketCount2 > 25)
        {
            std::string type = (m_sendLastPacketCount > 5) ? "static" : "dynamic";
            sLog.outWarden("Player %s (account: %u) (latency: %u, IP: %s) exceeded Warden module response delay (%s) - disconnecting client",
                _session->GetPlayerName(), _session->GetAccountId(), _session->GetLatency(), _session->GetRemoteAddress().c_str(), type.c_str());
            _session->KickPlayer();
            return;
        }

        // failed sync attempts larger than normal
        if (m_failedSyncPacketCount > 3)
        {
            sLog.outWarden("Player %s (account: %u) (latency: %u, IP: %s) too many failed sync attempts - disconnecting client",
                _session->GetPlayerName(), _session->GetAccountId(), _session->GetLatency(), _session->GetRemoteAddress().c_str());
            m_failedSyncPacketCount = 0;
            _session->KickPlayer();
            return;
        }

        // first thread - static checks
        if (diff >= _checkTimer)
        {
            if (_session->GetPlayer() && _session->GetPlayer()->IsInWorld())
            {
                // send request
                RequestData();           
                // set timer
                _checkTimer = GetRandCheckTimer();
                _checkTimer2 += 1*IN_MILLISECONDS;
            }
            else
                // forced cycle update if player isn't in world
                _checkTimer = 0;
        }
        else
            _checkTimer -= diff;

        // second separate thread - dynamic checks
        if (diff >= _checkTimer2)
        {
            if (_session->GetPlayer() && _session->GetPlayer()->IsInWorld() && !_session->GetPlayer()->IsBeingTeleported())
            {
                // send request
                RequestData2();
                // set timer
                _checkTimer2 = 4*IN_MILLISECONDS;
            }
            else
                // forced request after teleport or other
                _checkTimer2 = 0;
        }
        else
            _checkTimer2 -= diff;
    }
}

void Warden::SendPacket(ByteBuffer& data)
{
    // Encrypt with warden RC4 key
    EncryptData(const_cast<uint8*>(data.contents()), data.size());

    WorldPacket pkt(SMSG_WARDEN_DATA, data.size());
    pkt.append(data);
    _session->SendPacket(&pkt);
}

void Warden::DecryptData(uint8* buffer, uint32 length)
{
    _inputCrypto.UpdateData(length, buffer);
}

void Warden::EncryptData(uint8* buffer, uint32 length)
{
    _outputCrypto.UpdateData(length, buffer);
}

uint32 Warden::BuildChecksum(const uint8* data, uint32 dataLen)
{
    uint8 hash[20];
    SHA1(data, dataLen, hash);
    uint32 checkSum = 0;
    for (uint8 i = 0; i < 5; ++i)
        checkSum = checkSum ^ *(uint32*)(&hash[0] + i * 4);

    return checkSum;
}

bool Warden::IsValidCheckSum(uint32 checksum, const uint8* data, const uint16 length)
{
    uint32 newChecksum = BuildChecksum(data, length);

    if (checksum != newChecksum)
        return false;
    else
        return true;
}

void Warden::ClearAlerts()
{
    m_currentSpeedHackAlert = 0;
    m_baseSpeedHackAlert = 0;
    //m_moveFlagsAlert = 0;
}

void Warden::ClearOffsets()
{
    playerBase = 0;
    offset = 0;
    playerMovementBase = 0;
}

void Warden::Reset()
{
    _checkTimer = 10000;
    _checkTimer2 = 5000;

    ClearAlerts();
    ClearOffsets();
}

std::string Warden::ConvertPacketDataToString(const uint8 * packet_data, uint16 length)
{
    std::ostringstream ss;

    // convert packet data to string
    for (uint32 i = 0; i < length; i++)
    {
        if (int(packet_data[i]) < 16)
            ss << std::uppercase << std::hex << "0" << int(packet_data[i]) << "";
        else
            ss << std::uppercase << std::hex << int(packet_data[i]) << "";
    }

    std::string data_str = ss.str();
    return data_str;
}

uint32 Warden::GetRandCheckTimer()
{
    //uint32 min = sWorld.getConfig(CONFIG_UINT32_WARDEN_CHECK_INTERVAL_MIN);
    //uint32 max = sWorld.getConfig(CONFIG_UINT32_WARDEN_CHECK_INTERVAL_MAX);
    uint32 min = 25000;
    uint32 max = 35000;
    
    if (min > max)
        min = max;

    uint32 timer = urand(min, max);
    
    return timer;
}

void Warden::HandleWardenDataOpcode(WorldPacket& recvData)
{
	// initialize decrypt packet
	DecryptData(const_cast<uint8*>(recvData.contents()), recvData.size());

    uint8 opcode;
    recvData >> opcode;
    sLog.outDebug("CLIENT WARDEN: Got packet, %02X, size %u.", opcode, recvData.size());

    switch(opcode)
    {
    case 0:
        sLog.outDebug("CLIENT WARDEN: Account - %u get opcode 00 - Load module failed or module is missing...", _session->GetAccountId());
        SendModuleToClient();
        break;
    case 1:
        sLog.outDebug("CLIENT WARDEN: Account - %u get opcode 01 - Module has loaded. Recv answer....", _session->GetAccountId());
        RequestHash();
        break;
    case 2:
        sLog.outDebug("CLIENT WARDEN: Account - %u get opcode 02 - Recv Cheat Checks answer....", _session->GetAccountId());
        HandleData(recvData);
        break;
    case 3:
        sLog.outDebug("CLIENT WARDEN: Account - %u get opcode 03", _session->GetAccountId());
        break;
    case 4:
		sLog.outDebug("CLIENT WARDEN: Account - %u get opcode 04", _session->GetAccountId());
        HandleHashResult(recvData);
        //if (!_warden->GetCurrentModuleOffset())
        InitializeModule();
        //else
            //_warden->TestInit();
        break;
    case 5:
        sLog.outWarden("CLIENT WARDEN: Account - %u get opcode 05 - Module load failed!", _session->GetAccountId());
        _session->KickPlayer();
        break;
    default:
        sLog.outDebug("WARDEN: Got unknown opcode %02X of size %u.", opcode, recvData.size() - 1);
        break;
    }
}
