#include "input.hpp"

//#include <SDL3/SDL.h>
#include <cmath>
#include <algorithm>

#include "settings.hpp"
#include "util.hpp"
#include "sim.hpp"


void Inputs::handleMouseEvents(float& mouseX, float& mouseY, uint32_t& mouseState) {
	if (mouseState & SDL_BUTTON_LMASK || mouseState & SDL_BUTTON_RMASK) {
		if (mouseX >= 0 && mouseX < SimState::screenWidth && mouseY >= 0 && mouseY < SimState::screenHeight) {
			int mouseRelX = mouseX / SimState::pixelsPerCell;
			int mouseRelY = mouseY / SimState::pixelsPerCell;

			// radius

			if(SimState::mouseRadius == 0) {
				for (size_t py = 0; py < SimState::pixelsPerCell; ++py) {
					for (size_t px = 0; px < SimState::pixelsPerCell; ++px) {
						int newState = 0;
						if (SimState::mouseFillMethod == 0) newState = SimState::rules.size()-1;
						else if (SimState::mouseFillMethod == 1) newState = rand() % (SimState::rules.size()-1) + 1;
						else if (SimState::mouseFillMethod == 2) newState = rand() % SimState::rules.size();

						SimState::cellMap[mouseRelY*SimState::cellsWidth + mouseRelX].state = newState;
					}
				}
			}

			// these are poorly named :)
			for (int minRadY = -SimState::mouseRadius; minRadY <= SimState::mouseRadius; ++minRadY) {
				for (int minRadX = -SimState::mouseRadius; minRadX <= SimState::mouseRadius; ++minRadX) {
					if (
						// no static_cast, sue me
						(mouseRelY+minRadY) >= 0 && (mouseRelY+minRadY) < (int)SimState::cellsHeight 
						&& (mouseRelX+minRadX) >= 0 && (mouseRelX+minRadX) < (int)SimState::cellsWidth  // screen bounds
						&& sqrt(pow(minRadX,2) + pow(minRadY,2)) <= (float)SimState::mouseRadius        // radius bounds
					) {
						// repeated code
						for (size_t py = 0; py < SimState::pixelsPerCell; ++py) {
							for (size_t px = 0; px < SimState::pixelsPerCell; ++px) {
								int newState = 0;
								if (mouseState & SDL_BUTTON_RMASK);
								else if (SimState::mouseFillMethod == 0) newState = SimState::manualFillState;
								else if (SimState::mouseFillMethod == 1) newState = rand() % (SimState::rules.size()-1) + 1;
								else if (SimState::mouseFillMethod == 2) newState = rand() % SimState::rules.size();

								SimState::cellMap[(mouseRelY+minRadY)*SimState::cellsWidth + (mouseRelX+minRadX)].state = newState;
							}
						}
					}
				}
			}  // yikes, you could get lost in these
		}      // (used to be more, too)
	}
}


void Inputs::handleKeyEvents(SDL_Event& event) {
	// desire to change indexed rule
	float edit_desire = 0;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_EVENT_QUIT) {
			SimState::presentRules = false;
		}
		else if (event.type == SDL_EVENT_KEY_DOWN) {
			switch (event.key.key) {

				case SDLK_ESCAPE: 
					SimState::running = false;
					break;
				
				case SDLK_UP: 
					clamp_rollover(--SimState::edit_stateIndex, 0, static_cast<int>(SimState::rules.size()-1));
					SimState::presentRules = true;
					break;
				case SDLK_DOWN: 
					clamp_rollover(++SimState::edit_stateIndex, 0, static_cast<int>(SimState::rules.size()-1));
					SimState::presentRules = true;
					break;
				case SDLK_LEFT: {
					//                             TODO magic number  v
					int max = static_cast<int>(SimState::rules.size()+5);
					if (SimState::simMode == 1) max -= 2;

					clamp_rollover(--SimState::edit_ruleIndex, 0, max);
					SimState::presentRules = true;
					break;
				}
				case SDLK_RIGHT: {
					int max = static_cast<int>(SimState::rules.size()+5);
					if (SimState::simMode == 1) max -= 2;

					clamp_rollover(++SimState::edit_ruleIndex, 0, max);
					SimState::presentRules = true;
					break;
				}
				
				case SDLK_MINUS:  // decrement rule value                     v TODO magic number (kinda)
					if (SimState::simMode == 0 || SimState::edit_ruleIndex >= 2) edit_desire = -1.0f;
					break;
				case SDLK_EQUALS:  // increment rule value
					if (SimState::simMode == 0 || SimState::edit_ruleIndex >= 2) edit_desire = 1.0f;
					break;
				
				case SDLK_KP_PLUS: 
					if (SimState::simMode == 0 || SimState::edit_ruleIndex >= 2) edit_desire = .1f;
					break;
				case SDLK_KP_MINUS: 
					if (SimState::simMode == 0 || SimState::edit_ruleIndex >= 2) edit_desire = -.1f;
					break;
				
				case SDLK_0: 
					if (SimState::simMode == 1) edit_desire = 0.1f;  // turns into 0 when casted
					break;
				case SDLK_1: 
					if (SimState::simMode == 1) edit_desire = 1.0f;
					break;
				case SDLK_2: 
					if (SimState::simMode == 1) edit_desire = 2.0f;
					break;
				case SDLK_3: 
					if (SimState::simMode == 1) edit_desire = 3.0f;
					break;
				case SDLK_4: 
					if (SimState::simMode == 1) edit_desire = 4.0f;
					break;
				case SDLK_5: 
					if (SimState::simMode == 1) edit_desire = 5.0f;
					break;
				case SDLK_6: 
					if (SimState::simMode == 1) edit_desire = 6.0f;
					break;
				case SDLK_7: 
					if (SimState::simMode == 1) edit_desire = 7.0f;
					break;
				case SDLK_8: 
					if (SimState::simMode == 1) edit_desire = 8.0f;
					break;
				case SDLK_9: 
					if (SimState::simMode == 1) edit_desire = 9.0f;
					break;
				
				case SDLK_Q:  // increase red
					SimState::stateColors[SimState::edit_stateIndex].r = static_cast<uint8_t>(min((int)SimState::stateColors[SimState::edit_stateIndex].r + 16, 255));
					SimState::presentRules = true;
					break;
				case SDLK_W:  // increase green
					SimState::stateColors[SimState::edit_stateIndex].g = static_cast<uint8_t>(min((int)SimState::stateColors[SimState::edit_stateIndex].g + 16, 255));
					SimState::presentRules = true;
					break;
				case SDLK_E:  // increase blue
					SimState::stateColors[SimState::edit_stateIndex].b = static_cast<uint8_t>(min((int)SimState::stateColors[SimState::edit_stateIndex].b + 16, 255));
					SimState::presentRules = true;
					break;
				case SDLK_A:  // decrease red
					SimState::stateColors[SimState::edit_stateIndex].r = static_cast<uint8_t>(max((int)SimState::stateColors[SimState::edit_stateIndex].r - 16, 0));
					SimState::presentRules = true;
					break;
				case SDLK_S:  // decrease green
					SimState::stateColors[SimState::edit_stateIndex].g = static_cast<uint8_t>(max((int)SimState::stateColors[SimState::edit_stateIndex].g - 16, 0));
					SimState::presentRules = true;
					break;
				case SDLK_D:  // decrease blue
					SimState::stateColors[SimState::edit_stateIndex].b = static_cast<uint8_t>(max((int)SimState::stateColors[SimState::edit_stateIndex].b - 16, 0));
					SimState::presentRules = true;
					break;
				
				case SDLK_SPACE:  // pause
					SimState::paused = SimState::paused == -1 ? 0 : -1;
					break;
				case SDLK_L:  // advance one generation
					if (SimState::paused == 0) SimState::paused = 1;
					break;
				case SDLK_C:  // clear
					for (size_t i = 0; i < SimState::cellMap.size(); ++i) {
						SimState::cellMap[i].state = 0;
					}
					break;
				
				case SDLK_TAB: 
					SimState::simMode++;
					clamp_rollover(SimState::simMode, 0, 1);
					SimState::edit_ruleIndex = 0;
					SimState::presentRules = true;
					break;

				case SDLK_O:  // decease mouse radius
					SimState::mouseRadius--;
					if (SimState::mouseRadius == -1) SimState::mouseRadius = 0;
					SimState::presentRules = true;
					break;
				case SDLK_P:  // increase mouse radius
					SimState::mouseRadius++;
					SimState::presentRules = true;
					break;
				case SDLK_U:  // decrease mouse fill method
					SimState::mouseFillMethod--;
					clamp(SimState::mouseFillMethod, 0, 2);
					SimState::presentRules = true;
					break;
				case SDLK_I:  // increase mouse fill method
					SimState::mouseFillMethod++;
					clamp(SimState::mouseFillMethod, 0, 2);  // TODO magic number
					SimState::presentRules = true;
					break;
				case SDLK_H:  // decrease manual fill state
					SimState::manualFillState--;
					clamp(SimState::manualFillState, 0, (int)SimState::rules.size()-1);
					SimState::presentRules = true;
					break;
				case SDLK_J:  // increase manual fill state
					SimState::manualFillState++;
					clamp(SimState::manualFillState, 0, (int)SimState::rules.size()-1);
					SimState::presentRules = true;
					break;
				
				case SDLK_LEFTBRACKET:  // decrease frame delay
					SimState::maxFrameDelay -= 2;
					clamp(SimState::maxFrameDelay, 0, 256);
					break;
				case SDLK_RIGHTBRACKET:  // increase frame delay
					SimState::maxFrameDelay += 2;
					clamp(SimState::maxFrameDelay, 0, 256);
					break;
				
				case SDLK_SLASH:  // fill with random
					for (size_t x = 0; x < SimState::cellsWidth; ++x) {
						for (size_t y = 0; y < SimState::cellsHeight; ++y) {
							SimState::cellMap[y*SimState::cellsWidth + x] = Cell{rand() % SimState::rules.size()};
						}
					}
					break;
				
				case SDLK_COMMA:  // decrement cell pixel size
					clamp(--SimState::pixelsPerCell, (size_t)1, (size_t)16);
					SimState::cellsWidth = SimState::screenWidth/SimState::pixelsPerCell;
					SimState::cellsHeight = SimState::screenHeight/SimState::pixelsPerCell;

					SimState::cellMap.clear();
					SimState::cellMap.resize(SimState::cellsWidth*SimState::cellsHeight);
					SimState::cellMap.assign(SimState::cellsWidth*SimState::cellsHeight, Cell{0});

					SimState::newMap.clear();
					SimState::newMap.resize(SimState::cellsWidth*SimState::cellsHeight);
					SimState::newMap.assign(SimState::cellsWidth*SimState::cellsHeight, Cell{0});

					// black screen
					for (size_t s = 0; s < SimState::screen.size(); ++s) SimState::screen[s] = RGBA_t{0,0,0,255};

					SimState::presentRules = true;

					break;
				case SDLK_PERIOD:  // increment cell pixel size
					clamp(++SimState::pixelsPerCell, (size_t)1, (size_t)16);
					SimState::cellsWidth = SimState::screenWidth/SimState::pixelsPerCell;
					SimState::cellsHeight = SimState::screenHeight/SimState::pixelsPerCell;

					SimState::cellMap.clear();
					SimState::cellMap.resize(SimState::cellsWidth*SimState::cellsHeight);
					SimState::cellMap.assign(SimState::cellsWidth*SimState::cellsHeight, Cell{0});

					SimState::newMap.clear();
					SimState::newMap.resize(SimState::cellsWidth*SimState::cellsHeight);
					SimState::newMap.assign(SimState::cellsWidth*SimState::cellsHeight, Cell{0});

					// black screen
					for (size_t s = 0; s < SimState::screen.size(); ++s) SimState::screen[s] = RGBA_t{0,0,0,255};

					SimState::presentRules = true;

					break;
				
				case SDLK_BACKSPACE:  // delete topmost state
					if (SimState::rules.size() != 1) {
						SimState::rules.pop_back();
						SimState::stateColors.pop_back();

						// remove reference from surrounding
						for (Rules& rule : SimState::rules) {
							rule.surrounding.pop_back();
							rule.sv_surrounding.pop_back();
						}

						// destroy all of those cells
						for (Cell& cell : SimState::cellMap) {
							if (cell.state == static_cast<int>(SimState::rules.size())) cell.state = 0;
						}

						// make sure state index isnt out of bounds
						if (SimState::edit_stateIndex >= static_cast<int>(SimState::rules.size())) {
							SimState::edit_stateIndex = SimState::rules.size()-1;
						}

						SimState::presentRules = true;
					}

					break;
				
				case SDLK_RETURN:  // add topmost state
					Rules defaultRuleset {2.0f, 4.0f, 3.0f, 3.0f, 0.0f, -1, {0.0f}};
					LOOP(SimState::rules.size()-1) {
						defaultRuleset.surrounding.push_back(1.0f);
						defaultRuleset.sv_surrounding.push_back(1);
					}

					// default values for set-value mode
					defaultRuleset.sv_birthValues = {3};
					defaultRuleset.sv_survivalValues = {2,3};

					SimState::rules.push_back(defaultRuleset);
					SimState::stateColors.push_back(RGBA_t{255,255,255,255});

					// add surrounding SimState::rules to each state
					for (Rules& rule : SimState::rules) {
						rule.surrounding.push_back(1.0f);
						rule.sv_surrounding.push_back(1);
					}

					SimState::presentRules = true;
					break;
			}
		}
	}

	// edit SimState::rules
	if (edit_desire != 0.0f) {
		Rules& ruleToEdit = SimState::rules[SimState::edit_stateIndex];

		if (SimState::simMode == 0) {
			if (SimState::edit_ruleIndex == 0) {
				// minToSurvive
				ruleToEdit.minToSurvive += edit_desire;
				clamp(ruleToEdit.minToSurvive, SimState::minRuleVal, SimState::maxRuleVal);
				// std::round(value * 10.0f) / 10.0f;
			} else if (SimState::edit_ruleIndex == 1) {
				// minToDie
				ruleToEdit.minToDie += edit_desire;
				clamp(ruleToEdit.minToDie, SimState::minRuleVal, SimState::maxRuleVal);
			} else if (SimState::edit_ruleIndex == 2) {
				// minForReproduction
				ruleToEdit.minForReproduction += edit_desire;
				clamp(ruleToEdit.minForReproduction, SimState::minRuleVal, SimState::maxRuleVal);
			} else if (SimState::edit_ruleIndex == 3) {
				// maxForReproduction
				ruleToEdit.maxForReproduction += edit_desire;
				clamp(ruleToEdit.maxForReproduction, SimState::minRuleVal, SimState::maxRuleVal);
			} else if (SimState::edit_ruleIndex == 4) {
				// outOfBounds
				ruleToEdit.outOfBounds += edit_desire;
				clamp(ruleToEdit.outOfBounds, SimState::minRuleVal, SimState::maxRuleVal);
			} else if (SimState::edit_ruleIndex == 5) {
				// explititJump
				ruleToEdit.explicitJump += edit_desire;
				clamp(ruleToEdit.explicitJump, 0, (int)SimState::rules.size()-1);
			} else {
				// surrounding rules
				ruleToEdit.surrounding[SimState::edit_ruleIndex-6] += edit_desire;
				clamp(ruleToEdit.surrounding[SimState::edit_ruleIndex-6], SimState::minRuleVal, SimState::maxRuleVal);
			}
		} else if (SimState::simMode == 1) {
			if (SimState::edit_ruleIndex == 0) {
				// birth values
				if (isIn(ruleToEdit.sv_birthValues, (int)edit_desire)) {
					std::erase(ruleToEdit.sv_birthValues, (int)edit_desire);
				} else {
					ruleToEdit.sv_birthValues.push_back((int)edit_desire);
					std::sort(ruleToEdit.sv_birthValues.begin(), ruleToEdit.sv_birthValues.end());
				}
			} else if (SimState::edit_ruleIndex == 1) {
				// survival values
				if (isIn(ruleToEdit.sv_survivalValues, (int)edit_desire)) {
					std::erase(ruleToEdit.sv_survivalValues, (int)edit_desire);
				} else {
					ruleToEdit.sv_survivalValues.push_back((int)edit_desire);
					std::sort(ruleToEdit.sv_survivalValues.begin(), ruleToEdit.sv_survivalValues.end());
				}
			} else if (SimState::edit_ruleIndex == 2) {
				// bounds value
				ruleToEdit.outOfBounds += edit_desire;
				clamp(ruleToEdit.outOfBounds, SimState::minRuleVal, SimState::maxRuleVal);
			} else if (SimState::edit_ruleIndex == 3) {
				// explititJump
				ruleToEdit.explicitJump += edit_desire;
				clamp(ruleToEdit.explicitJump, 0, (int)SimState::rules.size()-1);
			} else if (edit_desire <= 1.0f) {
				ruleToEdit.sv_surrounding[SimState::edit_ruleIndex-4] += (int)edit_desire;
				clamp(ruleToEdit.sv_surrounding[SimState::edit_ruleIndex-4], (int)SimState::minRuleVal, (int)SimState::maxRuleVal);
			}
		}

		SimState::presentRules = true;
	}
}