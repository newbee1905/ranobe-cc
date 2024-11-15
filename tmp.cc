#include <fmt/core.h>
#include <fmt/color.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <algorithm>

class TextViewer {
private:
	std::vector<std::string> content;
	size_t current_line = 0;
	size_t horizontal_scroll = 0;
	size_t window_height;
	size_t window_width;
	struct termios original_termios;
	std::string search_query;
	std::vector<size_t> search_results;
	size_t current_result = 0;
	bool wrap_lines = false;
	bool mouse_enabled = true;

	void enableRawMode() {
		tcgetattr(STDIN_FILENO, &original_termios);
		struct termios raw = original_termios;
		raw.c_lflag &= ~(ECHO | ICANON);
		raw.c_cc[VMIN] = 0;  // No minimum input
		raw.c_cc[VTIME] = 1; // 100ms timeout
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
	}

	void disableRawMode() {
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
	}

	void enableMouse() {
		fmt::print("\x1B[?1000h"); // Enable mouse click tracking
		fmt::print("\x1B[?1002h"); // Enable mouse movement tracking
		fmt::print("\x1B[?1015h"); // Enable urxvt style mouse support
		fmt::print("\x1B[?1006h"); // Enable SGR mouse support
	}

	void disableMouse() {
		fmt::print("\x1B[?1000l");
		fmt::print("\x1B[?1002l");
		fmt::print("\x1B[?1015l");
		fmt::print("\x1B[?1006l");
	}

	void clearScreen() {
		fmt::print("\x1B[2J\x1B[H");
	}

	void getWindowSize() {
		struct winsize w;
		if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != -1) {
			window_height = w.ws_row - 4; // Reserve space for header/footer
			window_width = w.ws_col;
		} else {
			window_height = 24;
			window_width = 80;
		}
	}

	std::string getUserInput(const std::string& prompt) {
		disableRawMode();
		disableMouse();
		clearScreen();
		fmt::print("{}", prompt);
		
		std::string input;
		std::getline(std::cin, input);
		
		enableRawMode();
		if (mouse_enabled) enableMouse();
		return input;
	}

	void performSearch() {
		search_results.clear();
		current_result = 0;

		if (search_query.empty()) return;

		std::string query_lower = search_query;
		std::transform(query_lower.begin(), query_lower.end(), 
					  query_lower.begin(), ::tolower);

		for (size_t i = 0; i < content.size(); ++i) {
			std::string line_lower = content[i];
			std::transform(line_lower.begin(), line_lower.end(), 
						 line_lower.begin(), ::tolower);
			
			if (line_lower.find(query_lower) != std::string::npos) {
				search_results.push_back(i);
			}
		}
	}

	std::vector<std::string> wrapLine(const std::string& line) {
		std::vector<std::string> wrapped;
		if (!wrap_lines || line.length() <= window_width) {
			wrapped.push_back(line);
			return wrapped;
		}

		size_t pos = 0;
		while (pos < line.length()) {
			wrapped.push_back(line.substr(pos, window_width));
			pos += window_width;
		}
		return wrapped;
	}

	void displayLine(const std::string& line, bool highlight_search = true) {
		std::string display_line;
		
		if (wrap_lines) {
			display_line = line;
		} else {
			if (horizontal_scroll < line.length()) {
				display_line = line.substr(horizontal_scroll, window_width);
			}
		}

		if (highlight_search && !search_query.empty()) {
			std::string line_lower = display_line;
			std::string query_lower = search_query;
			std::transform(line_lower.begin(), line_lower.end(), 
						 line_lower.begin(), ::tolower);
			std::transform(query_lower.begin(), query_lower.end(), 
						 query_lower.begin(), ::tolower);
			
			size_t pos = 0;
			while ((pos = line_lower.find(query_lower, pos)) != std::string::npos) {
				fmt::print("{}", display_line.substr(0, pos));
				fmt::print(fg(fmt::color::black) | bg(fmt::color::white), 
						 "{}", display_line.substr(pos, search_query.length()));
				display_line = display_line.substr(pos + search_query.length());
				line_lower = line_lower.substr(pos + query_lower.length());
				pos = 0;
			}
		}
		fmt::print("{}\n", display_line);
	}

	void displayContent() {
		clearScreen();
		
		// Header
		fmt::print(fg(fmt::color::steel_blue), 
				  "File: Lines {}-{} of {} ({}%) | Scroll: {} | Wrap: {}", 
				  current_line + 1,
				  std::min(current_line + window_height, content.size()),
				  content.size(),
				  (current_line * 100) / std::max(size_t(1), content.size()),
				  horizontal_scroll,
				  wrap_lines ? "On" : "Off");

		if (!search_query.empty() && !search_results.empty()) {
			fmt::print(fg(fmt::color::steel_blue), " | Search: '{}' ({}/{})",
				search_query, current_result + 1, search_results.size());
		}
		fmt::print("\n");
		
		fmt::print(fg(fmt::color::gray), 
				  "────────────────────────────────────\n");

		// Content
		size_t displayed_lines = 0;
		for (size_t i = current_line; 
			 i < content.size() && displayed_lines < window_height; ++i) {
			
			if (wrap_lines) {
				auto wrapped = wrapLine(content[i]);
				for (const auto& line : wrapped) {
					if (displayed_lines >= window_height) break;
					displayLine(line);
					displayed_lines++;
				}
			} else {
				displayLine(content[i]);
				displayed_lines++;
			}
		}

		// Footer
		fmt::print(fg(fmt::color::gray), 
				  "────────────────────────────────────\n");
		fmt::print(fg(fmt::color::dark_gray), 
			"j:↓ k:↑ h:← l:→ q:quit g:top G:bottom /:search n:next N:prev w:wrap m:mouse\n");
	}

	void handleMouse() {
		char seq[32];
		if (read(STDIN_FILENO, seq, sizeof(seq)) <= 0) return;

		// Parse mouse input
		if (seq[0] == '\x1b' && seq[1] == '[' && seq[2] == 'M') {
			int button = seq[3] - 32;
			int x = seq[4] - 32;
			int y = seq[5] - 32;

			if (button & 0x40) { // Wheel events
				if (button & 0x01) { // Wheel up
					if (current_line > 0) current_line--;
				} else { // Wheel down
					if (current_line + window_height < content.size()) current_line++;
				}
			}
		}
	}

public:
	TextViewer(const std::string& filename) {
		std::ifstream file(filename);
		if (!file.is_open()) {
			throw std::runtime_error("Cannot open file: " + filename);
		}

		std::string line;
		while (std::getline(file, line)) {
			content.push_back(line);
		}

		getWindowSize();
	}

	void run() {
		enableRawMode();
		if (mouse_enabled) enableMouse();
		
		bool running = true;
		while (running) {
			displayContent();
			
			char c;
			if (read(STDIN_FILENO, &c, 1) <= 0) continue;
			
			if (c == '\x1b') { // Handle mouse events
				handleMouse();
				continue;
			}
			
			switch (c) {
				case 'j': // Down
					if (current_line + window_height < content.size()) {
						current_line++;
					}
					break;
					
				case 'k': // Up
					if (current_line > 0) {
						current_line--;
					}
					break;
					
				case 'h': // Left
					if (horizontal_scroll > 0) {
						horizontal_scroll--;
					}
					break;
					
				case 'l': // Right
					if (!wrap_lines) {
						horizontal_scroll++;
					}
					break;
					
				case 'g': // Top
					current_line = 0;
					horizontal_scroll = 0;
					break;
					
				case 'G': // Bottom
					if (content.size() > window_height) {
						current_line = content.size() - window_height;
					}
					break;
					
				case '/': { // Search
					search_query = getUserInput("Search: ");
					performSearch();
					if (!search_results.empty()) {
						current_line = search_results[0];
					}
					break;
				}
				
				case 'n': // Next search result
					if (!search_results.empty()) {
						current_result = (current_result + 1) % search_results.size();
						current_line = search_results[current_result];
					}
					break;
					
				case 'N': // Previous search result
					if (!search_results.empty()) {
						current_result = 
							(current_result + search_results.size() - 1) % 
							search_results.size();
						current_line = search_results[current_result];
					}
					break;

				case 'w': // Toggle word wrap
					wrap_lines = !wrap_lines;
					horizontal_scroll = 0; // Reset horizontal scroll when toggling wrap
					break;

				case 'm': // Toggle mouse support
					mouse_enabled = !mouse_enabled;
					if (mouse_enabled) {
						enableMouse();
					} else {
						disableMouse();
					}
					break;
					
				case 'q': // Quit
					running = false;
					break;
			}
		}
		
		disableMouse();
		disableRawMode();
		clearScreen();
	}
};

int main(int argc, char* argv[]) {
	if (argc != 2) {
		fmt::print(fg(fmt::color::red), "Usage: {} <filename>\n", argv[0]);
		return 1;
	}

	try {
		TextViewer viewer(argv[1]);
		viewer.run();
	} catch (const std::exception& e) {
		fmt::print(fg(fmt::color::red), "Error: {}\n", e.what());
		return 1;
	}

	return 0;
}
