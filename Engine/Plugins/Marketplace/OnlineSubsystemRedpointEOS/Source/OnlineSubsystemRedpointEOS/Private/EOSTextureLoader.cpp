// Copyright June Rhodes. All Rights Reserved.

// Parts of this file are based on AsyncTaskDownloadImage.cpp.

#include "OnlineSubsystemRedpointEOS/Shared/EOSTextureLoader.h"

#if EOS_HAS_IMAGE_DECODING
#include "Engine/Texture2DDynamic.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#endif

EOS_ENABLE_STRICT_WARNINGS

#if EOS_HAS_IMAGE_DECODING

static void WriteRawToTexture_RenderThread(
    FTexture2DDynamicResource *TextureResource,
    TArray64<uint8> *RawData,
    bool bUseSRGB = true)
{
    check(IsInRenderingThread());

    if (TextureResource)
    {
        FRHITexture2D *TextureRHI = TextureResource->GetTexture2DRHI();

        int32 Width = TextureRHI->GetSizeX();
        int32 Height = TextureRHI->GetSizeY();

        uint32 DestStride = 0;
        uint8 *DestData =
            reinterpret_cast<uint8 *>(RHILockTexture2D(TextureRHI, 0, RLM_WriteOnly, DestStride, false, false));

        for (int32 y = 0; y < Height; y++)
        {
            uint8 *DestPtr = &DestData[((int64)Height - 1 - y) * DestStride];

            const FColor *SrcPtr = &((FColor *)(RawData->GetData()))[((int64)Height - 1 - y) * Width];
            for (int32 x = 0; x < Width; x++)
            {
                *DestPtr++ = SrcPtr->B;
                *DestPtr++ = SrcPtr->G;
                *DestPtr++ = SrcPtr->R;
                *DestPtr++ = SrcPtr->A;
                SrcPtr++;
            }
        }

        RHIUnlockTexture2D(TextureRHI, 0, false, false);
    }

    delete RawData;
}

#endif

UTexture *FEOSTextureLoader::LoadTextureFromHttpResponse(const FHttpResponsePtr &Response)
{
#if EOS_HAS_IMAGE_DECODING
    if (!(Response.IsValid() && Response->GetContentLength() > 0))
    {
        return nullptr;
    }

    EImageFormat ImageFormat = EImageFormat::Invalid;
    if (Response->GetHeader("Content-Type") == "image/jpeg")
    {
        ImageFormat = EImageFormat::JPEG;
    }
    else if (Response->GetHeader("Content-Type") == "image/png")
    {
        ImageFormat = EImageFormat::PNG;
    }
    else
    {
        return nullptr;
    }

    IImageWrapperModule &ImageWrapperModule =
        FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
    if (ImageWrapper.IsValid() &&
        ImageWrapper->SetCompressed(Response->GetContent().GetData(), Response->GetContentLength()))
    {
        TArray64<uint8> *RawData = new TArray64<uint8>();
        const ERGBFormat InFormat = ERGBFormat::BGRA;
        if (ImageWrapper->GetRaw(InFormat, 8, *RawData))
        {
            if (UTexture2DDynamic *Texture =
                    UTexture2DDynamic::Create(ImageWrapper->GetWidth(), ImageWrapper->GetHeight()))
            {
                Texture->SRGB = true;
                Texture->UpdateResource();

#if defined(UE_5_0_OR_LATER)
                FTexture2DDynamicResource *TextureResource =
                    static_cast<FTexture2DDynamicResource *>(Texture->GetResource());
#else
                FTexture2DDynamicResource *TextureResource =
                    static_cast<FTexture2DDynamicResource *>(Texture->Resource);
#endif // #if defined(UE_5_0_OR_LATER)
                if (TextureResource)
                {
                    ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)
                    ([TextureResource, RawData](FRHICommandListImmediate &RHICmdList) {
                        WriteRawToTexture_RenderThread(TextureResource, RawData);
                    });
                }
                else
                {
                    delete RawData;
                }

                return Texture;
            }
        }
    }
#endif

    return nullptr;
}

EOS_DISABLE_STRICT_WARNINGS