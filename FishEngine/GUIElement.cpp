#include "GUIElement.h"
GUIElement::GUIElement(DxHandler* dxHandler)
	:dxHandler(dxHandler)
{								      //X  Y  Z   R	 G  B  A, U, V  nX nY nZ
	mesh = new Mesh(this->dxHandler->devicePtr);
	mesh->vertices.push_back(Vertex{ -1,  1, 0.1f,  1, 1, 1, 1, 0, 0, 0, 0, -1 });
	mesh->vertices.push_back(Vertex{ 1, -1, 0.1f,    1, 1, 1, 1, 1, 1, 0, 0, -1 });
	mesh->vertices.push_back(Vertex{ -1,  -1, 0.1f,  1, 1, 1, 1, 0, 1, 0, 0, -1 });

	mesh->vertices.push_back(Vertex{ -1,  1, 0.1f,  1, 1, 1, 1, 0, 0, 0, 0, -1 });
	mesh->vertices.push_back(Vertex{ 1,  1, 0.1f,   1, 1, 1, 1, 1, 0, 0, 0, -1 });
	mesh->vertices.push_back(Vertex{ 1,  -1, 0.1f, 1, 1, 1, 1, 1, 1, 0, 0, -1 });

	mesh->createVertexBuffer();
}

GUIElement::GUIElement()
{
}

void GUIElement::setDxHandler(DxHandler* dxHandler)
{
	this->dxHandler = dxHandler;
}

void GUIElement::draw(Camera& camera)
{
	PS_CONSTANT_GUI_BUFFER guiBuff;
	guiBuff.selected = false;
	guiBuff.isButton = false;
	DxHandler::contextPtr->UpdateSubresource(DxHandler::guiBuffer, 0, NULL, &guiBuff, 0, 0);

	this->dxHandler->draw(mesh, camera, false, nullptr);
}
