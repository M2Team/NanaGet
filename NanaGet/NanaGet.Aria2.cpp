﻿/*
 * PROJECT:   NanaGet
 * FILE:      NanaGet.Aria2.cpp
 * PURPOSE:   Implementation for the NanaGet.Aria2
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "NanaGet.Aria2.h"

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

NanaGet::Aria2::DownloadGid NanaGet::Aria2::ToDownloadGid(
    nlohmann::json const& Value)
{
    try
    {
        return std::strtoull(
            Value.get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {
        return 0;
    }
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

    try
    {
        Result.Uri = Value.at("uri").get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        Result.Status = NanaGet::Aria2::ToUriStatus(Value.at("status"));
    }
    catch (...)
    {

    }

    return Result;
}

NanaGet::Aria2::FileInformation NanaGet::Aria2::ToFileInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::FileInformation Result;

    try
    {
        Result.Index = std::strtoull(
            Value.at("index").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        Result.Path = Value.at("path").get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        Result.Length = std::strtoull(
            Value.at("length").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        Result.CompletedLength = std::strtoull(
            Value.at("completedLength").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        std::string Selected = Value.at("selected").get<std::string>();
        if (0 == std::strcmp(Selected.c_str(), "true"))
        {
            Result.Selected = true;
        }
        else if (0 == std::strcmp(Selected.c_str(), "false"))
        {
            Result.Selected = false;
        }
    }
    catch (...)
    {

    }

    try
    {
        for (nlohmann::json const& Uri : Value.at("uris"))
        {
            Result.Uris.emplace_back(NanaGet::Aria2::ToUriInformation(Uri));
        }
    }
    catch (...)
    {

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

    try
    {
        Result.Name = Value.at("name").get<std::string>();
    }
    catch (...)
    {

    }

    return Result;
}

NanaGet::Aria2::BitTorrentInformation NanaGet::Aria2::ToBitTorrentInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::BitTorrentInformation Result;

    try
    {
        for (nlohmann::json const& Item : Value.at("announceList"))
        {
            std::vector<std::string> Content;
            for (nlohmann::json const& InnerItem : Item)
            {
                Content.emplace_back(InnerItem.get<std::string>());
            }
            Result.AnnounceList.emplace_back(Content);
        }
    }
    catch (...)
    {

    }

    try
    {
        Result.Comment = Value.at("comment").get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        Result.CreationDate = std::strtoull(
            Value.at("creationDate").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        Result.Mode = NanaGet::Aria2::ToBitTorrentFileMode(Value.at("mode"));
    }
    catch (...)
    {

    }

    try
    {
        Result.Info = NanaGet::Aria2::ToBitTorrentInfoDictionary(
            Value.at("info"));
    }
    catch (...)
    {

    }

    return Result;
}

NanaGet::Aria2::DownloadInformation NanaGet::Aria2::ToDownloadInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::DownloadInformation Result;

    try
    {
        Result.Gid = NanaGet::Aria2::ToDownloadGid(Value.at("gid"));
    }
    catch (...)
    {

    }

    try
    {
        Result.Status = NanaGet::Aria2::ToDownloadStatus(Value.at("status"));
    }
    catch (...)
    {

    }

    try
    {
        Result.TotalLength = std::strtoull(
            Value.at("totalLength").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        Result.CompletedLength = std::strtoull(
            Value.at("completedLength").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        Result.UploadLength = std::strtoull(
            Value.at("uploadLength").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        Result.Bitfield = Value.at("bitfield").get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        Result.DownloadSpeed = std::strtoull(
            Value.at("downloadSpeed").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        Result.UploadSpeed = std::strtoull(
            Value.at("uploadSpeed").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        Result.InfoHash = Value.at("infoHash").get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        Result.NumSeeders = std::strtoull(
            Value.at("numSeeders").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        std::string Seeder = Value.at("seeder").get<std::string>();
        if (0 == std::strcmp(Seeder.c_str(), "true"))
        {
            Result.Seeder = true;
        }
        else if (0 == std::strcmp(Seeder.c_str(), "false"))
        {
            Result.Seeder = false;
        }
    }
    catch (...)
    {

    }

    try
    {
        Result.PieceLength = std::strtoull(
            Value.at("pieceLength").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        Result.NumPieces = std::strtoull(
            Value.at("numPieces").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        Result.Connections = std::strtol(
            Value.at("connections").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        Result.ErrorCode = std::strtol(
            Value.at("errorCode").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        Result.ErrorMessage = Value.at("errorMessage").get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        for (nlohmann::json const& Item : Value.at("followedBy"))
        {
            Result.FollowedBy.emplace_back(NanaGet::Aria2::ToDownloadGid(Item));
        }
    }
    catch (...)
    {

    }

    try
    {
        Result.Following = NanaGet::Aria2::ToDownloadGid(Value.at("following"));
    }
    catch (...)
    {

    }

    try
    {
        Result.BelongsTo = NanaGet::Aria2::ToDownloadGid(Value.at("belongsTo"));
    }
    catch (...)
    {

    }

    try
    {
        Result.Dir = Value.at("dir").get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        for (nlohmann::json const& File : Value.at("files"))
        {
            Result.Files.emplace_back(NanaGet::Aria2::ToFileInformation(File));
        }
    }
    catch (...)
    {

    }

    try
    {
        Result.BitTorrent = NanaGet::Aria2::ToBitTorrentInformation(
            Value.at("bittorrent"));
    }
    catch (...)
    {

    }

    try
    {
        Result.VerifiedLength = std::strtoull(
            Value.at("verifiedLength").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        std::string VerifyIntegrityPending =
            Value.at("verifyIntegrityPending").get<std::string>();
        if (0 == std::strcmp(VerifyIntegrityPending.c_str(), "true"))
        {
            Result.VerifyIntegrityPending = true;
        }
        else if (0 == std::strcmp(VerifyIntegrityPending.c_str(), "false"))
        {
            Result.VerifyIntegrityPending = false;
        }
    }
    catch (...)
    {

    }

    return Result;
}

NanaGet::Aria2::PeerInformation NanaGet::Aria2::ToPeerInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::PeerInformation Result;

    try
    {
        Result.PeerId = Value.at("peerId").get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        Result.Ip = Value.at("ip").get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        Result.Port = static_cast<std::uint16_t>(std::strtoul(
            Value.at("port").get<std::string>().c_str(),
            nullptr,
            10));
    }
    catch (...)
    {

    }

    try
    {
        Result.Bitfield = Value.at("bitfield").get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        std::string AmChoking =
            Value.at("amChoking").get<std::string>();
        if (0 == std::strcmp(AmChoking.c_str(), "true"))
        {
            Result.AmChoking = true;
        }
        else if (0 == std::strcmp(AmChoking.c_str(), "false"))
        {
            Result.AmChoking = false;
        }
    }
    catch (...)
    {

    }

    try
    {
        std::string PeerChoking =
            Value.at("peerChoking").get<std::string>();
        if (0 == std::strcmp(PeerChoking.c_str(), "true"))
        {
            Result.PeerChoking = true;
        }
        else if (0 == std::strcmp(PeerChoking.c_str(), "false"))
        {
            Result.PeerChoking = false;
        }
    }
    catch (...)
    {

    }

    try
    {
        Result.DownloadSpeed = std::strtoull(
            Value.at("downloadSpeed").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        Result.UploadSpeed = std::strtoull(
            Value.at("uploadSpeed").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        std::string Seeder = Value.at("seeder").get<std::string>();
        if (0 == std::strcmp(Seeder.c_str(), "true"))
        {
            Result.Seeder = true;
        }
        else if (0 == std::strcmp(Seeder.c_str(), "false"))
        {
            Result.Seeder = false;
        }
    }
    catch (...)
    {

    }

    return Result;
}

NanaGet::Aria2::ServerInformation NanaGet::Aria2::ToServerInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::ServerInformation Result;

    try
    {
        Result.Uri = Value.at("uri").get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        Result.CurrentUri = Value.at("currentUri").get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        Result.DownloadSpeed = std::strtoull(
            Value.at("downloadSpeed").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    return Result;
}

NanaGet::Aria2::ServersInformation NanaGet::Aria2::ToServersInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::ServersInformation Result;

    try
    {
        Result.Index = std::strtoull(
            Value.at("index").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        for (nlohmann::json const& Server : Value.at("servers"))
        {
            Result.Servers.emplace_back(
                NanaGet::Aria2::ToServerInformation(Server));
        }
    }
    catch (...)
    {

    }

    return Result;
}

NanaGet::Aria2::GlobalStatusInformation NanaGet::Aria2::ToGlobalStatusInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::GlobalStatusInformation Result;

    try
    {
        Result.DownloadSpeed = std::strtoull(
            Value.at("downloadSpeed").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        Result.UploadSpeed = std::strtoull(
            Value.at("uploadSpeed").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        Result.NumActive = std::strtoull(
            Value.at("numActive").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        Result.NumWaiting = std::strtoull(
            Value.at("numWaiting").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        Result.NumStopped = std::strtoull(
            Value.at("numStopped").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    try
    {
        Result.NumStoppedTotal = std::strtoull(
            Value.at("numStoppedTotal").get<std::string>().c_str(),
            nullptr,
            10);
    }
    catch (...)
    {

    }

    return Result;
}

NanaGet::Aria2::VersionInformation NanaGet::Aria2::ToVersionInformation(
    nlohmann::json const& Value)
{
    VersionInformation Result;

    try
    {
        Result.Version = Value.at("version").get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        for (nlohmann::json const& EnabledFeature : Value.at("enabledFeatures"))
        {
            Result.EnabledFeatures.emplace_back(
                EnabledFeature.get<std::string>());
        }
    }
    catch (...)
    {

    }

    return Result;
}

NanaGet::Aria2::SessionInformation NanaGet::Aria2::ToSessionInformation(
    nlohmann::json const& Value)
{
    NanaGet::Aria2::SessionInformation Result;

    try
    {
        Result.SessionId = Value.at("sessionId").get<std::string>();
    }
    catch (...)
    {

    }

    return Result;
}