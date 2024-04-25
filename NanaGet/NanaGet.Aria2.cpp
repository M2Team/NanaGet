/*
 * PROJECT:   NanaGet
 * FILE:      NanaGet.Aria2.cpp
 * PURPOSE:   Implementation for the NanaGet.Aria2
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "NanaGet.Aria2.h"

#include <Mile.Helpers.CppBase.h>

namespace NanaGet::Json
{
    std::string GetStringValue(
        nlohmann::json const& Current)
    {
        return Current.is_string()
            ? Current.get<std::string>()
            : std::string();
    }

    std::string GetStringValue(
        nlohmann::json const& Root,
        std::string const& Key)
    {
        return GetStringValue(Mile::Json::GetSubKey(Root, Key));
    }
}

namespace NanaGet::Aria2
{
    NLOHMANN_JSON_SERIALIZE_ENUM(NanaGet::Aria2::DownloadStatus, {
        { NanaGet::Aria2::DownloadStatus::Active, "active" },
        { NanaGet::Aria2::DownloadStatus::Waiting, "waiting" },
        { NanaGet::Aria2::DownloadStatus::Paused, "paused" },
        { NanaGet::Aria2::DownloadStatus::Complete, "complete" },
        { NanaGet::Aria2::DownloadStatus::Error, "error" },
        { NanaGet::Aria2::DownloadStatus::Removed, "removed" }
    })

    NLOHMANN_JSON_SERIALIZE_ENUM(NanaGet::Aria2::UriStatus, {
       { NanaGet::Aria2::UriStatus::Used, "used" },
       { NanaGet::Aria2::UriStatus::Waiting, "waiting" }
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(NanaGet::Aria2::BitTorrentFileMode, {
       { NanaGet::Aria2::BitTorrentFileMode::Single, "single" },
       { NanaGet::Aria2::BitTorrentFileMode::Multi, "multi" }
    });
}

std::string NanaGet::Aria2::FromDownloadGid(
    NanaGet::Aria2::DownloadGid const& Value)
{
    return Mile::FormatString("%016llX", Value);
}

NanaGet::Aria2::DownloadGid NanaGet::Aria2::ToDownloadGid(
    nlohmann::json const& Value)
{
    return Mile::ToUInt64(NanaGet::Json::GetStringValue(Value), 16);
}

NanaGet::Aria2::DownloadStatus NanaGet::Aria2::ToDownloadStatus(
    nlohmann::json const& Value)
{
    try
    {
        return Value.get<NanaGet::Aria2::DownloadStatus>();
    }
    catch (...)
    {
        return NanaGet::Aria2::DownloadStatus::Error;
    }
}

NanaGet::Aria2::UriStatus NanaGet::Aria2::ToUriStatus(
    nlohmann::json const& Value)
{
    try
    {
        return Value.get<NanaGet::Aria2::UriStatus>();
    }
    catch (...)
    {
        return NanaGet::Aria2::UriStatus::Used;
    }
}

NanaGet::Aria2::UriInformation NanaGet::Aria2::ToUriInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::UriInformation Result;

    Result.Uri = NanaGet::Json::GetStringValue(Value, "uri");

    Result.Status = NanaGet::Aria2::ToUriStatus(
        Mile::Json::ToPrimitive(Mile::Json::GetSubKey(Value, "status")));

    return Result;
}

NanaGet::Aria2::FileInformation NanaGet::Aria2::ToFileInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::FileInformation Result;

    Result.Index = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "index"));

    Result.Path = NanaGet::Json::GetStringValue(Value, "path");

    Result.Length = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "length"));

    Result.CompletedLength = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "completedLength"));

    std::string Selected = NanaGet::Json::GetStringValue(Value, "selected");
    if (0 == std::strcmp(Selected.c_str(), "true"))
    {
        Result.Selected = true;
    }
    else if (0 == std::strcmp(Selected.c_str(), "false"))
    {
        Result.Selected = false;
    }

    for (nlohmann::json const& Uri : Mile::Json::ToArray(
        Mile::Json::GetSubKey(Value, "uris")))
    {
        Result.Uris.emplace_back(NanaGet::Aria2::ToUriInformation(
            Mile::Json::ToObject(Uri)));
    }

    return Result;
}

NanaGet::Aria2::BitTorrentFileMode NanaGet::Aria2::ToBitTorrentFileMode(
    nlohmann::json const& Value)
{
    try
    {
        return Value.get<NanaGet::Aria2::BitTorrentFileMode>();
    }
    catch (...)
    {
        return NanaGet::Aria2::BitTorrentFileMode::None;
    }
}

NanaGet::Aria2::BitTorrentInfoDictionary NanaGet::Aria2::ToBitTorrentInfoDictionary(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::BitTorrentInfoDictionary Result;

    Result.Name = NanaGet::Json::GetStringValue(Value, "name");

    return Result;
}

NanaGet::Aria2::BitTorrentInformation NanaGet::Aria2::ToBitTorrentInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::BitTorrentInformation Result;

    for (nlohmann::json const& Array : Mile::Json::ToArray(
        Mile::Json::GetSubKey(Value, "announceList")))
    {
        std::vector<std::string> Content;
        for (nlohmann::json const& Item : Mile::Json::ToArray(Array))
        {
            Content.emplace_back(NanaGet::Json::GetStringValue(Item));
        }
        Result.AnnounceList.emplace_back(Content);
    }

    Result.Comment = NanaGet::Json::GetStringValue(Value, "comment");

    Result.CreationDate = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "creationDate"));

    Result.Mode = NanaGet::Aria2::ToBitTorrentFileMode(
        Mile::Json::ToPrimitive(Mile::Json::GetSubKey(Value, "mode")));

    Result.Info = NanaGet::Aria2::ToBitTorrentInfoDictionary(
        Mile::Json::ToObject(Mile::Json::GetSubKey(Value, "info")));

    return Result;
}

NanaGet::Aria2::DownloadInformation NanaGet::Aria2::ToDownloadInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::DownloadInformation Result;

    Result.Gid = NanaGet::Aria2::ToDownloadGid(
        Mile::Json::ToPrimitive(Mile::Json::GetSubKey(Value, "gid")));

    Result.Status = NanaGet::Aria2::ToDownloadStatus(
        Mile::Json::ToPrimitive(Mile::Json::GetSubKey(Value, "status")));

    Result.TotalLength = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "totalLength"));

    Result.CompletedLength = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "completedLength"));

    Result.UploadLength = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "uploadLength"));

    Result.Bitfield = NanaGet::Json::GetStringValue(Value, "bitfield");

    Result.DownloadSpeed = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "downloadSpeed"));

    Result.UploadSpeed = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "uploadSpeed"));

    Result.InfoHash = NanaGet::Json::GetStringValue(Value, "infoHash");

    Result.NumSeeders = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "numSeeders"));

    std::string Seeder = NanaGet::Json::GetStringValue(Value, "seeder");
    if (0 == std::strcmp(Seeder.c_str(), "true"))
    {
        Result.Seeder = true;
    }
    else if (0 == std::strcmp(Seeder.c_str(), "false"))
    {
        Result.Seeder = false;
    }

    Result.PieceLength = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "pieceLength"));

    Result.NumPieces = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "numPieces"));

    Result.Connections = Mile::ToInt32(
        NanaGet::Json::GetStringValue(Value, "connections"));

    Result.ErrorCode = Mile::ToInt32(
        NanaGet::Json::GetStringValue(Value, "errorCode"));

    Result.ErrorMessage =
        NanaGet::Json::GetStringValue(Value, "errorMessage");

    for (nlohmann::json const& Item : Mile::Json::ToArray(
        Mile::Json::GetSubKey(Value, "followedBy")))
    {
        Result.FollowedBy.emplace_back(NanaGet::Aria2::ToDownloadGid(Item));
    }

    Result.Following = NanaGet::Aria2::ToDownloadGid(
        Mile::Json::ToPrimitive(Mile::Json::GetSubKey(Value, "following")));

    Result.BelongsTo = NanaGet::Aria2::ToDownloadGid(
        Mile::Json::ToPrimitive(Mile::Json::GetSubKey(Value, "belongsTo")));

    Result.Dir = NanaGet::Json::GetStringValue(Value, "dir");

    for (nlohmann::json const& File : Mile::Json::ToArray(
        Mile::Json::GetSubKey(Value, "files")))
    {
        Result.Files.emplace_back(NanaGet::Aria2::ToFileInformation(
            Mile::Json::ToObject(File)));
    }

    Result.BitTorrent = NanaGet::Aria2::ToBitTorrentInformation(
        Mile::Json::ToObject(Mile::Json::GetSubKey(Value, "bittorrent")));

    Result.VerifiedLength = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "verifiedLength"));

    std::string VerifyIntegrityPending =
        NanaGet::Json::GetStringValue(Value, "verifyIntegrityPending");
    if (0 == std::strcmp(VerifyIntegrityPending.c_str(), "true"))
    {
        Result.VerifyIntegrityPending = true;
    }
    else if (0 == std::strcmp(VerifyIntegrityPending.c_str(), "false"))
    {
        Result.VerifyIntegrityPending = false;
    }

    return Result;
}

std::string NanaGet::Aria2::ToFriendlyName(
    NanaGet::Aria2::DownloadInformation const& Value)
{
    if (!Value.BitTorrent.Info.Name.empty())
    {
        return Value.BitTorrent.Info.Name;
    }

    if (!Value.Files.empty())
    {
        const char* Candidate = nullptr;

        if (!Value.Files[0].Path.empty())
        {
            Candidate = Value.Files[0].Path.c_str();
        }
        else if (!Value.Files[0].Uris.empty())
        {
            Candidate = Value.Files[0].Uris[0].Uri.c_str();
        }

        if (Candidate)
        {
            const char* RawName = std::strrchr(Candidate, L'/');
            return std::string(
                (RawName && RawName != Candidate)
                ? &RawName[1]
                : Candidate);
        }
    }

    return NanaGet::Aria2::FromDownloadGid(Value.Gid);
}

NanaGet::Aria2::PeerInformation NanaGet::Aria2::ToPeerInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::PeerInformation Result;

    Result.PeerId = NanaGet::Json::GetStringValue(Value, "peerId");

    Result.Ip = NanaGet::Json::GetStringValue(Value, "ip");

    Result.Port = static_cast<std::uint16_t>(Mile::ToUInt32(
        NanaGet::Json::GetStringValue(Value, "port")));

    Result.Bitfield = NanaGet::Json::GetStringValue(Value, "bitfield");

    std::string AmChoking =
        NanaGet::Json::GetStringValue(Value, "amChoking");
    if (0 == std::strcmp(AmChoking.c_str(), "true"))
    {
        Result.AmChoking = true;
    }
    else if (0 == std::strcmp(AmChoking.c_str(), "false"))
    {
        Result.AmChoking = false;
    }

    std::string PeerChoking =
        NanaGet::Json::GetStringValue(Value, "peerChoking");
    if (0 == std::strcmp(PeerChoking.c_str(), "true"))
    {
        Result.PeerChoking = true;
    }
    else if (0 == std::strcmp(PeerChoking.c_str(), "false"))
    {
        Result.PeerChoking = false;
    }

    Result.DownloadSpeed = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "downloadSpeed"));

    Result.UploadSpeed = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "uploadSpeed"));

    std::string Seeder = NanaGet::Json::GetStringValue(Value, "seeder");
    if (0 == std::strcmp(Seeder.c_str(), "true"))
    {
        Result.Seeder = true;
    }
    else if (0 == std::strcmp(Seeder.c_str(), "false"))
    {
        Result.Seeder = false;
    }

    return Result;
}

NanaGet::Aria2::ServerInformation NanaGet::Aria2::ToServerInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::ServerInformation Result;

    Result.Uri = NanaGet::Json::GetStringValue(Value, "uri");

    Result.CurrentUri = NanaGet::Json::GetStringValue(Value, "currentUri");

    Result.DownloadSpeed = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "downloadSpeed"));

    return Result;
}

NanaGet::Aria2::ServersInformation NanaGet::Aria2::ToServersInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::ServersInformation Result;

    Result.Index = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "index"));

    for (nlohmann::json const& Server : Mile::Json::ToArray(
        Mile::Json::GetSubKey(Value, "servers")))
    {
        Result.Servers.emplace_back(NanaGet::Aria2::ToServerInformation(
            Mile::Json::ToObject(Server)));
    }

    return Result;
}

NanaGet::Aria2::GlobalStatusInformation NanaGet::Aria2::ToGlobalStatusInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::GlobalStatusInformation Result;

    Result.DownloadSpeed = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "downloadSpeed"));

    Result.UploadSpeed = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "uploadSpeed"));

    Result.NumActive = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "numActive"));

    Result.NumWaiting = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "numWaiting"));

    Result.NumStopped = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "numStopped"));

    Result.NumStoppedTotal = Mile::ToUInt64(
        NanaGet::Json::GetStringValue(Value, "numStoppedTotal"));

    return Result;
}

NanaGet::Aria2::VersionInformation NanaGet::Aria2::ToVersionInformation(
    nlohmann::json const& Value)
{
    VersionInformation Result;

    Result.Version = NanaGet::Json::GetStringValue(Value, "version");

    for (nlohmann::json const& EnabledFeature : Mile::Json::ToArray(
        Mile::Json::GetSubKey(Value, "enabledFeatures")))
    {
        Result.EnabledFeatures.emplace_back(
            EnabledFeature.get<std::string>());
    }

    return Result;
}

NanaGet::Aria2::SessionInformation NanaGet::Aria2::ToSessionInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::SessionInformation Result;

    Result.SessionId = NanaGet::Json::GetStringValue(Value, "sessionId");

    return Result;
}
