#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <string>
#include <cmath>
//#include <unordered_map>
//#include <unordered_set>

#include <SDL3/SDL.h>

#include "settings.hpp"
#include "render.hpp"
#include "cell.hpp"
#include "util.hpp"
#include "sim.hpp"


int main() {
	if (0) {
		outputCellBias(Neighborhood::moore);
		outputCellBias(Neighborhood::falloff2);
		outputCellBias(Neighborhood::falloff2_less);
		outputCellBias(Neighborhood::vonNeumann);
		outputCellBias(Neighborhood::carved);

		return 0;
	}

	srand(time(0));

	//std::cout << std::setprecision(1);

	Renderer renderer {SimState::screenWidth, SimState::screenHeight, "Cellular Automata Unbounded"};

	std::string 
		ansi_reset = "\033[0m", 
		ansi_red = "\033[91m", 
		ansi_blue = "\033[94m", 
		ansi_green = "\033[92m", 
		ansi_active = "\033[1;38:5:46m", 
		ansi_inactive = "\033[1m";

	// get color for parameters when outputting
	auto getPresentColor = [&](int stateIndex, int ruleIndex) {
		if (stateIndex == SimState::edit_stateIndex && ruleIndex == SimState::edit_ruleIndex) return ansi_active;
		return ansi_inactive;
	};


	// fill with random
	for (size_t x = 0; x < SimState::cellsWidth; ++x) {
		for (size_t y = 0; y < SimState::cellsHeight; ++y) {
			SimState::cellMap[y*SimState::cellsWidth + x] = Cell{rand() % 2};
		}
	}
	

	while (SimState::running) {
		std::chrono::time_point<std::chrono::steady_clock> timeStart = std::chrono::steady_clock::now();

		if (SimState::presentRules) {
			system("cls");

			// output SimState::rules

			std::cout
				<< "[Arrows] to select rule, [+],[-] to change value\nKeypad [+],[-] changes by 0.1\n[Q][W][E],[A][S][D] to change color\n\n"
				<< "          SURVIVAL    REPRODUCE  BOUNDS   SURROUNDINGS" << '\n'
				<< "          min  lim    min  max                         " << '\n';
			for (size_t ri = 0; ri < SimState::rules.size(); ++ri) {
				std::cout 
					<< "State " << ri << ": "
					<< getPresentColor(ri, 0) << padString(round(SimState::rules[ri].minToSurvive, 1), 4) << ansi_reset << ","
					<< getPresentColor(ri, 1) << padString(round(SimState::rules[ri].minToDie, 1), 4) << ansi_reset << " ~ "
					<< getPresentColor(ri, 2) << padString(round(SimState::rules[ri].minForReproduction, 1), 4) << ansi_reset << ","
					<< getPresentColor(ri, 3) << padString(round(SimState::rules[ri].maxForReproduction, 1), 4) << ansi_reset << " ~ "
					<< getPresentColor(ri, 4) << padString(round(SimState::rules[ri].outOfBounds, 1), 4) << ansi_reset << " {";
				for (size_t si = 0; si < SimState::rules[ri].surrounding.size(); ++si) {
					std::cout 
						<< getPresentColor(ri, 5 + si) 
						<< padString(round(SimState::rules[ri].surrounding[si], 1), 4) 
						<< ansi_reset << (si == SimState::rules[ri].surrounding.size()-1 ? "}  [": ",");
				}
				// color
				std::cout 
					<< ansi_red   << padString(SimState::stateColors[ri].r, 3) << ansi_reset << " "
					<< ansi_green << padString(SimState::stateColors[ri].g, 3) << ansi_reset << " "
					<< ansi_blue  << padString(SimState::stateColors[ri].b, 3) << ansi_reset << "]\n";
			}
			std::cout
				<< "\n[O],[P] Mouse radius: " << SimState::mouseRadius << '\n'
				<<   "[U],[I] Mouse fill method: " << SimState::mouseFillMethod << '\n'
				<< "\n[<],[>] Pixels per cell: " << SimState::pixelsPerCell << '\n';

			/* T.ODO look into why this crashes
			for (size_t ri = 0; ri < SimState::rules.size(); ++ri) {
				std::cout
					<< "State " << ri << ":"
					<< std::format("{:2}", SimState::rules[ri].minToSurvive) << ","
					<< std::format("{:2}", SimState::rules[ri].minToDie) << ","
					<< std::format("{:2}", SimState::rules[ri].numForReproduction) << " {";
				for (size_t si = 0; si < SimState::rules[ri].surrounding.size(); ++si) {
					std::cout << std::format("{:2}", SimState::rules[ri].surrounding[si]) << (si == SimState::rules[ri].surrounding.size()-1 ? "}\n": ",");
				}
			}*/

			SimState::presentRules = false;
		}

		// poll events
		// key buffer, not key states
		SDL_Event event;
		// desire to change indexed rule
		float edit_desire = 0;
		// mouse position
		float mouseX, mouseY;

		// we gonna do mouse first lol
		uint32_t mouseState = SDL_GetMouseState(&mouseX, &mouseY);
		if (mouseState & SDL_BUTTON_LMASK || mouseState & SDL_BUTTON_RMASK) {
			if (SimState::paused == 0 || 1) {  // no longer care about paused
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
										else if (SimState::mouseFillMethod == 0) newState = SimState::rules.size()-1;
										else if (SimState::mouseFillMethod == 1) newState = rand() % (SimState::rules.size()-1) + 1;
										else if (SimState::mouseFillMethod == 2) newState = rand() % SimState::rules.size();

										SimState::cellMap[(mouseRelY+minRadY)*SimState::cellsWidth + (mouseRelX+minRadX)].state = newState;
									}
								}
							}
						}
					}
				}  // yikes, you could get lost in these
			}
		}
		
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				SimState::running = false;
			}
			else if (event.type == SDL_EVENT_KEY_DOWN) {
				switch (event.key.key) {

					case SDLK_ESCAPE: 
						SimState::running = false;
						break;
					
					case SDLK_UP: 
						clamp_rollover(--SimState::edit_stateIndex, 0, static_cast<int>(SimState::rules.size()-1));
						SimState::running = true;
						break;
					case SDLK_DOWN: 
						clamp_rollover(++SimState::edit_stateIndex, 0, static_cast<int>(SimState::rules.size()-1));
						SimState::running = true;
						break;
					case SDLK_LEFT: 
						clamp_rollover(--SimState::edit_ruleIndex, 0, static_cast<int>(SimState::rules.size()+4));
						SimState::running = true;
						break;
					case SDLK_RIGHT: 
						clamp_rollover(++SimState::edit_ruleIndex, 0, static_cast<int>(SimState::rules.size()+4));
						SimState::running = true;
						break;
					
					case SDLK_MINUS:  // decrement rule value
						edit_desire = -1;
						break;
					case SDLK_EQUALS:  // increment rule value
						edit_desire = 1;
						break;
					
					case SDLK_KP_PLUS: 
						edit_desire = .1;
						break;
					case SDLK_KP_MINUS: 
						edit_desire = -.1;
						break;
					
					case SDLK_Q:  // increase red
						SimState::stateColors[SimState::edit_stateIndex].r = static_cast<uint8_t>(min((int)SimState::stateColors[SimState::edit_stateIndex].r + 16, 255));
						SimState::running = true;
						break;
					case SDLK_W:  // increase green
						SimState::stateColors[SimState::edit_stateIndex].g = static_cast<uint8_t>(min((int)SimState::stateColors[SimState::edit_stateIndex].g + 16, 255));
						SimState::running = true;
						break;
					case SDLK_E:  // increase blue
						SimState::stateColors[SimState::edit_stateIndex].b = static_cast<uint8_t>(min((int)SimState::stateColors[SimState::edit_stateIndex].b + 16, 255));
						SimState::running = true;
						break;
					case SDLK_A:  // decrease red
						SimState::stateColors[SimState::edit_stateIndex].r = static_cast<uint8_t>(max((int)SimState::stateColors[SimState::edit_stateIndex].r - 16, 0));
						SimState::running = true;
						break;
					case SDLK_S:  // decrease green
						SimState::stateColors[SimState::edit_stateIndex].g = static_cast<uint8_t>(max((int)SimState::stateColors[SimState::edit_stateIndex].g - 16, 0));
						SimState::running = true;
						break;
					case SDLK_D:  // decrease blue
						SimState::stateColors[SimState::edit_stateIndex].b = static_cast<uint8_t>(max((int)SimState::stateColors[SimState::edit_stateIndex].b - 16, 0));
						SimState::running = true;
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

					case SDLK_O:  // decease mouse radius
						SimState::mouseRadius--;
						if (SimState::mouseRadius == -1) SimState::mouseRadius = 0;
						SimState::running = true;
						break;
					case SDLK_P:  // increase mouse radius
						SimState::mouseRadius++;
						SimState::running = true;
						break;
					case SDLK_U:  // decrease mouse fill method
						SimState::mouseFillMethod--;
						clamp(SimState::mouseFillMethod, 0, 2);
						SimState::running = true;
						break;
					case SDLK_I:  // increase mouse fill method
						SimState::mouseFillMethod++;
						clamp(SimState::mouseFillMethod, 0, 2);
						SimState::running = true;
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

						SimState::running = true;

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

						SimState::running = true;

						break;
					
					case SDLK_BACKSPACE:  // delete topmost state
						if (SimState::rules.size() != 1) {
							SimState::rules.pop_back();
							SimState::stateColors.pop_back();

							// remove reference from surrounding
							for (Rules& rule : SimState::rules) {
								rule.surrounding.pop_back();
							}

							// destroy all of those cells
							for (Cell& cell : SimState::cellMap) {
								if (cell.state == static_cast<int>(SimState::rules.size())) cell.state = 0;
							}

							// make sure state index isnt out of bounds
							if (SimState::edit_stateIndex >= static_cast<int>(SimState::rules.size())) SimState::edit_stateIndex = SimState::rules.size()-1;

							SimState::running = true;
						}

						break;
					
					case SDLK_RETURN:  // add topmost state
						Rules defaultRuleset {2, 4, 3, 3, 0, {0}};
						LOOP(SimState::rules.size()-1) defaultRuleset.surrounding.push_back(1);

						SimState::rules.push_back(defaultRuleset);
						SimState::stateColors.push_back(RGBA_t{255,255,255,255});

						// add surrounding SimState::rules to each state
						for (Rules& rule : SimState::rules) {
							rule.surrounding.push_back(1);
						}

						SimState::running = true;
						break;
				}
			}
		}

		// edit SimState::rules
		if (edit_desire != 0.0f) {
			if (SimState::edit_ruleIndex == 0) {  // minToSurvive
				SimState::rules[SimState::edit_stateIndex].minToSurvive += edit_desire;
				clamp(SimState::rules[SimState::edit_stateIndex].minToSurvive, SimState::minRuleVal, SimState::maxRuleVal);
				// std::round(value * 10.0f) / 10.0f;
			} else if (SimState::edit_ruleIndex == 1) {  // minToDie
				SimState::rules[SimState::edit_stateIndex].minToDie += edit_desire;
				clamp(SimState::rules[SimState::edit_stateIndex].minToDie, SimState::minRuleVal, SimState::maxRuleVal);
			} else if (SimState::edit_ruleIndex == 2) {  // minForReproduction
				SimState::rules[SimState::edit_stateIndex].minForReproduction += edit_desire;
				clamp(SimState::rules[SimState::edit_stateIndex].minForReproduction, SimState::minRuleVal, SimState::maxRuleVal);
			} else if (SimState::edit_ruleIndex == 3) {  // maxForReproduction
				SimState::rules[SimState::edit_stateIndex].maxForReproduction += edit_desire;
				clamp(SimState::rules[SimState::edit_stateIndex].maxForReproduction, SimState::minRuleVal, SimState::maxRuleVal);
			} else if (SimState::edit_ruleIndex == 4) {  // outOfBounds
				SimState::rules[SimState::edit_stateIndex].outOfBounds += edit_desire;
				clamp(SimState::rules[SimState::edit_stateIndex].outOfBounds, SimState::minRuleVal, SimState::maxRuleVal);
			} else {  // surrounding SimState::rules
				SimState::rules[SimState::edit_stateIndex].surrounding[SimState::edit_ruleIndex-5] += edit_desire;
				clamp(SimState::rules[SimState::edit_stateIndex].surrounding[SimState::edit_ruleIndex-5], SimState::minRuleVal, SimState::maxRuleVal);
			}

			SimState::running = true;
		}


		// TODO optimize checks

		if (SimState::frameDelay == 0 && SimState::paused != 0) {
			std::vector<std::thread> threads {};

			// chunks are vertical slices
			int xChunkSize = SimState::cellsWidth / SimState::numThreads;
			int overflow = SimState::cellsWidth % SimState::numThreads;

			for (size_t t = 0; t < SimState::numThreads; ++t) {
				threads.push_back(std::thread(doCellCalc, 
					0, SimState::cellsHeight, 
					xChunkSize*t, xChunkSize*(t+1) + (t == SimState::numThreads-1 ? overflow : 0)
				));
			}

			for (std::thread& t : threads) t.join();

			SimState::cellMap.swap(SimState::newMap);

			if (SimState::paused > 0) SimState::paused--;
		} else if (SimState::paused == 0) {
			// only calculate render
			// TODO put in function, same as in doCellCalc()
			for (size_t y = 0; y < SimState::cellsHeight; ++y) {
				for (size_t x = 0; x < SimState::cellsWidth; ++x) {
					for (size_t py = 0; py < SimState::pixelsPerCell; ++py) {
						for (size_t px = 0; px < SimState::pixelsPerCell; ++px) {
							SimState::screen[(SimState::pixelsPerCell*y+py)*SimState::screenWidth + (SimState::pixelsPerCell*x+px)] = SimState::stateColors[SimState::cellMap[y*SimState::cellsWidth + x].state];
						}
					}
				}
			}
		}

		renderer.render(SimState::screen);

		SimState::frameDelay--;
		if (SimState::frameDelay < 0) SimState::frameDelay = SimState::maxFrameDelay;

		// deltatime

		const auto frameElapsed = std::chrono::steady_clock::now() - timeStart;
		const std::chrono::duration<double> delta = frameElapsed;

		if (0 && delta.count() < 1.0/(SimState::fps)) {
			std::this_thread::sleep_for(std::chrono::duration<double>(1.0/(SimState::fps) - delta.count()));
		}
	}
	
	return 0;
}