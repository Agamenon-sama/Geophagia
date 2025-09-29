#pragma once

#include "GuiFrame.h"
#include "../scene/Camera.h"
#include "../Events.h"
#include "../Common.h"

namespace Necrosis {

enum class DevFrameEventType : u8 {
    Unknown,
    WireframeMode,
    Sensitivity,
    Speed
};

struct DevFrameEvent : Event {
    DevFrameEventType type;
    union {
        bool wireframe;
        float fval;
    };
};

class DevFrame : public GuiFrame {
public:
    DevFrame();

    virtual void render() override;
    virtual void uiRender() override;

    Camera camera;

    EventDispatcher<DevFrameEvent> dispatcher;

private:
    bool _wireframeOn = false;
};

}
