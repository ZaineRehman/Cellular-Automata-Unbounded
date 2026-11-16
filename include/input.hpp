#pragma once

#include <SDL3/SDL.h>

namespace Inputs {
	void handleMouseEvents(float& mouseX, float& mouseY, uint32_t& mouseState);

	void handleKeyEvents(SDL_Event& event);
}