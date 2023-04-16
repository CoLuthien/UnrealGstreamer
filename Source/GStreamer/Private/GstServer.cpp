
#include "GstServer.h"

extern "C"
{
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <gst/app/gstappsrc.h>
}

#include <thread>
#include <future>
#include <iostream>
#include <cassert>

static constexpr char const* AppsrcName = "src";

GstMedia::GstMedia(std::string_view     config,
                   std::string_view     url,
                   _GstRTSPMountPoints* mount)
    : m_impl{std::make_unique<GstMediaImpl>(config, url, mount)}
{
}
GstMedia::~GstMedia()
{
    m_impl.reset();
}

typedef struct _source
{
    GstElement* data = nullptr;
} source;

static void
media_configure_callback(GstRTSPMediaFactory* factory, GstRTSPMedia* media, source* self)
{
    GstElement* element = gst_rtsp_media_get_element(media);
    self->data          = gst_bin_get_by_name_recurse_up(GST_BIN(element), "src");
    g_object_set(self->data, "block", false, nullptr);
}

class GSTREAMER_API GstMedia::GstMediaImpl
{
public:
    GstMediaImpl(std::string_view config, std::string_view url, GstRTSPMountPoints* mount)
        : m_mount{mount}, m_url(url)
    {
        m_appsrc                     = std::make_unique<source>();
        GstRTSPMediaFactory* factory = gst_rtsp_media_factory_new();
        gst_rtsp_media_factory_set_launch(factory, config.data());
        gst_rtsp_media_factory_set_eos_shutdown(factory, TRUE);
        gst_rtsp_media_factory_set_shared(factory, TRUE);
        g_signal_connect(factory,
                         "media-configure",
                         (GCallback)media_configure_callback,
                         m_appsrc.get());
        gst_rtsp_mount_points_add_factory(mount, url.data(), factory);
        m_factory = factory;
    }
    ~GstMediaImpl()
    {
        gst_rtsp_mount_points_remove_factory(m_mount, m_url.data());
        gst_app_src_end_of_stream(GST_APP_SRC(m_appsrc->data));
        gst_object_unref(m_appsrc->data);
        m_appsrc.reset();
    }

public:
    GstElement* GetAppsrc() { return m_appsrc->data; }

private:
    GstRTSPMountPoints*     m_mount;
    GstRTSPMediaFactory*    m_factory;
    GstClockTime            m_timestamp;
    std::string             m_url;
    std::unique_ptr<source> m_appsrc;
};

void
GstMedia::Push(std::shared_ptr<IPixelBuffer> image)
{
    auto src = m_impl->GetAppsrc();
    if (src == nullptr)
    {
        return;
    }

    if (m_images.size() > GstMedia::queue_size)
    {
        return;
    }
    m_images.emplace_back(std::move(image));
}

void
GstMedia::Publish()
{
    auto src = m_impl->GetAppsrc();
    if (src == nullptr || m_images.empty())
    {
        return;
    }
    auto image = m_images.front();

    if (image->IsComplete())
    {
        auto future =
            std::async(std::launch::async, [src, image = std::move(image)]() -> void {
                // inherently slow because ue4 render thread works behind
                // for 2~3 frame
                auto buffer = gst_buffer_new_memdup(image->GetData(), image->GetSize());
                auto time   = gst_element_get_current_running_time(src);
                GST_BUFFER_PTS(buffer) = time;
                GST_BUFFER_DTS(buffer) = time;
                gst_app_src_push_buffer(GST_APP_SRC(src), buffer);
            });
        m_images.pop_front();
    }
}

static void
EventWorker(GMainLoop* loop)
{
    g_main_loop_run(loop);
}

class GSTREAMER_API GstServer::GstServerInfo
{
public:
    GstServerInfo(std::string_view address = {})
    {
        static constexpr int    argc = 0;
        static constexpr char** argv = nullptr;
        // initialize gst
        if (!gst_is_initialized())
        {
            gst_init(const_cast<int*>(&argc), const_cast<char***>(&argv));
        }
        m_loop = g_main_loop_new(nullptr, false);
        // create rtsp server
        m_server = gst_rtsp_server_new();
        // waiting for connection, initially, it always return 404 not found
        gst_rtsp_server_set_service(m_server, "8555");

        // get mount point
        m_mount = gst_rtsp_server_get_mount_points(m_server);
        gst_rtsp_server_attach(m_server, nullptr);
        m_worker = std::make_unique<std::thread>(EventWorker, m_loop);
    }

    ~GstServerInfo()
    {

        g_main_loop_quit(m_loop);
        m_worker->join();
        m_worker.reset(nullptr);
        g_main_loop_unref(m_loop);
        gst_object_unref(m_mount);
        gst_object_unref(m_server);
        gst_deinit();
    }

public:
    GstRTSPMountPoints* GetMount() { return m_mount; }

private:
    GstRTSPServer*               m_server;
    GstRTSPMountPoints*          m_mount;
    std::unique_ptr<std::thread> m_worker;
    GMainLoop*                   m_loop;
};

GstServer::GstServer() : m_info{std::make_unique<GstServerInfo>()}
{
}

std::unique_ptr<GstMedia>
GstServer::GetMedia(std::string_view config, std::string_view url)
{
    return std::make_unique<GstMedia>(config, url, m_info->GetMount());
}

GstServer::~GstServer() = default;
