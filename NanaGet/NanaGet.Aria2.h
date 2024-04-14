/*
 * PROJECT:   NanaGet
 * FILE:      NanaGet.Aria2.h
 * PURPOSE:   Definition for the NanaGet.Aria2
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef NANAGET_ARIA2
#define NANAGET_ARIA2

#if (defined(__cplusplus) && __cplusplus >= 201703L)
#elif (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#else
#error "[NanaGetCore] You should use a C++ compiler with the C++17 standard."
#endif

#include <cstdint>
#include <ctime>
#include <string>
#include <vector>

namespace NanaGet::Aria2
{
    /**
     * @brief The GID (or gid) is a key to manage each download. Each download
     *        will be assigned a unique GID. The GID is stored as 64-bit binary
     *        value in aria2. For RPC access, it is represented as a hex string
     *        of 16 characters (e.g., 2089b05ecca3d829). Normally, aria2
     *        generates this GID for each download, but the user can specify
     *        GIDs manually using the --gid option. When querying downloads by
     *        GID, you can specify only the prefix of a GID as long as it is
     *        unique among others.
     */
    using DownloadGid = std::uint64_t;

    /**
     * @brief The status of download item.
     */
    enum class DownloadStatus : std::int32_t
    {
        /**
         * @brief Indicating currently downloading/seeding.
         */
        Active,
        /**
         * @brief Indicating in the queue; download is not started.
         */
        Waiting,
        /**
         * @brief Indicating the download is paused.
         */
        Paused,
        /**
         * @brief Indicating stopped and completed download.
         */
        Complete,
        /**
         * @brief Indicating stopped download because of error.
         */
        Error,
        /**
         * @brief Indicating removed by user's discretion.
         */
        Removed,
    };

    /**
     * @brief The status of URI.
     */
    enum class UriStatus : std::int32_t
    {
        /**
         * @brief Indicating the URI has been used.
         */
        Used,
        /**
         * @brief Indicating the URI has not been used.
         */
        Waiting,
    };

    /**
     * @brief The information of URI and its status.
     */
    struct UriInformation
    {
        /**
         * @brief URI
         */
        std::string Uri;
        /**
         * @brief The status of URI.
         */
        UriStatus Status;
    };

    /**
     * @brief The information of file to download.
     */
    struct FileInformation
    {
        /**
         * @brief Index of the file, starting at 1, in the same order as files
         *        appear in the multi-file torrent.
         */
        std::size_t Index;
        /**
         * @brief File path.
         */
        std::string Path;
        /**
         * @brief File size in bytes.
         */
        std::size_t Length;
        /**
         * @brief Completed length of this file in bytes. Please note that it is
         *        possible that sum of "CompletedLength" is less than the
         *        "CompletedLength" returned by the "aria2.tellStatus()" method.
         *        This is because "CompletedLength" in "aria2.getFiles()" only
         *        includes completed pieces. On the other hand,
         *        "CompletedLength" in "aria2.tellStatus()" also includes
         *        partially completed pieces.
         */
        std::size_t CompletedLength;
        /**
         * @brief true if this file is selected by "--select-file" option. If
         *        "--select-file" is not specified or this is single-file
         *        torrent or not a torrent download at all, this value is always
         *        true. Otherwise false.
         */
        bool Selected;
        /**
         * @brief Returns a list of URIs for this file. The element type is the
         *        same struct used in the "aria2.getUris()" method.
         */
        std::vector<UriInformation> Uris;
    };

    /**
     * @brief The file mode of the torrent.
     */
    enum class BitTorrentFileMode : std::int32_t
    {
        /**
         * @brief Indicating no mode. This value is used when file mode is not
         *        available.
         */
        None,
        /**
         * @brief Indicating single file torrent.
         */
        Single,
        /**
         * @brief Indicating multi file torrent.
         */
        Multi,
    };
    /**
     * @brief The Info dictionary information retrieved from the .torrent
     *        (file).
     */
    struct BitTorrentInfoDictionaryInformation
    {
        /**
         * @brief "name" in "info" dictionary. "name.utf-8" is used if
         *        available.
         */
        std::string Name;
    };

    /**
     * @brief The information retrieved from the .torrent (file).
     */
    struct BitTorrentInformation
    {
        /**
         * @brief List of lists of announce URIs. If the torrent contains
         *        "announce" and no "announce-list", "announce" is converted to
         *        the "announce-list" format.
         */
        std::vector<std::vector<std::string>> AnnounceList;
        /**
         * @brief The comment of the torrent. "comment.utf-8" is used if
         *        available.
         */
        std::string Comment;
        /**
         * @brief The creation time of the torrent. The value is an integer
         *        since the epoch, measured in seconds.
         */
        std::time_t CreationDate;
        /**
         * @brief File mode of the torrent.
         */
        BitTorrentFileMode Mode;
        /**
         * @brief Returns the information retrieved from the Info dictionary.
         */
        BitTorrentInfoDictionaryInformation Info;
    };

    /**
     * @brief The information of download item.
     */
    struct DownloadInformation
    {
        /**
         * @brief GID of the download.
         */
        DownloadGid Gid;
        /**
         * @brief Status of the download.
         */
        DownloadStatus Status;
        /**
         * @brief Total length of the download in bytes.
         */
        std::size_t TotalLength;
        /**
         * @brief Completed length of the download in bytes.
         */
        std::size_t CompletedLength;
        /**
         * @brief Uploaded length of the download in bytes.
         */
        std::size_t UploadLength;
        /**
         * @brief Hexadecimal representation of the download progress. The
         *        highest bit corresponds to the piece at index 0. Any set bits
         *        indicate loaded pieces, while unset bits indicate not yet
         *        loaded and/or missing pieces. Any overflow bits at the end are
         *        set to zero. When the download was not started yet, this key
         *        will not be included in the response.
         */
        std::string Bitfield;
        /**
         * @brief Download speed of this download measured in bytes/sec.
         */
        std::size_t DownloadSpeed;
        /**
         * @brief Upload speed of this download measured in bytes/sec.
         */
        std::size_t UploadSpeed;
        /**
         * @brief InfoHash. BitTorrent only.
         */
        std::string InfoHash;
        /**
         * @brief The number of seeders aria2 has connected to. BitTorrent only.
         */
        std::size_t NumSeeders;
        /**
         * @brief true if the local endpoint is a seeder. Otherwise false.
         *        BitTorrent only.
         */
        bool Seeder;
        /**
         * @brief Piece length in bytes.
         */
        std::size_t PieceLength;
        /**
         * @brief The number of pieces.
         */
        std::size_t NumPieces;
        /**
         * @brief The number of peers/servers aria2 has connected to.
         */
        std::int32_t Connections;
        /**
         * @brief The code of the last error for this item, if any. The value
         *        is a string. The error codes are defined in the EXIT STATUS
         *        section. This value is only available for stopped/completed
         *        downloads.
         */
        std::int32_t ErrorCode;
        /**
         * @brief The (hopefully) human readable error message associated to
         *        "ErrorCode".
         */
        std::string ErrorMessage;
        /**
         * @brief List of GIDs which are generated as the result of this
         *        download. For example, when aria2 downloads a Metalink file,
         *        it generates downloads described in the Metalink (see the
         *        "--follow-metalink" option). This value is useful to track
         *        auto-generated downloads. If there are no such downloads,
         *        this key will not be included in the response.
         */
        std::vector<DownloadGid> FollowedBy;
        /**
         * @brief The reverse link for "FollowedBy". A download included in
         *        "FollowedBy" has this object's GID in its "Following" value.
         */
        DownloadGid Following;
        /**
         * @brief GID of a parent download. Some downloads are a part of another
         *        download. For example, if a file in a Metalink has BitTorrent
         *        resources, the downloads of ".torrent" files are parts of that
         *        parent. If this download has no parent, this key will not be
         *        included in the response.
         */
        DownloadGid BelongsTo;
        /**
         * @brief Directory to save files.
         */
        std::string Dir;
        /**
         * @brief Returns the list of files. The elements of this list are the
         *        same structs used in "aria2.getFiles()" method.
         */
        std::vector<FileInformation> Files;
        /**
         * @brief Returns the information retrieved from the .torrent (file).
         *        BitTorrent only.
         */
        BitTorrentInformation BitTorrent;
        /**
         * @brief The number of verified number of bytes while the files are
         *        being hash checked. This key exists only when this download is
         *        being hash checked.
         */
        std::size_t VerifiedLength;
        /**
         * @brief true if this download is waiting for the hash check in a
         *        queue. This key exists only when this download is in the
         *        queue.
         */
        bool VerifyIntegrityPending;
    };

    /**
     * @brief The information of peers of the download.
     */
    struct PeerInformation
    {
        /**
         * @brief Percent-encoded peer ID.
         */
        std::string PeerId;
        /**
         * @brief IP address of the peer.
         */
        std::string Ip;
        /**
         * @brief Port number of the peer.
         */
        std::uint16_t Port;
        /**
         * @brief Hexadecimal representation of the download progress of the
         *        peer. The highest bit corresponds to the piece at index 0. Set
         *        bits indicate the piece is available and unset bits indicate
         *        the piece is missing. Any spare bits at the end are set to
         *        zero.
         */
        std::string Bitfield;
        /**
         * @brief true if aria2 is choking the peer. Otherwise false.
         */
        bool AmChoking;
        /**
         * @brief true if the peer is choking aria2. Otherwise false.
         */
        bool PeerChoking;
        /**
         * @brief Download speed (byte/sec) that this client obtains from the
         *        peer.
         */
        std::size_t DownloadSpeed;
        /**
         * @brief Upload speed(byte/sec) that this client uploads to the peer.
         */
        std::size_t UploadSpeed;
        /**
         * @brief true if this peer is a seeder. Otherwise false.
         */
        bool Seeder;
    };

    /**
     * @brief The information of currently connected HTTP(S)/FTP/SFTP server of
     *        the download.
     */
    struct ServerInformation
    {
        /**
         * @brief Original URI.
         */
        std::string Uri;
        /**
         * @brief This is the URI currently used for downloading. If redirection
         *        is involved, currentUri and uri may differ.
         */
        std::string CurrentUri;
        /**
         * @brief Download speed (byte/sec).
         */
        std::size_t DownloadSpeed;
    };

    /**
     * @brief The information of currently connected HTTP(S)/FTP/SFTP servers of
     *        the download.
     */
    struct ServersInformation
    {
        /**
         * @brief Index of the file, starting at 1, in the same order as files
         *        appear in the multi-file metalink.
         */
        std::size_t Index;
        /**
         * @brief A list of the information of currently connected
         *        HTTP(S)/FTP/SFTP server of the download.
         */
        std::vector<ServerInformation> Servers;
    };

    /**
     * @brief The information of global statistics of current aria2 session.
     */
    struct GlobalStatusInformation
    {
        /**
         * @brief Overall download speed (byte/sec).
         */
        std::size_t DownloadSpeed;
        /**
         * @brief Overall upload speed(byte/sec).
         */
        std::size_t UploadSpeed;
        /**
         * @brief The number of active downloads.
         */
        std::size_t NumActive;
        /**
         * @brief The number of waiting downloads.
         */
        std::size_t NumWaiting;
        /**
         * @brief The number of stopped downloads in the current session. This
         *        value is capped by the "--max-download-result" option.
         */
        std::size_t NumStopped;
        /**
         * @brief The number of stopped downloads in the current session and not
         *        capped by the "--max-download-result" option.
         */
        std::size_t NumStoppedTotal;
    };

    /**
     * @brief The information of the version of aria2 and the list of enabled
     *        features.
     */
    struct VersionInformation
    {
        /**
         * @brief Version number of aria2 as a string.
         */
        std::string Version;
        /**
         * @brief List of enabled features. Each feature is given as a string.
         */
        std::vector<std::string> EnabledFeatures;
    };
    
    /**
     * @brief The information of current aria2 session.
     */
    struct SessionInformation
    {
        /**
         * @brief Session ID, which is generated each time when aria2 is
         *        invoked.
         */
        std::string SessionId;
    };
}

#endif // !NANAGET_ARIA2
