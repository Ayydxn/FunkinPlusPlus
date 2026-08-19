#pragma once

class ICommandBuffer
{
public:
    virtual ~ICommandBuffer() = default;
    
    ICommandBuffer(const ICommandBuffer&) = delete;
    ICommandBuffer& operator=(const ICommandBuffer&) = delete;
protected:
    ICommandBuffer() = default;
};