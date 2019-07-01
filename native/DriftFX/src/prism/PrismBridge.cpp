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



#include "PrismBridge.h"
#include <utils/Logger.h>
#include <SharedTexture.h>

#include <map>

using namespace driftfx;
using namespace driftfx::gl;

using namespace driftfx::internal;
using namespace driftfx::internal::prism;

PrismBridge* PrismBridge::bridge = nullptr;
std::map<SharedTextureFactoryId, OnTextureCreatedFunc> PrismBridge::handlers;

PrismBridge* PrismBridge::Get() {
	return bridge;
}

PrismBridge::PrismBridge(Context* fxContext) :
		defaultContext(nullptr),
		fxContext(fxContext) {

}

PrismBridge::~PrismBridge() {

}

void PrismBridge::Destroy() {
	delete bridge;
	bridge = nullptr;
}


GLContext* PrismBridge::GetDefaultContext() {
	return defaultContext;
}

Context* PrismBridge::GetFxContext() {
	return fxContext;
}

SharedTextureFactoryId PrismBridge::Register(SharedTextureFactoryId id, OnTextureCreatedFunc func) {
	// WTF? Logger seems here to segvault!!
	auto name = SharedTextureFactory::GetFactoryName(id);
	std::cout << "PrismBridge Registering Shared Texture Handler " << id << " (" << name << ")" << std::endl;
	std::cout << "about to segvault!" << std::endl << std::flush;
	//LogDebug("ABOUT TO SEGVAULT!");
	//LogDebug("Registered prism handler for " << id);
	handlers[id] = func;
	return id;
}

int PrismBridge::OnTextureCreated(Frame* frame, jobject fxTexture) {
	auto mode = frame->GetSurfaceData().transferMode;
	auto handler = handlers[mode];
	if (handler == nullptr) {
		LogError("No handler available for " << mode << "!!");
		return 0;
	}
	else {
		return handler(this, frame, fxTexture);
	}

}