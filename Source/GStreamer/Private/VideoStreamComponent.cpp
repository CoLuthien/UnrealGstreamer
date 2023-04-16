
#include "VideoStreamComponent.h"

#include "GstServer.h"
#include <Engine/TextureRenderTarget2D.h>
#include <Components/SceneCaptureComponent2D.h>
#include <Kismet/GameplayStatics.h>
#include <vector>
#include <string_view>

static constexpr std::string_view url_base = "/";
static constexpr std::string_view base_config =
    "appsrc name=src is-live=true block=true format=GST_FORMAT_TIME "
    "caps=video/x-raw,format=BGRA,width=1920,height=1080,framerate=60/1 ! videoconvert ! "
    "video/x-raw,format=NV12 ! x264enc speed-preset=ultrafast bitrate=16392 tune=zerolatency ! "
    "rtph264pay config-interval=0 name=pay0 pt=96 ";



 

UVideoStreamComponent::UVideoStreamComponent() : Super()
{
    this->bCaptureEveryFrame          = true;
    PrimaryComponentTick.bCanEverTick = true;
}

class UVideoStreamComponent::PixelBuffer final : public IPixelBuffer
{
public:
    PixelBuffer(int W, int H) { m_buffer.Reserve(W * H); }

public:
    void                SetData(TArray<FColor>&& image) { m_buffer = MoveTemp(image); }
    TArray<FColor>&     GetArray() { return m_buffer; }
    virtual void*       GetData() override { return m_buffer.GetData(); }
    virtual std::size_t GetSize() override { return m_buffer.Num() * sizeof(FColor); }
    void                Submit() { m_fence.BeginFence(); }
    virtual bool        IsComplete() const override { return m_fence.IsFenceComplete(); }

public:
    FRenderCommandFence m_fence;
    TArray<FColor>      m_buffer;
};

void
UVideoStreamComponent::TickComponent(float                        DeltaTime,
                                     ELevelTick                   TickType,
                                     FActorComponentTickFunction* ThisTickFunction)

{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (m_media && m_streamable)
    {
        auto capture = GenerateCapture();
        m_media->Push(std::move(capture));
        m_media->Publish();
    }
}

void
UVideoStreamComponent::ConfigureStream(int W, int H, std::string_view url, std::string_view config)
{
    auto server = GstServer::Get();

    Width = W, Height = H;
    stream_config = config.data();
    stream_url    = url.data();
    m_media       = server->GetMedia(config, url);
}

void
UVideoStreamComponent::ConfigureCapture()
{
    if (nullptr == TextureTarget->GameThread_GetRenderTargetResource()) // check initialization
    {
        this->TextureTarget->InitCustomFormat(Width, Height, PF_B8G8R8A8, false);
        this->TextureTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8_SRGB;
        this->CaptureSource                     = ESceneCaptureSource::SCS_SceneColorHDR;
    }
    else
    {
        this->TextureTarget->ResizeTarget(Width, Height);
    }
}

void
UVideoStreamComponent::BeginPlay()
{
    Super::BeginPlay();
    this->TextureTarget = NewObject<UTextureRenderTarget2D>();
    FString&& OwnerName = GetOwner()->GetName();

    std::string_view url = "/test";
    std::string_view config = TCHAR_TO_ANSI(*stream_config);
    ConfigureStream(Width, Height, url, base_config);
    ConfigureCapture();
}

void
UVideoStreamComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    m_media.reset();
}

bool
UVideoStreamComponent::ToggleStreaming()
{
    m_streamable = (!m_streamable);
    return m_streamable;
}

std::shared_ptr<UVideoStreamComponent::PixelBuffer>
UVideoStreamComponent::GenerateCapture()
{
    FTextureRenderTargetResource* RenderTarget =
        this->TextureTarget->GameThread_GetRenderTargetResource();
    FIntPoint Size    = RenderTarget->GetSizeXY();
    FIntRect  ImgSize = FIntRect(0, 0, Size.X, Size.Y);
    auto      request = std::make_shared<PixelBuffer>(Width, Height);
    ENQUEUE_RENDER_COMMAND(ReadSurfaceCommand)
    (
        [RenderTarget, ImgSize, request](FRHICommandListImmediate& RHICmdList)
        {
            RHICmdList.ReadSurfaceData(RenderTarget->GetRenderTargetTexture(),
                                       ImgSize,
                                       request->m_buffer,
                                       FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX));
        });
    request->Submit();

    return request;
}
