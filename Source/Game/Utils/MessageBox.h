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
	static bool ShowSimple(EMessageBoxType MessageBoxType, const std::string& Title, const std::string& Message);
};

