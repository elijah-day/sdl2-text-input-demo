/* Copyright (C) 2023 Elijah Day

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the “Software”), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define KEY_COUNT 322

#define STR_LEN 256

#define FONT_SIZE 32
#define FONT_PATH "data/fonts/mononoki-Regular.ttf"

#define ASPECT_RATIO 1
#define RENDER_SCALE 24

SDL_Texture *load_font_texture
(
	SDL_Renderer *renderer,
	TTF_Font *font,
	SDL_Color font_color,
	const char *font_text
)
{
	SDL_Texture *font_texture = NULL;

	if(font)
	{
		SDL_Surface *font_surface =
		TTF_RenderText_Solid
		(
			font,
			font_text,
			font_color
		);
		
		if(font_surface == NULL)
		{
			printf
			(
				"TTF_RenderText_Solid error: %s\n",
				SDL_GetError()
			);
		}
		
		font_texture = SDL_CreateTextureFromSurface
		(
			renderer,
			font_surface
		);
		
		if(font_texture == NULL)
		{
			printf
			(
				"SDL_CreateTextureFromSurface error: %s\n",
				SDL_GetError()
			);
		}
		
		SDL_FreeSurface(font_surface);
		
		return font_texture;
	}
	else
	{
		printf
		(
			"NULL font\n"
		);
	}
	
	return NULL;
}

int main(int argc, char *argv[])
{
	bool running_state = true;

	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("SDL_Init error: %s\n", SDL_GetError());
		running_state = false;
	}
	
	TTF_Init();

	SDL_Window *window = SDL_CreateWindow
	(
		"",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		720,
		360,
		SDL_WINDOW_SHOWN
	);
	
	SDL_Renderer *renderer = SDL_CreateRenderer
	(
		window,
		-1,
		SDL_RENDERER_ACCELERATED
	);

	SDL_Color font_color = {255, 255, 255, 255};
	TTF_Font *font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
	SDL_Rect dstrect;
	
	if(font == NULL)
	{
		printf
		(
			"TTF_OpenFont error: %s\n",
			SDL_GetError()
		);
	}
	
	SDL_Texture *font_texture = NULL;
	
	SDL_Event event;
	
	bool key_presses[KEY_COUNT];
	bool key_hold_state = false;
	
	for(int i = 0; i < KEY_COUNT; i++) key_presses[i] = false;
	
	char str[STR_LEN] = "";
	int str_pos = -1;
	
	int str_len = 0;
	
	SDL_StartTextInput();
	
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	
	while(running_state)
	{	
		while(SDL_PollEvent(&event)) switch(event.type)
		{
			case SDL_QUIT:
				running_state = false;
				break;
				
			case SDL_TEXTINPUT:
				if(strlen(str) < STR_LEN - 1)
				{
					strcat(str, event.text.text);
					str_pos++;
					str_len = strlen(str);
				}
				break;
				
			case SDL_KEYDOWN:
				key_presses[event.key.keysym.scancode] = true;
				break;
				
			case SDL_KEYUP:
				key_presses[event.key.keysym.scancode] = false;
				break;
		}
		
		/*There's probably a better way of doing this.*/
		if(key_presses[SDL_SCANCODE_BACKSPACE] && str_pos > -1)
		{
			for(int i = str_pos; i < STR_LEN; i++)
			{
				str[i] = '\0';
			}
			
			str_len = strlen(str);
			str_pos--;
		}
		
		if(key_presses[SDL_SCANCODE_RETURN])
		{
			if(strcmp(str, "exit") == 0)
			{
				running_state = false;
			}
			else if(strcmp(str, "bg") == 0)
			{
			 	SDL_SetRenderDrawColor
			 	(
			 		renderer,
			 		rand() % 256,
			 		rand() % 256,
			 		rand() % 256,
			 		rand() % 256
			 	);
			}
			else if(strcmp(str, "fg") == 0)
			{
			 	font_color.r = rand() % 256;
			 	font_color.g = rand() % 256;
			 	font_color.b = rand() % 256;
			 	font_color.a = rand() % 256;
			}
		
			for(int i = 0; i < STR_LEN; i++)
			{
				str[i] = '\0';
			}
			
			str_len = 0;
			str_pos = -1;
		}
		
		for(int i = 0; i < KEY_COUNT; i++)
		{
			if(key_presses[i])
			{
				key_hold_state = true;
				key_presses[i] = false;
				break;
			}
			
			key_hold_state = false; 
		}
		
		SDL_RenderFillRect(renderer, NULL);
		
		if(key_hold_state == true)
		{
			dstrect.x = 0;
			dstrect.y = 0;
			dstrect.w = RENDER_SCALE * ASPECT_RATIO * str_len;
			dstrect.h = RENDER_SCALE;
		
			font_texture = load_font_texture
			(
				renderer,
				font,
				font_color,
				str
			);
			
			if(font_texture == NULL)
			{
				printf
				(
					"load_font_texture error: %s\n",
					SDL_GetError()
				);
			}
		}
		
		if
		(
			SDL_RenderCopy
			(
				renderer,
				font_texture,
				NULL,
				&dstrect
			)
			!= 0
		)
		{
			printf
			(
				"SDL_RenderCopy error: %s\n",
				SDL_GetError()
			);
		}
		
		SDL_RenderPresent(renderer);
	}

	if(font_texture) SDL_DestroyTexture(font_texture);

	TTF_CloseFont(font);
	
	SDL_SetWindowFullscreen(window, 0);
	
	TTF_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
