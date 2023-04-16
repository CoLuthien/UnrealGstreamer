
#include "GStreamerModule.h"
extern "C"
{
#include <gst/gst.h>
#include <gst/gstregistry.h>
#include <gst/rtsp-server/rtsp-server.h>
}

bool
Initialize(char const* bin, char const* plugin)
{
    int    argc = 0;
    char** argv = nullptr;
    gst_init(&argc, &argv);

    guint major, minor, micro, nano;
    gst_version(&major, &minor, &micro, &nano);

    GstRegistry* registry = gst_registry_get();
    gst_registry_scan_path(registry, bin);
    gst_registry_scan_path(registry, plugin);

    return (bool)gst_is_initialized();
}