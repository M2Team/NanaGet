/*
 * PROJECT:   NanaGet
 * FILE:      NanaGet.JsonRpc2.h
 * PURPOSE:   Definition for the NanaGet.JsonRpc2
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef NANAGET_JSONRPC2
#define NANAGET_JSONRPC2

#if (defined(__cplusplus) && __cplusplus >= 201703L)
#elif (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#else
#error "[NanaGet.JsonRpc2] You should use a C++ compiler with the C++17 standard."
#endif

#include <cstdint>
#include <string>

#include <json.hpp>

namespace NanaGet::JsonRpc2
{
    /**
     * @brief The request message of a JSON-RPC 2.0 call.
     */
    struct RequestMessage
    {
        /**
         * @brief true if this request is a notification. Otherwise false.
         */
        bool IsNotification = false;
        /**
         * @brief A string containing the name of the method to be invoked.
         *        method names that begin with the word rpc followed by a period
         *        character (U+002E or ASCII 46) are reserved for rpc-internal
         *        methods and extensions and MUST NOT be used for anything else.
         */
        std::string Method;
        /**
         * @brief A structured JSON value that holds the parameter values to be
         *        used during the invocation of the method. This member MAY be
         *        omitted.
         */
        std::string Parameters;
        /**
         * @brief An identifier established by the client. If it is not included
         *        it is assumed to be a notification. The value SHOULD normally
         *        not be NULL and Numbers SHOULD NOT contain fractional parts.
         */
        std::string Identifier;
    };

    std::string FromRequestMessage(
        RequestMessage const& Value);

    /**
     * @brief The message returned when a JSON-RPC 2.0 call encounters an error.
     */
    struct ErrorMessage
    {
        /**
         * @brief A number that indicates the error type that occurred.
         */
        std::int64_t Code;
        /**
         * @brief A string providing a short description of the error. The
         *        message SHOULD be limited to a concise single sentence.
         */
        std::string Message;
        /**
         * @brief A primitive or structured JSON value that contains additional
         *        information about the error. This may be omitted. The value
         *        of this member is defined by the Server (e.g. detailed error
         *        information, nested errors etc.).
         */
        std::string Data;
    };

    std::string FromErrorMessage(
        ErrorMessage const& Value);

    ErrorMessage ToErrorMessage(
        nlohmann::json const& Value);

    /**
     * @brief The response message of a JSON-RPC 2.0 call.
     */
    struct ResponseMessage
    {
        /**
         * @brief true if this JSON-RPC 2.0 call is succeed. Otherwise false.
         */
        bool IsSucceeded = false;
        /**
         * @brief The value of this member is determined by the method invoked
         *        on the Server if IsSucceeded is true, or return a JSON string
         *        which represents ErrorMessage.
         */
        std::string Message;
        /**
         * @brief It MUST be the same as the value of the id member in the
         *        request message. If there was an error in detecting the id
         *        in the request message (e.g. Parse error/Invalid Request),
         *        it MUST be NULL.
         */
        std::string Identifier;
    };

    bool ToResponseMessage(
        std::string const& Source,
        ResponseMessage& Destination);
}

#endif // !NANAGET_JSONRPC2
