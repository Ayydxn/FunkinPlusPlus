#include "FunkinPCH.h"
#include "MessageBox.h"
#include "Engine/EngineContext.h"

#include <SDL3/SDL_messagebox.h>

namespace
{
	SDL_MessageBoxFlags GetSDLMessageBoxFlags(EMessageBoxType MessageBoxType)
	{
		switch (MessageBoxType)
		{
			case EMessageBoxType::Information: return SDL_MESSAGEBOX_INFORMATION;
			case EMessageBoxType::Warning:     return SDL_MESSAGEBOX_WARNING;
			case EMessageBoxType::Error:       return SDL_MESSAGEBOX_ERROR;
			default:                           return SDL_MESSAGEBOX_INFORMATION;
		}
	}
}

bool CMessageBox::ShowSimple(EMessageBoxType MessageBoxType, const std::string& Title, const std::string& Message)
{
	const auto MessageBoxTitle = "Friday Night Funkin'++ - " + Title;
	const auto Window = CEngineContext::GetInstance().GetNativeWindowHandle().SDLWindow;

	return SDL_ShowSimpleMessageBox(GetSDLMessageBoxFlags(MessageBoxType), MessageBoxTitle.c_str(), Message.c_str(), Window);
}
