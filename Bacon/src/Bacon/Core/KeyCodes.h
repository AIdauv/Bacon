#pragma once

namespace Bacon
{
    typedef enum class KeyCode : uint16_t
    {
        // From glfw3.h
        Space = 32,
        Apostrophe = 39, /* ' */
        Comma = 44, /* , */
        Minus = 45, /* - */
        Period = 46, /* . */
        Slash = 47, /* / */

        D0 = 48, /* 0 */
        D1 = 49, /* 1 */
        D2 = 50, /* 2 */
        D3 = 51, /* 3 */
        D4 = 52, /* 4 */
        D5 = 53, /* 5 */
        D6 = 54, /* 6 */
        D7 = 55, /* 7 */
        D8 = 56, /* 8 */
        D9 = 57, /* 9 */

        Semicolon = 59, /* ; */
        Equal = 61, /* = */

        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,

        LeftBracket = 91,  /* [ */
        Backslash = 92,  /* \ */
        RightBracket = 93,  /* ] */
        GraveAccent = 96,  /* ` */

        World1 = 161, /* non-US #1 */
        World2 = 162, /* non-US #2 */

        /* Function keys */
        Escape = 256,
        Enter = 257,
        Tab = 258,
        Backspace = 259,
        Insert = 260,
        Delete = 261,
        Right = 262,
        Left = 263,
        Down = 264,
        Up = 265,
        PageUp = 266,
        PageDown = 267,
        Home = 268,
        End = 269,
        CapsLock = 280,
        ScrollLock = 281,
        NumLock = 282,
        PrintScreen = 283,
        Pause = 284,
        F1 = 290,
        F2 = 291,
        F3 = 292,
        F4 = 293,
        F5 = 294,
        F6 = 295,
        F7 = 296,
        F8 = 297,
        F9 = 298,
        F10 = 299,
        F11 = 300,
        F12 = 301,
        F13 = 302,
        F14 = 303,
        F15 = 304,
        F16 = 305,
        F17 = 306,
        F18 = 307,
        F19 = 308,
        F20 = 309,
        F21 = 310,
        F22 = 311,
        F23 = 312,
        F24 = 313,
        F25 = 314,

        /* Keypad */
        KP0 = 320,
        KP1 = 321,
        KP2 = 322,
        KP3 = 323,
        KP4 = 324,
        KP5 = 325,
        KP6 = 326,
        KP7 = 327,
        KP8 = 328,
        KP9 = 329,
        KPDecimal = 330,
        KPDivide = 331,
        KPMultiply = 332,
        KPSubtract = 333,
        KPAdd = 334,
        KPEnter = 335,
        KPEqual = 336,

        LeftShift = 340,
        LeftControl = 341,
        LeftAlt = 342,
        LeftSuper = 343,
        RightShift = 344,
        RightControl = 345,
        RightAlt = 346,
        RightSuper = 347,
        Menu = 348
    } Key;

    inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
    {
        os << static_cast<int32_t>(keyCode);
        return os;
    }
}

// From glfw3.h
#define BC_KEY_SPACE           ::Bacon::Key::Space
#define BC_KEY_APOSTROPHE      ::Bacon::Key::Apostrophe    /* ' */
#define BC_KEY_COMMA           ::Bacon::Key::Comma         /* , */
#define BC_KEY_MINUS           ::Bacon::Key::Minus         /* - */
#define BC_KEY_PERIOD          ::Bacon::Key::Period        /* . */
#define BC_KEY_SLASH           ::Bacon::Key::Slash         /* / */
#define BC_KEY_0               ::Bacon::Key::D0
#define BC_KEY_1               ::Bacon::Key::D1
#define BC_KEY_2               ::Bacon::Key::D2
#define BC_KEY_3               ::Bacon::Key::D3
#define BC_KEY_4               ::Bacon::Key::D4
#define BC_KEY_5               ::Bacon::Key::D5
#define BC_KEY_6               ::Bacon::Key::D6
#define BC_KEY_7               ::Bacon::Key::D7
#define BC_KEY_8               ::Bacon::Key::D8
#define BC_KEY_9               ::Bacon::Key::D9
#define BC_KEY_SEMICOLON       ::Bacon::Key::Semicolon     /* ; */
#define BC_KEY_EQUAL           ::Bacon::Key::Equal         /* = */
#define BC_KEY_A               ::Bacon::Key::A
#define BC_KEY_B               ::Bacon::Key::B
#define BC_KEY_C               ::Bacon::Key::C
#define BC_KEY_D               ::Bacon::Key::D
#define BC_KEY_E               ::Bacon::Key::E
#define BC_KEY_F               ::Bacon::Key::F
#define BC_KEY_G               ::Bacon::Key::G
#define BC_KEY_H               ::Bacon::Key::H
#define BC_KEY_I               ::Bacon::Key::I
#define BC_KEY_J               ::Bacon::Key::J
#define BC_KEY_K               ::Bacon::Key::K
#define BC_KEY_L               ::Bacon::Key::L
#define BC_KEY_M               ::Bacon::Key::M
#define BC_KEY_N               ::Bacon::Key::N
#define BC_KEY_O               ::Bacon::Key::O
#define BC_KEY_P               ::Bacon::Key::P
#define BC_KEY_Q               ::Bacon::Key::Q
#define BC_KEY_R               ::Bacon::Key::R
#define BC_KEY_S               ::Bacon::Key::S
#define BC_KEY_T               ::Bacon::Key::T
#define BC_KEY_U               ::Bacon::Key::U
#define BC_KEY_V               ::Bacon::Key::V
#define BC_KEY_W               ::Bacon::Key::W
#define BC_KEY_X               ::Bacon::Key::X
#define BC_KEY_Y               ::Bacon::Key::Y
#define BC_KEY_Z               ::Bacon::Key::Z
#define BC_KEY_LEFT_BRACKET    ::Bacon::Key::LeftBracket   /* [ */
#define BC_KEY_BACKSLASH       ::Bacon::Key::Backslash     /* \ */
#define BC_KEY_RIGHT_BRACKET   ::Bacon::Key::RightBracket  /* ] */
#define BC_KEY_GRAVE_ACCENT    ::Bacon::Key::GraveAccent   /* ` */
#define BC_KEY_WORLD_1         ::Bacon::Key::World1        /* non-US #1 */
#define BC_KEY_WORLD_2         ::Bacon::Key::World2        /* non-US #2 */

/* Function keys */
#define BC_KEY_ESCAPE          ::Bacon::Key::Escape
#define BC_KEY_ENTER           ::Bacon::Key::Enter
#define BC_KEY_TAB             ::Bacon::Key::Tab
#define BC_KEY_BACKSPACE       ::Bacon::Key::Backspace
#define BC_KEY_INSERT          ::Bacon::Key::Insert
#define BC_KEY_DELETE          ::Bacon::Key::Delete
#define BC_KEY_RIGHT           ::Bacon::Key::Right
#define BC_KEY_LEFT            ::Bacon::Key::Left
#define BC_KEY_DOWN            ::Bacon::Key::Down
#define BC_KEY_UP              ::Bacon::Key::Up
#define BC_KEY_PAGE_UP         ::Bacon::Key::PageUp
#define BC_KEY_PAGE_DOWN       ::Bacon::Key::PageDown
#define BC_KEY_HOME            ::Bacon::Key::Home
#define BC_KEY_END             ::Bacon::Key::End
#define BC_KEY_CAPS_LOCK       ::Bacon::Key::CapsLock
#define BC_KEY_SCROLL_LOCK     ::Bacon::Key::ScrollLock
#define BC_KEY_NUM_LOCK        ::Bacon::Key::NumLock
#define BC_KEY_PRINT_SCREEN    ::Bacon::Key::PrintScreen
#define BC_KEY_PAUSE           ::Bacon::Key::Pause
#define BC_KEY_F1              ::Bacon::Key::F1
#define BC_KEY_F2              ::Bacon::Key::F2
#define BC_KEY_F3              ::Bacon::Key::F3
#define BC_KEY_F4              ::Bacon::Key::F4
#define BC_KEY_F5              ::Bacon::Key::F5
#define BC_KEY_F6              ::Bacon::Key::F6
#define BC_KEY_F7              ::Bacon::Key::F7
#define BC_KEY_F8              ::Bacon::Key::F8
#define BC_KEY_F9              ::Bacon::Key::F9
#define BC_KEY_F10             ::Bacon::Key::F10
#define BC_KEY_F11             ::Bacon::Key::F11
#define BC_KEY_F12             ::Bacon::Key::F12
#define BC_KEY_F13             ::Bacon::Key::F13
#define BC_KEY_F14             ::Bacon::Key::F14
#define BC_KEY_F15             ::Bacon::Key::F15
#define BC_KEY_F16             ::Bacon::Key::F16
#define BC_KEY_F17             ::Bacon::Key::F17
#define BC_KEY_F18             ::Bacon::Key::F18
#define BC_KEY_F19             ::Bacon::Key::F19
#define BC_KEY_F20             ::Bacon::Key::F20
#define BC_KEY_F21             ::Bacon::Key::F21
#define BC_KEY_F22             ::Bacon::Key::F22
#define BC_KEY_F23             ::Bacon::Key::F23
#define BC_KEY_F24             ::Bacon::Key::F24
#define BC_KEY_F25             ::Bacon::Key::F25

/* Keypad */
#define BC_KEY_KP_0            ::Bacon::Key::KP0
#define BC_KEY_KP_1            ::Bacon::Key::KP1
#define BC_KEY_KP_2            ::Bacon::Key::KP2
#define BC_KEY_KP_3            ::Bacon::Key::KP3
#define BC_KEY_KP_4            ::Bacon::Key::KP4
#define BC_KEY_KP_5            ::Bacon::Key::KP5
#define BC_KEY_KP_6            ::Bacon::Key::KP6
#define BC_KEY_KP_7            ::Bacon::Key::KP7
#define BC_KEY_KP_8            ::Bacon::Key::KP8
#define BC_KEY_KP_9            ::Bacon::Key::KP9
#define BC_KEY_KP_DECIMAL      ::Bacon::Key::KPDecimal
#define BC_KEY_KP_DIVIDE       ::Bacon::Key::KPDivide
#define BC_KEY_KP_MULTIPLY     ::Bacon::Key::KPMultiply
#define BC_KEY_KP_SUBTRACT     ::Bacon::Key::KPSubtract
#define BC_KEY_KP_ADD          ::Bacon::Key::KPAdd
#define BC_KEY_KP_ENTER        ::Bacon::Key::KPEnter
#define BC_KEY_KP_EQUAL        ::Bacon::Key::KPEqual

#define BC_KEY_LEFT_SHIFT      ::Bacon::Key::LeftShift
#define BC_KEY_LEFT_CONTROL    ::Bacon::Key::LeftControl
#define BC_KEY_LEFT_ALT        ::Bacon::Key::LeftAlt
#define BC_KEY_LEFT_SUPER      ::Bacon::Key::LeftSuper
#define BC_KEY_RIGHT_SHIFT     ::Bacon::Key::RightShift
#define BC_KEY_RIGHT_CONTROL   ::Bacon::Key::RightControl
#define BC_KEY_RIGHT_ALT       ::Bacon::Key::RightAlt
#define BC_KEY_RIGHT_SUPER     ::Bacon::Key::RightSuper
#define BC_KEY_MENU            ::Bacon::Key::Menu