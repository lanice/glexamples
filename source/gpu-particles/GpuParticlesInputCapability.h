
#pragma once



#include <gloperate/painter/AbstractInputCapability.h>


class GpuParticlesInputCapability : public gloperate::AbstractInputCapability
{
public:
    GpuParticlesInputCapability();

    virtual ~GpuParticlesInputCapability();

    bool paused() const;


    // Virtual functions from AbstractInputCapability
    virtual void onMouseMove(int x, int y) override;
    virtual void onMousePress(int x, int y, gloperate::MouseButton button) override;
    virtual void onMouseRelease(int x, int y, gloperate::MouseButton button) override;
    virtual void onMouseDoubleClick(int x, int y, gloperate::MouseButton button) override;
    virtual void onMouseWheel(int dx, int dy) override;
    virtual void onKeyDown(gloperate::Key key) override;
    virtual void onKeyUp(gloperate::Key key) override;


protected:
    bool m_paused;

};


