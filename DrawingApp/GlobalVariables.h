#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <Windows.h>
#include "Structs.h"

static DrawingState DRAWING_STATE = DrawingState::ALPHA;
sf::Shader alphaBlendingShader;
sf::Shader normalBlendingShader;
sf::Shader mainRenderShader;
sf::RenderStates alphaBlendingRState(&alphaBlendingShader);
sf::RenderStates normalBlendingRState(&normalBlendingShader);
sf::RenderStates mainRenderState(&mainRenderShader);

static const std::string ALPHA_BLENDING_SHADER_PATH = "./alpha_blending_shader.frag";
static const std::string NORMAL_BLENDING_SHADER_PATH = "./normal_blending_shader.frag";
static const std::string RENDER_SHADER_PATH = "./render_shader.frag";
static const std::string BRUSH_DIRECTORY = "./Brushes/";
static const std::string SCENES_DIRECTORY = "./Scenes/";
static const std::string SAVED_FILES_DIRECTORY = "./SavedFiles";
static const std::string TMP_DIRECTORY = "./SavedFiles/tmp";

static sf::Clock deltaClock; //For imgui
static sf::Clock clickClock; //For double click timing

static sf::RenderWindow mainWindow;
static sf::Event event;

sf::RenderTexture mainRenderTex;
sf::Texture mainNormalTex;
sf::Sprite mainNormalSprite;
sf::Sprite mainRenderSprite;

sf::Texture normalTex; // texture for the normal color picker
sf::Texture samplingTexture; //texture used for sampling the normal color
sf::Vector2i sampledPos;

bool pickNormalValue = false;
