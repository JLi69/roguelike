#include "roguelike.h"
#include "gl-func.h"
#include <glad/glad.h>
#include <stdlib.h>

void display(Level *level)
{	
	bindGLObj(getBuffer(SQUARE));	

	//Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);	

	//Draw the background
	useProgram(getShader(BACKGROUND_SHADER));
	activateTexture(getTexture(TILE_TEXTURE_MAP), GL_TEXTURE0);
	glUniform2f(getUniform(BACKGROUND_UNIFORM_TEX_OFFSET), 0.0f, 15.0f / 16.0f);
	glUniform2f(getUniform(BACKGROUND_UNIFORM_SCALE), TILE_SIZE, TILE_SIZE);
	glUniform1f(getUniform(BACKGROUND_UNIFORM_TEX_SIZE), 256.0f);
	glUniform1f(getUniform(BACKGROUND_UNIFORM_TEX_FRAC), 1.0f / 16.0f);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 49 * 33);	

	useProgram(getShader(DEFAULT_SHADER));
	glUniform2f(getUniform(DEFAULT_UNIFORM_SCALE), TILE_SIZE, TILE_SIZE);	
	glUniform1f(getUniform(DEFAULT_UNIFORM_TEX_SIZE), 256.0f);	
	glUniform1f(getUniform(DEFAULT_UNIFORM_TEX_FRAC), 1.0f / 16.0f);	
	activateTexture(getTexture(TILE_TEXTURE_MAP), GL_TEXTURE0);

	//Draw the level
	for(int i = 0; i < level->width * level->height; i++)
	{
		float x = (float)(i % level->width) * TILE_SIZE - TILE_SIZE * level->player.spr.x,
			  y = (float)(i / level->width) * TILE_SIZE - TILE_SIZE * level->player.spr.y;	

		if(x <= -24.0f * TILE_SIZE || x >= 24.0f * TILE_SIZE 
			|| y <= -16.0f * TILE_SIZE || y >= 16.0f * TILE_SIZE)
			continue;	

		glUniform2f(getUniform(DEFAULT_UNIFORM_OFFSET), x, y);			

		//Draw different textures for different tiles
		switch(level->tiles[i])
		{
		case EMPTY:
			continue;		
		case FLOOR:
			glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 1.0f / 16.0f, 15.0f / 16.0f);	
			break;
		case WALL:
			glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
						2.0f / 16.0f + (float)(level->randVals[i] % 6) * 1.0f / 16.0f,
						15.0f / 16.0f);	
			break;
		case EXIT:
			glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
						8.0f / 16.0f,
						15.0f / 16.0f);	
			break;
		case CHEST:
			glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
						0.0f / 16.0f,
						14.0f / 16.0f);
			break;
		case CHEST_MONSTER:
			glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
						2.0f / 16.0f,
						14.0f / 16.0f);	
			break;
		case OPEN_CHEST:
			glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
						1.0f / 16.0f,
						14.0f / 16.0f);	
			break;
		case SPIKE_TRAP_INACTIVE:
			glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
						3.0f / 16.0f,
						14.0f / 16.0f);	
			break;
		case SPIKE_TRAP_ACTIVE:
			glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
						4.0f / 16.0f,
						14.0f / 16.0f);
			break;
		}

		glDrawArrays(GL_TRIANGLES, 0, 6);	
	}

	activateTexture(getTexture(SPRITE_TEXTURE_MAP), GL_TEXTURE0);	
	
	//Draw the enemies
	//Sort the enemies
	for(int i = 0; i < level->enemyCount; i++)
	{
		glUniform2f(getUniform(DEFAULT_UNIFORM_OFFSET), level->enemies[i].animationX * TILE_SIZE - level->player.spr.x * TILE_SIZE, level->enemies[i].animationY * TILE_SIZE - level->player.spr.y * TILE_SIZE);
		switch(level->enemies[i].type)
		{
		case OPEN_CHEST_MONSTER:
			glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
								   0.0f + 1.0f / 16.0f * (float)level->enemies[i].spr.frame,
								   14.0f / 16.0f);	
			break;
		case SLIME:
			glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
								   0.0f + 1.0f / 16.0f * (float)level->enemies[i].spr.frame + 2.0f * 1.0f / 16.0f,
								   14.0f / 16.0f);
			break;	
		}
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}		
	
	//Draw the player				
	glUniform2f(getUniform(DEFAULT_UNIFORM_OFFSET), 0.0f, 0.0f);
	glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
									 0.0f + 1.0f / 16.0f * (float)level->player.spr.frame +
									 (float)level->player.spr.animation * 2.0f * 1.0f / 16.0f,
									 15.0f / 16.0f);	
	glDrawArrays(GL_TRIANGLES, 0, 6);	

	//Draw the health bar
	activateTexture(getTexture(ICON_TEXTURE_MAP), GL_TEXTURE0);
	glUniform2f(getUniform(DEFAULT_UNIFORM_SCALE), HUD_SIZE, HUD_SIZE);
	for(int i = 0; i < level->player.maxHealth; i += 2)
	{
		glUniform2f(getUniform(DEFAULT_UNIFORM_OFFSET),
					i * HUD_SIZE / 2 - level->player.maxHealth / 2 * HUD_SIZE / 2 + HUD_SIZE / 2,
										 TILE_SIZE / 2 + HUD_SIZE);
		if(level->player.health - i >= 2)	
			glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
											0.0f / 16.0f,
											15.0f / 16.0f);
		else if(level->player.health - i == 1)
			glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
											1.0f / 16.0f,
											15.0f / 16.0f);
		else if(level->player.health - i <= 0)
			glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
											2.0f / 16.0f,
											15.0f / 16.0f);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	getGLErrors();
}