#pragma once

#include "game_player.h"

namespace digital_curling {

	// GameLog
	// TODO: try not to use WritePrivateProffile()
	class GameLog {
	public:
		GameLog(const Player* const p1, const Player* const p2);
		~GameLog();

		// Create new file
		bool Create(const Player* const p1, const Player* const p2);

		// Write to logfile
		void Write(std::string message);

	private:
		std::string file_path_;
	};
}