#include "FunkinPCH.h"
#include "CoreInputTypes.h"

void FKeys::InitializeKeys()
{
    /* Printable */
    AddKeyToMap(Space);
    AddKeyToMap(Apostrophe);
    AddKeyToMap(Comma);
    AddKeyToMap(Minus);
    AddKeyToMap(Period);
    AddKeyToMap(Slash);
    AddKeyToMap(Zero);
    AddKeyToMap(One);
    AddKeyToMap(Two);
    AddKeyToMap(Three);
    AddKeyToMap(Four);
    AddKeyToMap(Five);
    AddKeyToMap(Six);
    AddKeyToMap(Seven);
    AddKeyToMap(Eight);
    AddKeyToMap(Nine);
    AddKeyToMap(Semicolon);
    AddKeyToMap(Equal);
    AddKeyToMap(A);
    AddKeyToMap(B);
    AddKeyToMap(C);
    AddKeyToMap(D);
    AddKeyToMap(E);
    AddKeyToMap(F);
    AddKeyToMap(G);
    AddKeyToMap(H);
    AddKeyToMap(I);
    AddKeyToMap(J);
    AddKeyToMap(K);
    AddKeyToMap(L);
    AddKeyToMap(M);
    AddKeyToMap(N);
    AddKeyToMap(O);
    AddKeyToMap(P);
    AddKeyToMap(Q);
    AddKeyToMap(R);
    AddKeyToMap(S);
    AddKeyToMap(T);
    AddKeyToMap(U);
    AddKeyToMap(V);
    AddKeyToMap(W);
    AddKeyToMap(X);
    AddKeyToMap(Y);
    AddKeyToMap(Z);
    AddKeyToMap(LeftBracket);
    AddKeyToMap(Backslash);
    AddKeyToMap(RightBracket);
    AddKeyToMap(GraveAccent);

    /* Navigation / function */
    AddKeyToMap(Escape);
    AddKeyToMap(Enter);
    AddKeyToMap(Tab);
    AddKeyToMap(Backspace);
    AddKeyToMap(Insert);
    AddKeyToMap(Delete);
    AddKeyToMap(RightArrow);
    AddKeyToMap(LeftArrow);
    AddKeyToMap(DownArrow);
    AddKeyToMap(UpArrow);
    AddKeyToMap(PageUp);
    AddKeyToMap(PageDown);
    AddKeyToMap(Home);
    AddKeyToMap(End);
    AddKeyToMap(CapsLock);
    AddKeyToMap(ScrollLock);
    AddKeyToMap(NumberLock);
    AddKeyToMap(PrintScreen);
    AddKeyToMap(Pause);
    AddKeyToMap(F1);
    AddKeyToMap(F2);
    AddKeyToMap(F3);
    AddKeyToMap(F4);
    AddKeyToMap(F5);
    AddKeyToMap(F6);
    AddKeyToMap(F7);
    AddKeyToMap(F8);
    AddKeyToMap(F9);
    AddKeyToMap(F10);
    AddKeyToMap(F11);
    AddKeyToMap(F12);
    AddKeyToMap(F13);
    AddKeyToMap(F14);
    AddKeyToMap(F15);
    AddKeyToMap(F16);
    AddKeyToMap(F17);
    AddKeyToMap(F18);
    AddKeyToMap(F19);
    AddKeyToMap(F20);
    AddKeyToMap(F21);
    AddKeyToMap(F22);
    AddKeyToMap(F23);
    AddKeyToMap(F24);

    /* Numpad */
    AddKeyToMap(NumPad0);
    AddKeyToMap(NumPad1);
    AddKeyToMap(NumPad2);
    AddKeyToMap(NumPad3);
    AddKeyToMap(NumPad4);
    AddKeyToMap(NumPad5);
    AddKeyToMap(NumPad6);
    AddKeyToMap(NumPad7);
    AddKeyToMap(NumPad8);
    AddKeyToMap(NumPad9);
    AddKeyToMap(NumPadDecimal);
    AddKeyToMap(NumPadDivide);
    AddKeyToMap(NumPadMultiply);
    AddKeyToMap(NumPadSubtract);
    AddKeyToMap(NumPadAdd);
    AddKeyToMap(NumPadEnter);

    /* Modifiers */
    AddKeyToMap(LeftShift);
    AddKeyToMap(LeftControl);
    AddKeyToMap(LeftAlt);
    AddKeyToMap(LeftSuper);
    AddKeyToMap(RightShift);
    AddKeyToMap(RightControl);
    AddKeyToMap(RightAlt);
    AddKeyToMap(RightSuper);

    /* Mouse buttons -- share the same code-space/map as keyboard keys (see CoreInputTypes.h) */
    AddKeyToMap(LeftMouseButton);
    AddKeyToMap(RightMouseButton);
    AddKeyToMap(MiddleMouseButton);
    AddKeyToMap(ThumbMouseButtonOne);
    AddKeyToMap(ThumbMouseButtonTwo);
}

void FKeys::AddKeyToMap(const FKey& Key)
{
    FKey::m_KeyCodeToKeyMap[Key.GetKeyCode()] = Key;
}