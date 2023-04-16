
#pragma once

#include <Containers/Queue.h>
#include <vector>
#include <memory>
#include <vector>
#include <deque>
#include <string_view>

struct _GstRTSPMountPoints;


class GSTREAMER_API IPixelBuffer
{
public:
    virtual ~IPixelBuffer() = default;

public:
    virtual void*       GetData()          = 0;
    virtual std::size_t GetSize()          = 0;
    virtual bool        IsComplete() const = 0;

private:
};

class GSTREAMER_API GstMedia
{
private:
    static constexpr auto queue_size = 10;

public:
    GstMedia(std::string_view config, std::string_view url, _GstRTSPMountPoints* mount);
    ~GstMedia();
    class GstMediaImpl;

public:
    void Push(std::shared_ptr<IPixelBuffer> Image);
    void Publish();

private:
    std::deque<std::shared_ptr<IPixelBuffer>> m_images;
    std::unique_ptr<GstMediaImpl>             m_impl;
};

class GSTREAMER_API GstServer
{
public:
    static GstServer* Get()
    {
        static GstServer instance;
        return &instance;
    }
    ~GstServer();

public:
    GstServer& operator=(GstServer const&) = delete;
    GstServer& operator=(GstServer&&)      = delete;
    GstServer(GstServer const&)            = delete;
    GstServer(GstServer&&)                 = delete;

public:
    std::unique_ptr<GstMedia> GetMedia(std::string_view config, std::string_view url);

private:
    GstServer();
    class GstServerInfo;
    std::unique_ptr<GstServerInfo> m_info;
};