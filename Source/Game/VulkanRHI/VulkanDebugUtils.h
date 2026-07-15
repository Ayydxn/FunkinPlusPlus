#pragma once

#define VK_CHECK_RESULT(Expression, Handle, Message, ...) \
    do { \
        auto ResultValue = (Expression); \
        if (ResultValue.result != vk::Result::eSuccess) \
        { \
            LOG_ERROR_TAG("VulkanRHI", "{} ({})", fmt::format(fmt::runtime(Message), ##__VA_ARGS__), vk::to_string(ResultValue.result)); \
            verifyFunkinf(false, "A Vulkan operation failed: {} ({})", fmt::format(fmt::runtime(Message), ##__VA_ARGS__), vk::to_string(ResultValue.result)); \
        } \
        else \
        { \
            (Handle) = ResultValue.value; \
        } \
    } while (false);

class CVulkanDebugUtils
{
public:
    static std::vector<const char*> GetSupportedValidationLayers();
};
