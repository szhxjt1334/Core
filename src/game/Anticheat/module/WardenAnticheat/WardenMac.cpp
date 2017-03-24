
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
#include "WardenMac.h"
#include "WardenStorageMgr.h"
#include "WardenModuleMac.h"
#include "Util.h"

#include "MoveSpline.h"

WardenMac::WardenMac() : Warden()
{
    ClearAlerts();
    ClearOffsets();
}

WardenMac::~WardenMac()
{
}

void WardenMac::Init(WorldSession *pClient, BigNumber *k)
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

WardenModule* WardenMac::GetModuleForClient()
{
    WardenModule *mod = new WardenModule;

    uint32 length = sizeof(MacModule.Module);

    // data assign
    mod->CompressedSize = length;
    mod->CompressedData = new uint8[length];
    memcpy(mod->CompressedData, MacModule.Module, length);
    memcpy(mod->Key, MacModule.ModuleKey, 16);

    // md5 hash
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, mod->CompressedData, length);
    MD5_Final((uint8*)&mod->ID, &ctx);

    return mod;
}

void WardenMac::RequestHash() {}
