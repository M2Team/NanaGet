/*
 * PROJECT:   NanaGet
 * FILE:      NanaGet.JsonRpc2.cpp
 * PURPOSE:   Implementation for the NanaGet.JsonRpc2
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "NanaGet.JsonRpc2.h"

std::string NanaGet::JsonRpc2::FromRequestMessage(
    NanaGet::JsonRpc2::RequestMessage const& Value)
{
    try
    {
        nlohmann::json ResultJson;
        ResultJson["jsonrpc"] = "2.0";
        ResultJson["method"] = Value.Method;
        ResultJson["params"] = nlohmann::json::parse(Value.Parameters);
        if (!Value.IsNotification)
        {
            ResultJson["id"] = Value.Identifier;
        }
        return ResultJson.dump(2);
    }
    catch (...)
    {
        return std::string();
    }
}

std::string NanaGet::JsonRpc2::FromErrorMessage(
    NanaGet::JsonRpc2::ErrorMessage const& Value)
{
    try
    {
        nlohmann::json ResultJson;
        ResultJson["code"] = Value.Code;
        ResultJson["message"] = Value.Message;
        ResultJson["data"] = nlohmann::json::parse(Value.Data);
        return ResultJson.dump(2);
    }
    catch (...)
    {
        return std::string();
    }
}

NanaGet::JsonRpc2::ErrorMessage NanaGet::JsonRpc2::ToErrorMessage(
    nlohmann::json const& Value)
{
    NanaGet::JsonRpc2::ErrorMessage Result;

    try
    {
        Result.Code = Value.at("code").get<std::int64_t>();
    }
    catch (...)
    {

    }

    try
    {
        Result.Message = Value.at("message").get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        Result.Data = Value.at("data").dump(2);
    }
    catch (...)
    {

    }

    return Result;
}

bool NanaGet::JsonRpc2::ToResponseMessage(
    std::string const& Source,
    NanaGet::JsonRpc2::ResponseMessage& Destination)
{
    nlohmann::json SourceJson;

    try
    {
        SourceJson = nlohmann::json::parse(Source);
    }
    catch (...)
    {
        return false;
    }

    if (!SourceJson.contains("jsonrpc"))
    {
        return false;
    }

    if ("2.0" != SourceJson["jsonrpc"].get<std::string>())
    {
        return false;
    }

    try
    {
        Destination.Identifier = SourceJson.at("id").get<std::string>();
    }
    catch (...)
    {
        return false;
    }

    Destination.IsSucceeded = SourceJson.contains("result");
    if (Destination.IsSucceeded)
    {
        try
        {
            Destination.Message = SourceJson.at("result").dump(2);
        }
        catch (...)
        {

        }
    }
    else
    {
        try
        {
            Destination.Message = SourceJson.at("error").dump(2);
        }
        catch (...)
        {
            return false;
        }
    }

    return true;
}
