#pragma once

struct SDL_Window;

enum class EMessageBoxType
{
	Information,
	Warning,
	Error
};

class CMessageBox
{
public:
	static int32 Show(EMessageBoxType MessageBoxType, const std::string& Title, const std::string& Message, const std::vector<std::string_view>& Buttons,
		SDL_Window* ParentWindow = nullptr);
	static bool ShowSimple(EMessageBoxType MessageBoxType, const std::string& Title, const std::string& Message, SDL_Window* ParentWindow = nullptr);
};

