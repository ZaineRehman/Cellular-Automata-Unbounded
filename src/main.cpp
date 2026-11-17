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
#include "input.hpp"
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

			// output rules

			if (SimState::simMode == 0) {
				std::cout
					<< "[Arrows] to select rule, [+],[-] to change value\nKeypad [+],[-] changes by 0.1\n[Q][W][E],[A][S][D] to change color\n\n"
					<< "          SURVIVAL    REPRODUCE  BOUNDS   SURROUNDINGS" << '\n'
					<< "          min  lim    min  max                         " << '\n';
				for (size_t ri = 0; ri < SimState::rules.size(); ++ri) {
					Rules thisRule = SimState::rules[ri];
					std::cout 
						<< "State " << ri << ": "
						<< getPresentColor(ri, 0) << padString(round(thisRule.minToSurvive, 1), 4) << ansi_reset << ","
						<< getPresentColor(ri, 1) << padString(round(thisRule.minToDie, 1), 4) << ansi_reset << " ~ "
						<< getPresentColor(ri, 2) << padString(round(thisRule.minForReproduction, 1), 4) << ansi_reset << ","
						<< getPresentColor(ri, 3) << padString(round(thisRule.maxForReproduction, 1), 4) << ansi_reset << " ~ "
						<< getPresentColor(ri, 4) << padString(round(thisRule.outOfBounds, 1), 4) << ansi_reset << " {";
					for (size_t si = 0; si < thisRule.surrounding.size(); ++si) {
						std::cout 
							<< getPresentColor(ri, 5 + si) 
							<< padString(round(thisRule.surrounding[si], 1), 4) 
							<< ansi_reset << (si == thisRule.surrounding.size()-1 ? "}  [": ",");
					}
					// color
					std::cout 
						<< ansi_red   << padString(SimState::stateColors[ri].r, 3) << ansi_reset << " "
						<< ansi_green << padString(SimState::stateColors[ri].g, 3) << ansi_reset << " "
						<< ansi_blue  << padString(SimState::stateColors[ri].b, 3) << ansi_reset << "]\n";
				}
			} else if (SimState::simMode == 1) {
				std::cout
					<< "[Arrows] to select rule, [0]-[9] or [+],[-]to change value\n[Q][W][E],[A][S][D] to change color\n";
				for (size_t ri = 0; ri < SimState::rules.size(); ++ri) {
					Rules thisRule = SimState::rules[ri];

					// output birth values
					std::cout << getPresentColor(ri, 0) << "B";
					for (size_t sb = 0; sb < thisRule.sv_birthValues.size(); ++sb) {
						std::cout << getPresentColor(ri, 0) << thisRule.sv_birthValues[sb] << ansi_reset;
					}

					// output survival values
					std::cout << "/" << getPresentColor(ri, 1) << "S";
					for (size_t sb = 0; sb < thisRule.sv_survivalValues.size(); ++sb) {
						std::cout << getPresentColor(ri, 1) << thisRule.sv_survivalValues[sb] << ansi_reset;
					}

					// output surrounding values
					std::cout << ansi_reset << " {";
					for (size_t si = 0; si < thisRule.sv_surrounding.size(); ++si) {
						std::cout 
							<< getPresentColor(ri, 2 + si) 
							<< padString(thisRule.sv_surrounding[si], 2) 
							<< ansi_reset << (si == thisRule.sv_surrounding.size()-1 ? "}  [": ",");
					}
					// color
					std::cout 
						<< ansi_red   << padString(SimState::stateColors[ri].r, 3) << ansi_reset << " "
						<< ansi_green << padString(SimState::stateColors[ri].g, 3) << ansi_reset << " "
						<< ansi_blue  << padString(SimState::stateColors[ri].b, 3) << ansi_reset << "]\n";
				}
			}
			std::cout
				<< "\n[O],[P] Mouse radius: " << SimState::mouseRadius << '\n'
				<<   "[U],[I] Mouse fill method: " << SimState::mouseFillMethod << '\n'
				<< "\n[<],[>] Pixels per cell: " << SimState::pixelsPerCell << '\n';

			/* TODO look into why this crashes
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
		// mouse position
		float mouseX, mouseY;

		// we gonna do mouse first lol
		uint32_t mouseState = SDL_GetMouseState(&mouseX, &mouseY);

		Inputs::handleMouseEvents(mouseX, mouseY, mouseState);
		
		Inputs::handleKeyEvents(event);


		// TODO optimize checks

		if (SimState::frameDelay == 0 && SimState::paused != 0) {
			// calculate next generation then render
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