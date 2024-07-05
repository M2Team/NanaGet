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
        ResultJson["id"] = Value.Identifier;
        return ResultJson.dump(2);
    }
    catch (...)
    {
        return std::string();
    }
}

bool NanaGet::JsonRpc2::ToNotificationMessage(
    std::string const& Source,
    NanaGet::JsonRpc2::NotificationMessage& Destination)
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

    Destination.Method = Mile::Json::ToString(
        Mile::Json::GetSubKey(SourceJson, "method"));

    nlohmann::json Parameters = Mile::Json::GetSubKey(SourceJson, "params");
    if (!Parameters.is_null())
    {
        Destination.Parameters = Parameters.dump(2);
    }

    return true;
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

    Result.Code = Mile::Json::ToInt64(
        Mile::Json::GetSubKey(Value, "code"));

    Result.Message = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "message"));

    nlohmann::json Data = Mile::Json::GetSubKey(Value, "data");
    if (!Data.is_null())
    {
        Result.Data = Data.dump(2);
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

    std::string JsonRpc = Mile::Json::ToString(
        Mile::Json::GetSubKey(SourceJson, "jsonrpc"));
    if ("2.0" != JsonRpc)
    {
        return false;
    }

    nlohmann::json Identifier = Mile::Json::GetSubKey(SourceJson, "id");
    if (Identifier.is_null())
    {
        return false;
    }
    Destination.Identifier = Mile::Json::ToString(Identifier);

    nlohmann::json Message = Mile::Json::GetSubKey(SourceJson, "result");
    Destination.IsSucceeded = !Message.is_null();
    if (!Destination.IsSucceeded)
    {
        Message = Mile::Json::GetSubKey(SourceJson, "error");
        if (Message.is_null())
        {
            return false;
        }

    }
    Destination.Message = Message.dump(2);

    return true;
}
