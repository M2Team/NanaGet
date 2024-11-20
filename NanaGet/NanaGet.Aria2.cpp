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

bool NanaGet::Aria2::ToBoolean(
    nlohmann::json const& Value)
{
    std::string RawValue = Mile::Json::ToString(Value);
    return (0 == std::strcmp(RawValue.c_str(), "true"));
}

std::string NanaGet::Aria2::FromDownloadGid(
    NanaGet::Aria2::DownloadGid const& Value)
{
    return Mile::FormatString("%016llX", Value);
}

NanaGet::Aria2::DownloadGid NanaGet::Aria2::ToDownloadGid(
    nlohmann::json const& Value)
{
    return Mile::ToUInt64(Mile::Json::ToString(Value), 16);
}

NanaGet::Aria2::DownloadStatus NanaGet::Aria2::ToDownloadStatus(
    nlohmann::json const& Value)
{
    std::string RawValue = Mile::Json::ToString(Value);

    if (0 == std::strcmp(RawValue.c_str(), "active"))
    {
        return NanaGet::Aria2::DownloadStatus::Active;
    }
    else if (0 == std::strcmp(RawValue.c_str(), "waiting"))
    {
        return NanaGet::Aria2::DownloadStatus::Waiting;
    }
    else if (0 == std::strcmp(RawValue.c_str(), "paused"))
    {
        return NanaGet::Aria2::DownloadStatus::Paused;
    }
    else if (0 == std::strcmp(RawValue.c_str(), "complete"))
    {
        return NanaGet::Aria2::DownloadStatus::Complete;
    }
    else if (0 == std::strcmp(RawValue.c_str(), "removed"))
    {
        return NanaGet::Aria2::DownloadStatus::Removed;
    }

    return NanaGet::Aria2::DownloadStatus::Error;
}

NanaGet::Aria2::UriStatus NanaGet::Aria2::ToUriStatus(
    nlohmann::json const& Value)
{
    std::string RawValue = Mile::Json::ToString(Value);

    if (0 == std::strcmp(RawValue.c_str(), "waiting"))
    {
        return NanaGet::Aria2::UriStatus::Waiting;
    }

    return NanaGet::Aria2::UriStatus::Used;
}

NanaGet::Aria2::UriInformation NanaGet::Aria2::ToUriInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::UriInformation Result;

    Result.Uri = Mile::Json::ToString(Mile::Json::GetSubKey(Value, "uri"));

    Result.Status = NanaGet::Aria2::ToUriStatus(
        Mile::Json::ToPrimitive(Mile::Json::GetSubKey(Value, "status")));

    return Result;
}

NanaGet::Aria2::FileInformation NanaGet::Aria2::ToFileInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::FileInformation Result;

    Result.Index = Mile::ToUInt64(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "index")));

    Result.Path = Mile::Json::ToString(Mile::Json::GetSubKey(Value, "path"));

    Result.Length = Mile::ToUInt64(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "length")));

    Result.CompletedLength = Mile::ToUInt64(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "completedLength")));

    Result.Selected = NanaGet::Aria2::ToBoolean(
        Mile::Json::GetSubKey(Value, "selected"));

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
    std::string RawValue = Mile::Json::ToString(Value);

    if (0 == std::strcmp(RawValue.c_str(), "single"))
    {
        return NanaGet::Aria2::BitTorrentFileMode::Single;
    }
    else if (0 == std::strcmp(RawValue.c_str(), "multi"))
    {
        return NanaGet::Aria2::BitTorrentFileMode::Multi;
    }

    return NanaGet::Aria2::BitTorrentFileMode::None;
}

NanaGet::Aria2::BitTorrentInfoDictionary NanaGet::Aria2::ToBitTorrentInfoDictionary(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::BitTorrentInfoDictionary Result;

    Result.Name = Mile::Json::ToString(Mile::Json::GetSubKey(Value, "name"));

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
            Content.emplace_back(Mile::Json::ToString(Item));
        }
        Result.AnnounceList.emplace_back(Content);
    }

    Result.Comment = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "comment"));

    Result.CreationDate = Mile::ToUInt64(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "creationDate")));

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
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "totalLength")));

    Result.CompletedLength = Mile::ToUInt64(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "completedLength")));

    Result.UploadLength = Mile::ToUInt64(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "uploadLength")));

    Result.Bitfield = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "bitfield"));

    Result.DownloadSpeed = Mile::ToUInt64(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "downloadSpeed")));

    Result.UploadSpeed = Mile::ToUInt64(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "uploadSpeed")));

    Result.InfoHash = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "infoHash"));

    Result.NumSeeders = Mile::ToUInt64(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "numSeeders")));

    Result.Seeder = NanaGet::Aria2::ToBoolean(
        Mile::Json::GetSubKey(Value, "seeder"));

    Result.PieceLength = Mile::ToUInt64(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "pieceLength")));

    Result.NumPieces = Mile::ToUInt64(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "numPieces")));

    Result.Connections = Mile::ToInt32(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "connections")));

    Result.ErrorCode = Mile::ToInt32(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "errorCode")));

    Result.ErrorMessage =
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "errorMessage"));

    for (nlohmann::json const& Item : Mile::Json::ToArray(
        Mile::Json::GetSubKey(Value, "followedBy")))
    {
        Result.FollowedBy.emplace_back(NanaGet::Aria2::ToDownloadGid(Item));
    }

    Result.Following = NanaGet::Aria2::ToDownloadGid(
        Mile::Json::ToPrimitive(Mile::Json::GetSubKey(Value, "following")));

    Result.BelongsTo = NanaGet::Aria2::ToDownloadGid(
        Mile::Json::ToPrimitive(Mile::Json::GetSubKey(Value, "belongsTo")));

    Result.Dir = Mile::Json::ToString(Mile::Json::GetSubKey(Value, "dir"));

    for (nlohmann::json const& File : Mile::Json::ToArray(
        Mile::Json::GetSubKey(Value, "files")))
    {
        Result.Files.emplace_back(NanaGet::Aria2::ToFileInformation(
            Mile::Json::ToObject(File)));
    }

    Result.BitTorrent = NanaGet::Aria2::ToBitTorrentInformation(
        Mile::Json::ToObject(Mile::Json::GetSubKey(Value, "bittorrent")));

    Result.VerifiedLength = Mile::ToUInt64(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "verifiedLength")));

    Result.VerifyIntegrityPending = NanaGet::Aria2::ToBoolean(
        Mile::Json::GetSubKey(Value, "verifyIntegrityPending"));

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

    Result.PeerId = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "peerId"));

    Result.Ip = Mile::Json::ToString(Mile::Json::GetSubKey(Value, "ip"));

    Result.Port = static_cast<std::uint16_t>(Mile::ToUInt32(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "port"))));

    Result.Bitfield = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "bitfield"));

    Result.AmChoking = NanaGet::Aria2::ToBoolean(
        Mile::Json::GetSubKey(Value, "amChoking"));

    Result.PeerChoking = NanaGet::Aria2::ToBoolean(
        Mile::Json::GetSubKey(Value, "peerChoking"));

    Result.DownloadSpeed = Mile::ToUInt64(Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "downloadSpeed")));

    Result.UploadSpeed = Mile::ToUInt64(Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "uploadSpeed")));

    Result.Seeder = NanaGet::Aria2::ToBoolean(
        Mile::Json::GetSubKey(Value, "seeder"));

    return Result;
}

NanaGet::Aria2::ServerInformation NanaGet::Aria2::ToServerInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::ServerInformation Result;

    Result.Uri = Mile::Json::ToString(Mile::Json::GetSubKey(Value, "uri"));

    Result.CurrentUri = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "currentUri"));

    Result.DownloadSpeed = Mile::ToUInt64(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "downloadSpeed")));

    return Result;
}

NanaGet::Aria2::ServersInformation NanaGet::Aria2::ToServersInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::ServersInformation Result;

    Result.Index = Mile::ToUInt64(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "index")));

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
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "downloadSpeed")));

    Result.UploadSpeed = Mile::ToUInt64(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "uploadSpeed")));

    Result.NumActive = Mile::ToUInt64(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "numActive")));

    Result.NumWaiting = Mile::ToUInt64(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "numWaiting")));

    Result.NumStopped = Mile::ToUInt64(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "numStopped")));

    Result.NumStoppedTotal = Mile::ToUInt64(
        Mile::Json::ToString(Mile::Json::GetSubKey(Value, "numStoppedTotal")));

    return Result;
}

NanaGet::Aria2::VersionInformation NanaGet::Aria2::ToVersionInformation(
    nlohmann::json const& Value)
{
    VersionInformation Result;

    Result.Version = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "version"));

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

    Result.SessionId = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "sessionId"));

    return Result;
}
