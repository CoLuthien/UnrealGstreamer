
#pragma once

#include "GstServer.h"
#include <Components/SceneCaptureComponent2D.h>
#include <Engine/TextureRenderTarget2D.h>
#include <CoreMinimal.h>
#include <memory>

#include "VideoStreamComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GSTREAMER_API UVideoStreamComponent : public USceneCaptureComponent2D
{
    GENERATED_BODY()
public:
    UVideoStreamComponent();

public:
    virtual void TickComponent(float                        DeltaTime,
                               ELevelTick                   TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    bool ToggleStreaming();
    void ConfigureStream(int W, int H, std::string_view url, std::string_view config);
    void ConfigureCapture();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int Width = 1920;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int Height = 1080;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString stream_url;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine = "true"))
    FString stream_config;

private:
    class PixelBuffer;
    std::unique_ptr<GstMedia>            m_media;
    bool                                 m_streamable = true;

private:
    std::shared_ptr<PixelBuffer> GenerateCapture();
};