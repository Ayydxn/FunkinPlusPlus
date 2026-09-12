#pragma once

#include "Application/Window.h"

#include <memory>

enum class ERHIBackend
{
    OpenGL,
    Vulkan,
    Direct3D11,
    Direct3D12,
    Metal
};

enum class ERHIInitializationResultCode
{
    Success,

	NoBackendAvailable,     // The graphics API/runtime (e.g. the Vulkan loader) isn't present on this system.
	NoSuitableAdapter,      // No physical device/adapter met the RHI's minimum requirements.
	MissingFeature          // A physical device/adapter was found, but it lacks a required feature or extension.
};

struct FRHIInitializationResult
{
	ERHIInitializationResultCode ResultCode;
    std::string ErrorTitle;
	std::string ErrorMessage;
    
    explicit operator bool() const { return ResultCode != ERHIInitializationResultCode::Success; }

    static FRHIInitializationResult MakeSuccess()
    {
		return {
            .ResultCode = ERHIInitializationResultCode::Success,
            .ErrorTitle = "",
            .ErrorMessage = ""
        };
    }
    
    static FRHIInitializationResult MakeFailure(ERHIInitializationResultCode ResultCode, const std::string& ErrorTitle, const std::string& ErrorMessage)
    {
        return {
            .ResultCode = ResultCode,
            .ErrorTitle = ErrorTitle,
            .ErrorMessage = ErrorMessage
        };
    }
};

class IRHIContext
{
public:
    virtual ~IRHIContext() = default;
    
    IRHIContext(const IRHIContext&) = delete;
    IRHIContext& operator=(const IRHIContext&) = delete;

    virtual FRHIInitializationResult Initialize(const FNativeWindowHandle& NativeWindowHandle, uint32 InitialWindowWidth, uint32 InitialWindowHeight, bool bRequestVSync) = 0;
    virtual void Destroy() = 0;
    
    virtual void OnWindowResized(uint32 NewWidth, uint32 NewHeight) = 0;
protected:
    IRHIContext() = default;
};

std::unique_ptr<IRHIContext> CreateRHIContext(ERHIBackend RHIBackend);
std::string GetRHIBackendName(ERHIBackend RHIBackend);
