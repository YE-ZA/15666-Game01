#include "PlayMode.hpp"

// for the GL_ERRORS() macro:
#include "gl_errors.hpp"

// for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

PlayMode::PlayMode()
{
	// TODO:
	//  you *must* use an asset pipeline of some sort to generate tiles.
	//  don't hardcode them like this!
	//  or, at least, if you do hardcode them like this,
	//   make yourself a script that spits out the code that you paste in here
	//    and check that script into your repository.

	glm::uvec2 png_size;
	std::vector<glm::u8vec4> png_data;
	std::string png_path0 = "../asset/dragon.png";
	std::string png_path1 = "../asset/dragon_attack.png";
	std::string png_path2 = "../asset/grass.png";
	std::string png_path3 = "../asset/soil.png";
	std::string png_path4 = "../asset/fire_01.png";
	std::string png_path5 = "../asset/fire_02.png";

	// set draw infos
	player_right.palette_index = 0;
	player_right.tile_start_index = 0;
	player_left.palette_index = 0;
	player_left.tile_start_index = 9;
	player_right_attack.palette_index = 0;
	player_right_attack.tile_start_index = 18;
	player_left_attack.palette_index = 0;
	player_left_attack.tile_start_index = 27;
	grass.palette_index = 1;
	grass.tile_start_index = 36;
	soil.palette_index = 2;
	soil.tile_start_index = 45;
	fire_01.palette_index = 3;
	fire_01.tile_start_index = 54;
	fire_02.palette_index = 4;
	fire_02.tile_start_index = 79;

	png_to_runtime(png_path0, png_size, png_data, &player_right);
	png_to_runtime(png_path0, png_size, png_data, &player_left, true);
	png_to_runtime(png_path1, png_size, png_data, &player_right_attack);
	png_to_runtime(png_path1, png_size, png_data, &player_left_attack, true);
	png_to_runtime(png_path2, png_size, png_data, &grass);
	png_to_runtime(png_path3, png_size, png_data, &soil);
	png_to_runtime(png_path4, png_size, png_data, &fire_01);
	png_to_runtime(png_path5, png_size, png_data, &fire_02);

	// for cleaning sprite
	no_fire.tile_count = fire_01.tile_count;
	no_fire.tile_width = fire_01.tile_width;
	no_fire.tile_start_index = fire_01.tile_start_index;
	no_fire.palette_index = 7;

	// all transparent used for no_fire
	ppu.palette_table[7] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	};
}

PlayMode::~PlayMode()
{
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size)
{

	if (evt.type == SDL_KEYDOWN)
	{
		if (evt.key.keysym.sym == SDLK_LEFT)
		{
			left.downs += 1;
			left.pressed = true;
			is_move_right = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_RIGHT)
		{
			right.downs += 1;
			right.pressed = true;
			is_move_right = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_UP)
		{
			up.downs += 1;
			up.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_DOWN)
		{
			down.downs += 1;
			down.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_SPACE)
		{
			space.downs += 1;
			space.pressed = true;
			return true;
		}
	}
	else if (evt.type == SDL_KEYUP)
	{
		if (evt.key.keysym.sym == SDLK_LEFT)
		{
			left.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_RIGHT)
		{
			right.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_UP)
		{
			up.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_DOWN)
		{
			down.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_SPACE)
		{
			space.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed)
{
	constexpr float PlayerSpeed = 90.0f;
	if (left.pressed)
		player_at.x -= PlayerSpeed * elapsed;
	if (right.pressed)
		player_at.x += PlayerSpeed * elapsed;
	if (down.pressed)
		player_at.y -= PlayerSpeed * elapsed;
	if (up.pressed)
		player_at.y += PlayerSpeed * elapsed;

	// limit the moving boundary
	if (player_at.x < 8)
		player_at.x = 8;
	if (player_at.y < 0)
		player_at.y = 0;
	if (player_at.x > 224)
		player_at.x = 224;
	if (player_at.y > 220)
		player_at.y = 220;

	// record burn area
	if (space.pressed)
	{
		for (uint32_t i = uint32_t(std::max(0.0f, 1.5f * player_at.x / 8 - 2)); i <= uint32_t(1.5f * player_at.x / 8 + 4); ++i)
			for (uint32_t j = uint32_t(std::max(0.0f, 1.5f * player_at.y / 8 - 2)); j <= uint32_t(1.5f * player_at.y / 8 + 4); ++j)
			{
				if (std::abs((int)i - int(1.5f * player_at.x / 8 + 1)) + std::abs((int)j - int(1.5f * player_at.y / 8 + 1)) <= 3)
					bg_pos[i][j] = 1;
			}
	}

	// reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
	space.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size)
{
	//--- set ppu state based on game state ---

	// background color
	ppu.background_color = glm::u8vec4(155, 180, 135, 255);

	// background tile
	for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y)
	{
		for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x)
		{
			if (bg_pos[x][y] == 1)
				draw_background_tile(&soil, x, y);
			else
				draw_background_tile(&grass, x, y);
		}
	}

	// background scroll:
	ppu.background_position.x = int32_t(-0.5f * player_at.x);
	ppu.background_position.y = int32_t(-0.5f * player_at.y);

	// sprite number decide drawing order
	if (is_move_right)
	{
		if (space.pressed)
		{
			timer++;

			if (timer / 10 % 2)
				draw_sprite(&fire_01, 0, -1, -1);
			else
				draw_sprite(&fire_02, 0, -1, -1);
			draw_sprite(&player_right_attack, 25);
		}
		else
		{
			draw_sprite(&no_fire);
			draw_sprite(&player_right, 25);
		}
	}
	else
	{
		if (space.pressed)
		{
			timer++;

			if (timer / 10 % 2)
				draw_sprite(&fire_01, 0, -1, -1);
			else
				draw_sprite(&fire_02, 0, -1, -1);
			draw_sprite(&player_left_attack, 25);
		}
		else
		{
			draw_sprite(&no_fire);
			draw_sprite(&player_left, 25);
		}
	}

	//--- actually draw ---
	ppu.draw(drawable_size);
}

void PlayMode::png_to_runtime(std::string suffix, glm::uvec2 &png_size, std::vector<glm::u8vec4> &png_data, DrawInfo *draw_info, bool horizontal_flip)
{
	load_png(data_path(suffix), &png_size, &png_data, OriginLocation::LowerLeftOrigin);

	// add colors to palette
	std::vector<glm::u8vec4> target_color;

	// this way will have more than 4 target_color if pixels are not sorted
	// std::unique_copy(png_data.begin(), png_data.end(), std::back_inserter(target_color));
	// maybe use std::count

	// load 4 colors
	target_color.emplace_back(png_data[0]);
	for (auto data : png_data)
	{
		if (data != target_color[0])
		{
			if (target_color.size() == 1)
				target_color.emplace_back(data);
			if (data != target_color[1])
			{
				if (target_color.size() == 2)
					target_color.emplace_back(data);
				if (data != target_color[2])
				{
					target_color.emplace_back(data);
					break;
				}
			}
		}
	}

	assert(target_color.size() <= 4);

	std::copy(target_color.begin(), target_color.end(), ppu.palette_table[draw_info->palette_index].begin());

	// flip png data
	if (horizontal_flip)
	{
		std::vector<glm::u8vec4> origin_data = png_data;

		for (uint32_t i = 0; i < png_data.size(); ++i)
		{
			png_data[i] = origin_data[png_size.x - 1 - i % png_size.x + i / png_size.x * png_size.x];
		}
	}

	// trait multiple tile_tables as an 2D array
	uint8_t array_width = uint8_t(std::ceil(png_size.x / 8.0));
	uint8_t array_height = uint8_t(std::ceil(png_size.y / 8.0));
	draw_info->tile_width = array_width;
	draw_info->tile_count = array_width * array_height;

	// set tile table colors
	for (uint32_t i = 0; i < png_data.size(); ++i)
	{
		// where the tile located in the 2D array
		uint32_t array_x = i % png_size.x / 8;
		uint32_t array_y = i / png_size.x / 8;

		// location of pixel in each tile
		uint32_t pixel_x = i % png_size.x % 8;
		uint32_t pixel_y = i / png_size.x % 8;

		// set colors for each pixel
		if (png_data[i] == target_color[0])
		{
			ppu.tile_table[draw_info->tile_start_index + array_x + array_width * array_y].bit0[pixel_y] &= ~(1 << pixel_x);
			ppu.tile_table[draw_info->tile_start_index + array_x + array_width * array_y].bit1[pixel_y] &= ~(1 << pixel_x);
		}
		else if (png_data[i] == target_color[1])
		{
			ppu.tile_table[draw_info->tile_start_index + array_x + array_width * array_y].bit0[pixel_y] |= 1 << pixel_x;
			ppu.tile_table[draw_info->tile_start_index + array_x + array_width * array_y].bit1[pixel_y] &= ~(1 << pixel_x);
		}
		else if (png_data[i] == target_color[2])
		{
			ppu.tile_table[draw_info->tile_start_index + array_x + array_width * array_y].bit0[pixel_y] &= ~(1 << pixel_x);
			ppu.tile_table[draw_info->tile_start_index + array_x + array_width * array_y].bit1[pixel_y] |= 1 << pixel_x;
		}
		else if (png_data[i] == target_color[3])
		{
			ppu.tile_table[draw_info->tile_start_index + array_x + array_width * array_y].bit0[pixel_y] |= 1 << pixel_x;
			ppu.tile_table[draw_info->tile_start_index + array_x + array_width * array_y].bit1[pixel_y] |= 1 << pixel_x;
		}
	}
}

void PlayMode::draw_sprite(DrawInfo *draw_info, uint32_t sprite_start_index, int8_t offset_x, int8_t offset_y)
{
	for (uint8_t i = 0; i < draw_info->tile_count; ++i)
	{
		ppu.sprites[i + sprite_start_index].x = int8_t(player_at.x + i % draw_info->tile_width * 8) + offset_x * 8;
		ppu.sprites[i + sprite_start_index].y = int8_t(player_at.y + i / draw_info->tile_width * 8) + offset_y * 8;
		ppu.sprites[i + sprite_start_index].index = i + draw_info->tile_start_index;
		ppu.sprites[i + sprite_start_index].attributes = draw_info->palette_index;
	}
}

void PlayMode::draw_background_tile(DrawInfo *draw_info, uint32_t x, uint32_t y)
{
	if ((x + PPU466::BackgroundWidth * (y / 3)) % draw_info->tile_width == 0)
	{
		if (y % (draw_info->tile_count / draw_info->tile_width) == 0)
			ppu.background[x + PPU466::BackgroundWidth * y] = 0b0000000000000000 | (draw_info->palette_index << 8) | draw_info->tile_start_index;
		if (y % (draw_info->tile_count / draw_info->tile_width) == 1)
			ppu.background[x + PPU466::BackgroundWidth * y] = 0b0000000000000000 | (draw_info->palette_index << 8) | (draw_info->tile_start_index + 3);
		if (y % (draw_info->tile_count / draw_info->tile_width) == 2)
			ppu.background[x + PPU466::BackgroundWidth * y] = 0b0000000000000000 | (draw_info->palette_index << 8) | (draw_info->tile_start_index + 6);
	}
	if ((x + PPU466::BackgroundWidth * (y / 3)) % draw_info->tile_width == 1)
	{
		if (y % (draw_info->tile_count / draw_info->tile_width) == 0)
			ppu.background[x + PPU466::BackgroundWidth * y] = 0b0000000000000000 | (draw_info->palette_index << 8) | (draw_info->tile_start_index + 1);
		if (y % (draw_info->tile_count / draw_info->tile_width) == 1)
			ppu.background[x + PPU466::BackgroundWidth * y] = 0b0000000000000000 | (draw_info->palette_index << 8) | (draw_info->tile_start_index + 4);
		if (y % (draw_info->tile_count / draw_info->tile_width) == 2)
			ppu.background[x + PPU466::BackgroundWidth * y] = 0b0000000000000000 | (draw_info->palette_index << 8) | (draw_info->tile_start_index + 7);
	}
	if ((x + PPU466::BackgroundWidth * (y / 3)) % draw_info->tile_width == 2)
	{
		if (y % (draw_info->tile_count / draw_info->tile_width) == 0)
			ppu.background[x + PPU466::BackgroundWidth * y] = 0b0000000000000000 | (draw_info->palette_index << 8) | (draw_info->tile_start_index + 2);
		if (y % (draw_info->tile_count / draw_info->tile_width) == 1)
			ppu.background[x + PPU466::BackgroundWidth * y] = 0b0000000000000000 | (draw_info->palette_index << 8) | (draw_info->tile_start_index + 5);
		if (y % (draw_info->tile_count / draw_info->tile_width) == 2)
			ppu.background[x + PPU466::BackgroundWidth * y] = 0b0000000000000000 | (draw_info->palette_index << 8) | (draw_info->tile_start_index + 8);
	}
}