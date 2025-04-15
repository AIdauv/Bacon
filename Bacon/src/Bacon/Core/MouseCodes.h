#pragma once

namespace Bacon
{
    typedef enum class MouseCode : uint16_t
    {
        // From glfw3.h
        Button0 = 0,
        Button1 = 1,
        Button2 = 2,
        Button3 = 3,
        Button4 = 4,
        Button5 = 5,
        Button6 = 6,
        Button7 = 7,

        ButtonLast = Button7,
        ButtonLeft = Button0,
        ButtonRight = Button1,
        ButtonMiddle = Button2
    } Mouse;

    inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode)
    {
        os << static_cast<int32_t>(mouseCode);
        return os;
    }
}

#define BC_MOUSE_BUTTON_0      ::Bacon::Mouse::Button0
#define BC_MOUSE_BUTTON_1      ::Bacon::Mouse::Button1
#define BC_MOUSE_BUTTON_2      ::Bacon::Mouse::Button2
#define BC_MOUSE_BUTTON_3      ::Bacon::Mouse::Button3
#define BC_MOUSE_BUTTON_4      ::Bacon::Mouse::Button4
#define BC_MOUSE_BUTTON_5      ::Bacon::Mouse::Button5
#define BC_MOUSE_BUTTON_6      ::Bacon::Mouse::Button6
#define BC_MOUSE_BUTTON_7      ::Bacon::Mouse::Button7
#define BC_MOUSE_BUTTON_LAST   ::Bacon::Mouse::ButtonLast
#define BC_MOUSE_BUTTON_LEFT   ::Bacon::Mouse::ButtonLeft
#define BC_MOUSE_BUTTON_RIGHT  ::Bacon::Mouse::ButtonRight
#define BC_MOUSE_BUTTON_MIDDLE ::Bacon::Mouse::ButtonMiddle