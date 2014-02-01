#ifndef ROCKETCOREUIELEMENTSCROLL_H
#define ROCKETCOREUIELEMENTSCROLL_H

#include "Rocket/Core/Context.h"
#include "Rocket/Core/Element.h"
#include "Rocket/Core/EventListener.h"

namespace Rocket {
namespace Core {

class UIElementScroll : public Rocket::Core::Element, public Rocket::Core::EventListener
{
public:
    UIElementScroll(const Rocket::Core::String& tag);
    ~UIElementScroll();
    static void registerWithlibRocket();

    virtual void OnChildAdd(Rocket::Core::Element* child);

    virtual void ProcessEvent(Rocket::Core::Event& event);

    virtual void OnUpdate();

private:
    Rocket::Core::Context* mContext;

    bool mScrolling;
    bool mHasMoved;
    int mMouseY;
    int mMouseDownY;
    float mMouseDownTime;

    float mLastUpdate;

    float mScrollValue;
    float mScrollVelocity;
};

}
}

#endif
