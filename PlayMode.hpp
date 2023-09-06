#include "PPU466.hpp"
#include "Mode.hpp"
#include "load_save_png.hpp"
#include "data_path.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode
{
	PlayMode();
	virtual ~PlayMode();

	// functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	// input tracking:
	struct Button
	{
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, space;

	// some weird background animation:
	float background_fade = 0.0f;

	// player position:
	glm::vec2 player_at = glm::vec2(8.0f, 0.0f);

	//----- drawing handled by PPU466 -----

	PPU466 ppu;

	//----- Custom -----
	struct DrawInfo
	{
		uint8_t tile_start_index = 0;
		uint8_t tile_width = 0;
		uint8_t tile_count = 0;
		uint8_t palette_index = 0;
	} player_right, player_left, player_right_attack, player_left_attack, grass, soil, fire_01, fire_02, no_fire;

	bool is_move_right = true;

	uint32_t bg_pos[64][60] = {0};

	uint32_t timer = 0;

	void png_to_runtime(std::string suffix, glm::uvec2 &png_size, std::vector<glm::u8vec4> &png_data, DrawInfo *draw_info, bool horizontal_flip = false);
	void draw_sprite(DrawInfo *draw_info, uint32_t sprite_start_index = 0, int8_t offset_x = 0, int8_t offset_y = 0);
	void draw_background_tile(DrawInfo *draw_info, uint32_t x, uint32_t y);
};
