#include "UIElementScroll.h"
#include <Rocket/Core.h>

namespace Rocket {
namespace Core {

UIElementScroll::UIElementScroll(const Rocket::Core::String& tag) :
Rocket::Core::Element(tag),
    mContext(NULL),
    mScrolling(false),
    mMouseY(0),
    mMouseDownY(0),
    mMouseDownTime(0.0f),
    mLastUpdate(0.0f),
    mScrollValue(0.0f),
    mScrollVelocity(0.0f)
{
    mLastUpdate = 0.0f;
}

UIElementScroll::~UIElementScroll()
{
    if (mContext)
    {
        mContext->RemoveEventListener("mouseup", this);
        mContext->RemoveEventListener("mousemove", this);
    }
}

void UIElementScroll::registerWithlibRocket()
{
    Rocket::Core::Factory::RegisterElementInstancer("scroll", new Rocket::Core::ElementInstancerGeneric<UIElementScroll>());
}

void UIElementScroll::OnChildAdd(Rocket::Core::Element* child)
{
    if (child != this)
        return;

    // We need to attach at the global level to catch the mouse up
    // while the cursor is no longer over us
    mContext = GetContext();
    mContext->AddEventListener("mouseup", this);
    mContext->AddEventListener("mousemove", this);
}

void UIElementScroll::ProcessEvent(Rocket::Core::Event& event)
{
    if (event.GetType() == "mousedown")
    {
        mScrolling = true;
        mMouseY = int(event.GetParameter<float>("mouse_y", 0));
        mMouseDownY = mMouseY;
        mMouseDownTime = Rocket::Core::GetSystemInterface()->GetElapsedTime();
        mScrollVelocity = 0.0f;
        mScrollValue = 0.0f;
    }
    else if (mScrolling)
    {
        if (event.GetType() == "mousemove")
        {
            int mouseY = int(event.GetParameter<float>("mouse_y", 0));
            int delta = mMouseY - mouseY;
            mMouseY = mouseY;

            SetScrollTop(GetScrollTop() + delta);
            event.StopPropagation();
        }
        else if (event.GetType() == "mouseup")
        {
            int mouseY = int(event.GetParameter<float>("mouse_y", 0));
            float deltaTime = Rocket::Core::GetSystemInterface()->GetElapsedTime() - mMouseDownTime;
            float velocity = (mMouseDownY - mouseY) / deltaTime;

            if (deltaTime < 1.0f && Math::AbsoluteValue(velocity) > 200.0f)
            {
                mScrollVelocity = velocity;
            }

            mScrolling = false;
            event.StopPropagation();
        }
    }

    Rocket::Core::Element::ProcessEvent(event);
}

void UIElementScroll::OnUpdate()
{
    float time = Rocket::Core::GetSystemInterface()->GetElapsedTime();
    float deltaTime = time - mLastUpdate;
    mLastUpdate = time;

    if (mScrollVelocity != 0.0f)
    {
        mScrollValue += mScrollVelocity * deltaTime;
        mScrollVelocity *= (1.0f - deltaTime) * 0.9f;

        if (fabs(mScrollVelocity) < 0.01f)
        {
            mScrollVelocity = 0.0f;
        }

        if (fabs(mScrollValue) > 1.0f)
        {
            int scrollAmount = (int)mScrollValue;
            SetScrollTop(GetScrollTop() + scrollAmount);
            mScrollValue -= (float)scrollAmount;
        }
    }
}

}
}