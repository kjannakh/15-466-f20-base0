#include "ColorTextureProgram.hpp"

#include "Mode.hpp"
#include "GL.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

#include <random>

/*
 * PongMode is a game mode that implements a single-player game of Pong.
 */

struct PongMode : Mode {
	PongMode();
	virtual ~PongMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	float courtX = 7.0f;
	float courtY = 5.0f;
	glm::vec2 court_radius = glm::vec2(courtX, courtY);
	glm::vec2 paddle_radius = glm::vec2(0.2f, 1.0f);
	float boost_size = 0.15f;
	glm::vec2 boost_radius = glm::vec2(0.2f, 0.2f);
	glm::vec2 ball_radius = glm::vec2(0.2f, 0.2f);

	glm::vec2 left_paddle = glm::vec2(-court_radius.x + 0.5f, 0.0f);
	glm::vec2 right_paddle = glm::vec2( court_radius.x - 0.5f, 0.0f);

	glm::vec2 ball = glm::vec2(0.0f, 0.0f);
	glm::vec2 ball_velocity = glm::vec2(-1.0f, 0.0f);

	float curve = 0.0f;
	// Hard-coded multipliers that affect strength of curveballs
	float curve_multiplier = 0.35f;
	float bend_multiplier = 3.5f;

	uint32_t left_score = 0;
	uint32_t right_score = 0;

	float ai_offset = 0.0f;
	float ai_offset_update = 0.5f;
	float target_offset = 0.0f;

	// Track paddle velocities for curveballs
	float left_pos = left_paddle.y;
	float right_pos = right_paddle.y;
	float player_vel = 0.0f;
	float ai_vel = 0.0f;
	float max_bend_factor = 20.0f;
	float bend_factor = 0.0f;
	float bend = 0.0f;
	bool ball_boost = 0;

	// Variable AI difficulty
	float ai_diff_min = 2.0f;
	float ai_diff_max = 4.0f; // Max is actually this plus min
	float ai_diff_max_pts = 5.0f;

	// Keep track of total time for ball speed
	float time_elapsed = 0.0f;
	float max_ball_speed = 9.0f; // Max is actually this plus min
	float max_speed_time = 60.0f;

	//random targets
	glm::vec2 target_radius = glm::vec2(0.5f, 0.5f);
	std::mt19937 mt; //mersenne twister pseudo-random number generator
	glm::vec2 target = glm::vec2(-target_range + mt() / float(mt.max()) * 2.0f * target_range, (mt() > mt.max() / 2) ? courtY - target_radius.y : -courtY + target_radius.y);
	float target_hit_time = 0.0f;
	bool target_hit = false;
	float respawn_time = 1.0f;
	int last_paddle_hit = 0;
	float target_range = 4.0f;

	//----- pretty rainbow trails -----

	float trail_length = 1.3f;
	std::deque< glm::vec3 > ball_trail; //stores (x,y,age), oldest elements first

	//----- opengl assets / helpers ------

	//draw functions will work on vectors of vertices, defined as follows:
	struct Vertex {
		Vertex(glm::vec3 const &Position_, glm::u8vec4 const &Color_, glm::vec2 const &TexCoord_) :
			Position(Position_), Color(Color_), TexCoord(TexCoord_) { }
		glm::vec3 Position;
		glm::u8vec4 Color;
		glm::vec2 TexCoord;
	};
	static_assert(sizeof(Vertex) == 4*3 + 1*4 + 4*2, "PongMode::Vertex should be packed");

	//Shader program that draws transformed, vertices tinted with vertex colors:
	ColorTextureProgram color_texture_program;

	//Buffer used to hold vertex data during drawing:
	GLuint vertex_buffer = 0;

	//Vertex Array Object that maps buffer locations to color_texture_program attribute locations:
	GLuint vertex_buffer_for_color_texture_program = 0;

	//Solid white texture:
	GLuint white_tex = 0;

	//matrix that maps from clip coordinates to court-space coordinates:
	glm::mat3x2 clip_to_court = glm::mat3x2(1.0f);
	// computed in draw() as the inverse of OBJECT_TO_CLIP
	// (stored here so that the mouse handling code can use it to position the paddle)

};
