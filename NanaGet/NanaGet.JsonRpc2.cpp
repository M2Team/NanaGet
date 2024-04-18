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
        ResultJson["data"] = Value.Data;
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
        Result.Data = Value.at("data").get<std::string>();
    }
    catch (...)
    {

    }

    return Result;
}

NanaGet::JsonRpc2::ResponseMessage NanaGet::JsonRpc2::ToResponseMessage(
    nlohmann::json const& Value)
{
    NanaGet::JsonRpc2::ResponseMessage Result;

    if (Value.contains("jsonrpc") &&
        "2.0" == Value["jsonrpc"].get<std::string>())
    {
        Result.IsSucceeded = Value.contains("result");
        if (Result.IsSucceeded)
        {
            try
            {
                Result.Message = Value.at("result").get<std::string>();
            }
            catch (...)
            {

            }
        }
        else
        {
            try
            {
                Result.Message = Value.at("error").get<std::string>();
            }
            catch (...)
            {
                NanaGet::JsonRpc2::ErrorMessage Error;
                Error.Code = -32603;
                Error.Message = "Unspecified error.";
                Result.Message = NanaGet::JsonRpc2::FromErrorMessage(Error);
            }
        }

        try
        {
            Result.Identifier = Value.at("id").get<std::string>();
        }
        catch (...)
        {

        }
    }
    else
    {
        NanaGet::JsonRpc2::ErrorMessage Error;
        Error.Code = -32603;
        Error.Message = "Invalid JSON-RPC 2.0 response message.";
        Result.Message = NanaGet::JsonRpc2::FromErrorMessage(Error);
    }

    return Result;
}
