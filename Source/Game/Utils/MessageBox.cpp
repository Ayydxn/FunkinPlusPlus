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

int32 CMessageBox::Show(EMessageBoxType MessageBoxType, const std::string& Title, const std::string& Message, const std::vector<std::string_view>& Buttons,
	SDL_Window* ParentWindow)
{
	const auto MessageBoxTitle = "Friday Night Funkin'++ - " + Title;
	
	std::vector<std::string> ButtonTexts;
	ButtonTexts.reserve(Buttons.size());
	
	for (const auto& Button : Buttons)
		ButtonTexts.emplace_back(Button);

	std::vector<SDL_MessageBoxButtonData> SDLButtons;
	SDLButtons.reserve(Buttons.size());

	for (size_t i = 0; i < Buttons.size(); ++i)
	{
		SDL_MessageBoxButtonData MessageBoxButtonData;
		MessageBoxButtonData.flags = 0;
		MessageBoxButtonData.buttonID = static_cast<int32>(i);
		MessageBoxButtonData.text = ButtonTexts[i].data();
		
		SDLButtons.push_back(MessageBoxButtonData);
	}

	SDL_MessageBoxData MessageBoxData;
	MessageBoxData.flags = GetSDLMessageBoxFlags(MessageBoxType);
	MessageBoxData.window = ParentWindow;
	MessageBoxData.title = MessageBoxTitle.c_str();
	MessageBoxData.message = Message.c_str();
	MessageBoxData.numbuttons = static_cast<int>(SDLButtons.size());
	MessageBoxData.buttons = SDLButtons.data();
	MessageBoxData.colorScheme = nullptr;

	int32 ButtonID = -1;
	verifyFunkinf(SDL_ShowMessageBox(&MessageBoxData, &ButtonID), "Failed to show message box: {}", SDL_GetError())

	return ButtonID;
}

bool CMessageBox::ShowSimple(EMessageBoxType MessageBoxType, const std::string& Title, const std::string& Message, SDL_Window* ParentWindow)
{
	const auto MessageBoxTitle = "Friday Night Funkin'++ - " + Title;

	return SDL_ShowSimpleMessageBox(GetSDLMessageBoxFlags(MessageBoxType), MessageBoxTitle.c_str(), Message.c_str(), ParentWindow);
}
