#include "TextHandler.h"

TextHandler::TextHandler()
{
    HRESULT hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        &pD2DFactory_
    );
}
