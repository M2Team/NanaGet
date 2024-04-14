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
        Result.Status = NanaGet::Aria2::ToUriStatus(
            Value.at("status"));
    }
    catch (...)
    {
        Result.Status = NanaGet::Aria2::UriStatus::Used;
    }

    return Result;
}
