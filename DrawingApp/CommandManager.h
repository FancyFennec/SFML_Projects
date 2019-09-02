#pragma once

#include <vector>
#include <algorithm>
#include "Scene.h"
#include "Settings.h"

enum COMMAND_TYPE {
	CREATE_LAYER,
	DELETE_LAYER,
	UPDATE_LAYER
};

struct command {
	command(COMMAND_TYPE type, int layerPos, sf::Texture newTexure, sf::Texture oldTexure) :
		type(type),
		layerPos(layerPos),
		newTexure(newTexure.copyToImage()),
		oldTexure(oldTexure.copyToImage()) {}

	COMMAND_TYPE type;
	int layerPos;
	sf::Image newTexure;
	sf::Image oldTexure;
};

struct create_command : command{
	create_command(int layerPos) :
		command(CREATE_LAYER, layerPos, sf::Texture(), sf::Texture()) {}
};

struct update_command : command {
	update_command(int layerPos, sf::Texture newTexure, sf::Texture oldTexure) :
		command(UPDATE_LAYER, layerPos, newTexure, oldTexure) {}
};

struct delete_command : command {
	delete_command(int layerPos, sf::Texture texture) :
		command(DELETE_LAYER, layerPos, sf::Texture(), texture) {}
};

class CommandManager
{
public:
	CommandManager() {
	};

	static void createLayer(int layerPos);
	static void deleteLayer(int layerPos, sf::Texture& texture);
	static void updateLayer(int layerPos, sf::Texture& newTexture, sf::Texture& oldTexture);

	static void moveForward();
	static void moveBackward();
	static void clearActions();

	static void initialize(Scene& scn) {
		scene = &scn;
		actions.reserve(MAX_ACTIONS); // Reserve memory for 20 actions
		actionIter = std::prev(actions.end());
	}

	~CommandManager();

private:
	static Scene* scene;
	static std::vector<command> actions;
	static std::vector<command>::iterator actionIter;

	static void delteLayerAt(int layerPos);
};
Scene* CommandManager::scene = nullptr;
std::vector<command> CommandManager::actions = { create_command(1) }; // We create a layer when we create the scene
std::vector<command>::iterator CommandManager::actionIter = actions.begin();

inline void CommandManager::createLayer(int layerPos) {
	if (actions.size() == MAX_ACTIONS) {
		actions.erase(actions.begin());
	}
	actions.push_back(create_command(layerPos));
	actionIter = std::prev(actions.end());

	std::advance(scene->lastActiveLayer, 1);
	scene->lastActiveLayer->clearLayer();
}

inline void CommandManager::deleteLayer(int layerPos, sf::Texture& texture) {
	if (actions.size() == MAX_ACTIONS) {
		actions.erase(actions.begin());
	}
	actions.push_back(delete_command(layerPos, texture));
	actionIter = std::prev(actions.end());

	delteLayerAt(layerPos);
}

inline void CommandManager::delteLayerAt(int layerPos)
{
	auto iter = scene->layers.begin() + layerPos;
	auto iterDist = std::distance(scene->layers.begin(), scene->currentLayer);
	scene->currentLayer = scene->layers.begin();

	if (std::prev(scene->lastActiveLayer) == scene->layers.begin()) { // Create a new Layer if There are none left
		scene->lastActiveLayer->clearLayer();

	}
	else { // This makes sure that we continue working on the layer we were before deleting
		bool isCurrentLayerBelowIter = scene->layers.begin() + iterDist < iter;
		iter->clearLayer();
		std::advance(scene->lastActiveLayer, -1);
		std::rotate(iter, iter + 1, scene->layers.end());

		if (isCurrentLayerBelowIter) {
			std::advance(scene->currentLayer, iterDist);

		}
		else {
			std::advance(scene->currentLayer, iterDist - 1);

		}

	}
	if (scene->currentLayer == scene->layers.begin()) std::advance(scene->currentLayer, 1);
}

inline void CommandManager::updateLayer(int layerPos, sf::Texture& newTexture, sf::Texture& oldTexture) {
	if (actions.size() == MAX_ACTIONS) {
		actions.erase(actions.begin());
	}
	actions.push_back(update_command(layerPos, newTexture, oldTexture));
	actionIter = std::prev(actions.end());
}

inline void CommandManager::moveForward()
{
	if (std::next(actionIter) != actions.end()) {
		std::advance(actionIter, 1);
		switch (actionIter->type) {
		case(CREATE_LAYER): {
			std::advance(scene->lastActiveLayer, 1);
			scene->lastActiveLayer->clearLayer();
			break;
		}
		case(DELETE_LAYER): {
			auto iter = scene->layers.begin() + actionIter->layerPos;
			auto currentLayerOffset = std::distance(scene->layers.begin(), scene->currentLayer);
			scene->currentLayer = scene->layers.begin();

			if (std::prev(scene->lastActiveLayer) == scene->layers.begin()) { // Create a new Layer if There are none left
				scene->lastActiveLayer->clearLayer();
			}
			else { // This makes sure that we continue working on the layer we were before deleting
				bool isCurrentLayerBelowIter = scene->layers.begin() + currentLayerOffset < iter;
				iter->clearLayer();
				std::advance(scene->lastActiveLayer, -1);
				std::rotate(iter, iter + 1, scene->layers.end());

				if (isCurrentLayerBelowIter) {
					std::advance(scene->currentLayer, currentLayerOffset);
				}
				else {
					std::advance(scene->currentLayer, currentLayerOffset - 1);
				}
			}
			if (scene->currentLayer == scene->layers.begin()) std::advance(scene->currentLayer, 1);
			break;
		}
		case(UPDATE_LAYER): {
			auto iter = (scene->layers.begin() + actionIter->layerPos);
			iter->tex.update(actionIter->newTexure);
			iter->sprite.setTexture(iter->tex);
			break;
		}
		}
	}
}

inline void CommandManager::moveBackward()
{
	if (actionIter != actions.begin()) {
		switch (actionIter->type) {
		case(CREATE_LAYER): {
			if (actionIter->layerPos != 1) {
				std::advance(scene->lastActiveLayer, -1);
			}
			break;
		}
		case(DELETE_LAYER): {
			auto iter = scene->layers.begin() + actionIter->layerPos;
			auto currentLayerOffset = std::distance(scene->layers.begin(), scene->currentLayer);
			scene->currentLayer = scene->layers.begin();

			if (std::next(iter) == scene->layers.begin()) { // Create a new Layer if There are none left
				scene->lastActiveLayer->tex.update(actionIter->oldTexure);
				scene->lastActiveLayer->sprite.setTexture(scene->lastActiveLayer->tex);
			}
			else { // This makes sure that we continue working on the layer we were before deleting
				bool isCurrentLayerBelowIter = scene->layers.begin() + currentLayerOffset < iter;
				std::advance(scene->lastActiveLayer, 1);
				std::rotate(iter, scene->lastActiveLayer, std::next(scene->lastActiveLayer));
				iter->tex.update(actionIter->oldTexure);
				iter->sprite.setTexture(iter->tex);

				if (isCurrentLayerBelowIter) {
					std::advance(scene->currentLayer, currentLayerOffset);
				}
				else {
					std::advance(scene->currentLayer, currentLayerOffset + 1);
				}
			}
			if (scene->currentLayer == scene->layers.begin()) std::advance(scene->currentLayer, 1);
			break;
		}
		case(UPDATE_LAYER): {
			auto iter = (scene->layers.begin() + actionIter->layerPos);
			iter->tex.update(actionIter->oldTexure);
			iter->sprite.setTexture(iter->tex);
			break;
		}
		}
		std::advance(actionIter, -1);
	}
}

inline void CommandManager::clearActions()
{
	if (std::next(actionIter) != actions.end()) {
		auto iter = std::prev(actions.end());
		while (iter != actionIter) {
			actions.erase(iter);
			iter = std::prev(actions.end());
		}
	}
	
	actionIter = std::prev(actions.end());
}

CommandManager::~CommandManager()
{
}
