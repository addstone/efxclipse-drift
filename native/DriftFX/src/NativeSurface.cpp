/*
 * Copyright (c) 2019 BestSolution.at and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Christoph Caks <ccaks@bestsolution.at> - initial API and implementation
 */
#include "Common.h"
#include "NativeSurface.h"
#include "SharedTexture.h"

#include "prism/PrismBridge.h"

#include <utils/Logger.h>

#include "GLRenderTarget.h"

#include <iostream>
using namespace std;

using namespace driftfx;
using namespace driftfx::gl;

using namespace driftfx::internal;
using namespace driftfx::internal::prism;

NativeSurface::NativeSurface(JNINativeSurface* api) :
	api(api),
	context(nullptr) {
	LogDebug("NativeSurface constructor")

}

NativeSurface::~NativeSurface() {
	LogDebug("NativeSurface" << " destructor")
}

void NativeSurface::Initialize() {
	LogDebug("init GLContext");
	context = PrismBridge::Get()->GetDefaultContext()->CreateSharedContext();
}

void NativeSurface::DisposeSharedTexture(long long id) {
	toDisposeMutex.lock();

	SharedTexture* texture = (SharedTexture*) id;
	toDispose.push_back(texture);

	toDisposeMutex.unlock();
}

void NativeSurface::DisposeSharedTextures() {
	LogDebug("Disposing shared textures");
	toDisposeMutex.lock();
	for (std::vector<SharedTexture*>::iterator it = toDispose.begin(); it != toDispose.end(); ++it) {
		SharedTexture* tex = (*it);
		LogDebug(" - " << tex);
		delete tex;
	}
	toDispose.clear();
	toDisposeMutex.unlock();
}

void NativeSurface::Cleanup() {
	LogDebug("clean textures");
	DisposeSharedTextures();

//	// TODO send some kind of signal to tell FX we are going to dispose our textures
	FrameData* frameData = new FrameData();
	frameData->d3dSharedHandle = 0;
	frameData->ioSurfaceHandle = 0;
	frameData->width=0;
	frameData->height=0;
	api->Present(*frameData);

	delete frameData;
//
//
	// NOTE: since textures know their context and set it current upon deletion
	// we must ensure that all textures from a context are deleted before the context is deleted!

	LogDebug("clean GLContext");
	delete context;
	context = nullptr;

}

GLContext* NativeSurface::GetContext() {
	return context;
}

void NativeSurface::UpdateSize(int width, int height) {
	this->height = height;
	this->width = width;
}

unsigned int NativeSurface::GetWidth() {
	return width;
}

unsigned int NativeSurface::GetHeight() {
	return height;
}

RenderTarget* NativeSurface::Acquire() {
	return Acquire(GetWidth(), GetHeight());
}

RenderTarget* NativeSurface::Acquire(unsigned int width, unsigned int height) {
	
	GLRenderTarget* target = new GLRenderTarget(this, width, height);
	
	return target;
	
	//LogDebug("acquire0");
	//DisposeSharedTextures();



	//PrismBridge* bridge = PrismBridge::Get();
	//// in case the system was destroyed
	//if (bridge == nullptr) {
	//	LogDebug("Could not acquire RenderTarget. Was the system destroyed?");
	//	return nullptr;
	//}

	//SharedTexture* tex = SharedTexture::Create(this, GetContext(), GetFxContext(), width, height);

	//tex->Connect();
	//tex->Lock();

	//return tex;
}

void NativeSurface::Present(RenderTarget* target, PresentationHint hint) {
	if (target == nullptr) {
		LogDebug("Cannot present nullptr; doing nothing.");
		return;
	}

	PrismBridge::Get()->GetShareManager()->Present(target);


	/*SharedTexture* texture = dynamic_cast<SharedTexture*>(target);

	texture->Unlock();
	texture->Disconnect();


	FrameData* frameData = texture->CreateFrameData();
	LogDebug("PRESENT " << frameData->ioSurfaceHandle << " " << frameData->glTextureName);
	frameData->presentationHint = hint;


	api->Present(*frameData);

	delete frameData;*/
}

Context* NativeSurface::GetFxContext() {
	return PrismBridge::Get()->GetFxContext();
}

JNINativeSurface* NativeSurface::GetAPI()
{
	return api;
}
