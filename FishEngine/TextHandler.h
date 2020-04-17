#pragma once
#include <d2d1.h>
#include <dwrite.h>

class TextHandler
{
private:
	IDWriteFactory* pDWriteFactory_;
	IDWriteTextFormat* pTextFormat_;

public:
	TextHandler();
};