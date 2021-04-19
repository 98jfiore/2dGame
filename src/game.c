#include <SDL.h>

#include <math.h>

#include "simple_logger.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "gfc_audio.h"

#include "entity.h"
#include "player.h"
#include "ent_npc.h"
#include "ent_environ.h"
#include "loader.h"
#include "level.h"
#include "ui.h"
#include "menu.h"
#include "event.h"
#include "font.h"
#include "game_state.h"

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
	Level *level;
    
    int mx,my;
	SDL_bool paused = SDL_FALSE;
	Uint8 game_state = 0;
    float mf = 0;
    Sprite *mouse;
    Vector4D mouseColor = {255,100,255,200};
    
    /*program initializtion*/
    init_logger("gf2d.log");
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        1200,
        720,
        1200,
        720,
        vector4d(0,0,0,255),
        0);
    gf2d_graphics_set_frame_delay(16);
    gf2d_sprite_init(1024);
	entity_manager_init(500);
    SDL_ShowCursor(SDL_DISABLE);
	
	/*Start audio*/
	gfc_audio_init(10, 4, 2, 4, SDL_TRUE, SDL_FALSE);

	font_init(5);

	level = level_load("defs/home_screen.json");
	
    
    /*demo setup*/
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16);

	start_game();


    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;
        
		if (!(game_state & GS_PAUSED))
		{
			entity_manager_update_entities();
			if (game_state & GS_CUTSCENE)
			{
				event_manager_update();
			}
		}
		ui_manager_update_components();
		menu_manager_update();
        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
			level_draw();

			entity_manager_draw_entities();

			ui_manager_draw_components();

			menu_manager_draw_components();

			event_manager_draw();

            //UI elements last
            gf2d_sprite_draw(
                mouse,
                vector2d(mx,my),
                NULL,
                NULL,
                NULL,
                NULL,
                &mouseColor,
                (int)mf);


        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
        
		if (keys[SDL_SCANCODE_ESCAPE])
		{
			if (menu_manager_active() == SDL_FALSE)
			{
				pause_game();
				menu_format_load("defs/menus/pause_menu.json");
			}
			//If there is no menu, open pause menu
			//done = 1; // exit condition
		}
		game_state = get_gameState();

		if (game_state & GS_QUIT)
		{
			done = 1;
		}
        //slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/
