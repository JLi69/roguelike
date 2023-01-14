#include "roguelike.h"
#include "gl-func.h"
#include <glad/glad.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "text.h"
#include "event.h"

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

	//Draw the items that can be picked up
	for(int i = 0; i < level->itemCount; i++)
	{
		if(level->items[i].hidden)
			continue;

		glUniform2f(getUniform(DEFAULT_UNIFORM_OFFSET),
					level->items[i].hitbox.x * TILE_SIZE - level->player.spr.x * TILE_SIZE,
					level->items[i].hitbox.y * TILE_SIZE - level->player.spr.y * TILE_SIZE);
		switch(level->items[i].item)
		{
		case COIN:
			glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
								   0.0f + 1.0f / 16.0f * (float)level->items[i].hitbox.frame,
								   12.0f / 16.0f);	
			break;
		default:
			break;	
		}
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	//Draw the enemies
	for(int i = 0; i < level->enemyCount; i++)
	{
		if(spriteDist(level->enemies[i].spr, level->player.spr) > 32.0f)
			continue;

		if(level->enemies[i].health <= 0 && level->enemies[i].damageCooldown <= 0.0f)
			continue;
		else if(level->enemies[i].health <= 0 && level->enemies[i].damageCooldown > 0.0f)	
			glUniform1f(getUniform(DEFAULT_UNIFORM_HIT_COOLDOWN), 1.0f); 

		if(level->enemies[i].damageCooldown >= 0.0f && level->enemies[i].health > 0)
			glUniform1f(getUniform(DEFAULT_UNIFORM_HIT_COOLDOWN), level->enemies[i].damageCooldown * 2.0f); 
		else if(level->enemies[i].health > 0)	
			glUniform1f(getUniform(DEFAULT_UNIFORM_HIT_COOLDOWN), 0.0f); 

		glUniform2f(getUniform(DEFAULT_UNIFORM_OFFSET),
					level->enemies[i].animationX * TILE_SIZE - level->player.spr.x * TILE_SIZE,
					level->enemies[i].animationY * TILE_SIZE - level->player.spr.y * TILE_SIZE);
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
		case SNAKE:
			if(level->enemies[i].spr.x <= level->player.spr.x)
			{
				glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
								   0.0f + 1.0f / 16.0f * (float)level->enemies[i].spr.frame + 2.0f * 1.0f / 16.0f,
								   13.0f / 16.0f);
			}
			else if(level->enemies[i].spr.x > level->player.spr.x)
			{
				glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
								   0.0f + 1.0f / 16.0f * (float)level->enemies[i].spr.frame,
								   13.0f / 16.0f);
			}
			break;
		case GHOST:
			glUniform1f(getUniform(DEFAULT_UNIFORM_ALPHA), 0.3f);		
			if(level->enemies[i].spr.x <= level->player.spr.x)
			{
				glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
								   0.0f + 1.0f / 16.0f * (float)level->enemies[i].spr.frame + 6.0f * 1.0f / 16.0f,
								   13.0f / 16.0f);
			}
			else if(level->enemies[i].spr.x > level->player.spr.x)
			{
				glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
								   4.0f / 16.0f + 1.0f / 16.0f * (float)level->enemies[i].spr.frame,
								   13.0f / 16.0f);
			}	
			break;
		case SKULL:
			if(level->enemies[i].spr.x <= level->player.spr.x)
			{
				glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
								   4.0f / 16.0f + 1.0f / 16.0f * (float)level->enemies[i].spr.frame,
								   14.0f / 16.0f);
			}
			else if(level->enemies[i].spr.x > level->player.spr.x)
			{
				glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
								   4.0f / 16.0f + 1.0f / 16.0f * (float)level->enemies[i].spr.frame + 2.0f / 16.0f,
								   14.0f / 16.0f);
			}	
			break;
		case EYEBALL:
			glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
								   8.0f / 16.0f + 1.0f / 16.0f * (float)level->enemies[i].spr.frame,
								   14.0f / 16.0f);
			break;
		}
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glUniform1f(getUniform(DEFAULT_UNIFORM_ALPHA), 1.0f);		
	}
	glUniform1f(getUniform(DEFAULT_UNIFORM_HIT_COOLDOWN), 0.0f); 

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
					i * HUD_SIZE / 2 + HUD_SIZE / 2 - level->player.maxHealth / 2 * HUD_SIZE / 2,
					getWindowHeight() / 2.0f - HUD_SIZE);
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

	//Display the player's score
	glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET), 
						   10.0f / 16.0f,
						   14.0f / 16.0f);	
	glUniform2f(getUniform(DEFAULT_UNIFORM_OFFSET),	
				-getDigits(level->player.score) * HUD_SIZE / 2.0f - HUD_SIZE / 2.0f,
				getWindowHeight() / 2.0f - HUD_SIZE - HUD_SIZE * 1.5f);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	drawNumber(-getDigits(level->player.score) * HUD_SIZE / 2.0f + HUD_SIZE / 2.0f,
			   getWindowHeight() / 2.0f - HUD_SIZE - HUD_SIZE * 1.5f,
			   HUD_SIZE, level->player.score); 

	//Attack animation
	if(level->player.attackCooldown > 0.0f)
	{
		useProgram(getShader(ATTACK_ANIMATION_SHADER));
		glUniform1f(getUniform(ATTACK_UNIFORM_ANIMATION_TIME), ATTACK_COOLDOWN_LENGTH - level->player.attackCooldown);		
		glUniform1i(getUniform(ATTACK_UNIFORM_DIRECTION), level->player.spr.animation / 2);	
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	//Print out any OpenGL errors
	getGLErrors();
}
