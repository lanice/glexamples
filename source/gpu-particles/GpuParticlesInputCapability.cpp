
#include "GpuParticlesInputCapability.h"


GpuParticlesInputCapability::GpuParticlesInputCapability()
:   m_paused(false)
{
}

GpuParticlesInputCapability::~GpuParticlesInputCapability()
{
}

bool GpuParticlesInputCapability::paused() const
{
    return m_paused;
}

void GpuParticlesInputCapability::onMouseMove(int /*x*/, int /*y*/)
{
}

void GpuParticlesInputCapability::onMousePress(int /*x*/, int /*y*/, gloperate::MouseButton /*button*/)
{
}

void GpuParticlesInputCapability::onMouseRelease(int /*x*/, int /*y*/, gloperate::MouseButton /*button*/)
{
}

void GpuParticlesInputCapability::onMouseDoubleClick(int /*x*/, int /*y*/, gloperate::MouseButton /*button*/)
{
}

void GpuParticlesInputCapability::onMouseWheel(int /*dx*/, int /*dy*/)
{
}

void GpuParticlesInputCapability::onKeyDown(gloperate::Key key)
{
    if (key == gloperate::Key::KeyP)
    {
        m_paused = !m_paused;
    }
}

void GpuParticlesInputCapability::onKeyUp(gloperate::Key /*key*/)
{
}
