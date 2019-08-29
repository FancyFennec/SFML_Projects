#pragma once

#include <vector>
#include <algorithm>
#include "Scene.h"

enum COMMAND_TYPE {
	CREATE_LAYER,
	DELETE_LAYER,
	UPDATE_LAYER
};

struct command {
	command(COMMAND_TYPE type, int layerPos, sf::Texture newTexure, sf::Texture oldTexure) :
		type(type),
		layerPos(layerPos),
		newTexure(newTexure),
		oldTexure(oldTexure) {}

	COMMAND_TYPE type;
	int layerPos;
	sf::Texture newTexure;
	sf::Texture oldTexure;
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
		actions.reserve(20);
	};

	static void createLayer(int layerPos);
	static void deleteLayer(int layerPos, sf::Texture& texture);
	static void updateLayer(int layerPos, sf::Texture& newTexture, sf::Texture& oldTexture);

	static void moveForward(Scene& scene);
	static void moveBackward(Scene& scene);
	static void clearActions();

	~CommandManager();

private:
	static std::vector<command> actions;
	static std::vector<command>::iterator actionIter;
};

std::vector<command> CommandManager::actions = { create_command(2) };
std::vector<command>::iterator CommandManager::actionIter = actions.begin();

inline void CommandManager::createLayer(int layerPos) {
	if (actions.size() == 20) {
		actions.erase(actions.begin());
	}
	actions.push_back(create_command(layerPos));
	std::advance(actionIter, 1);
}

inline void CommandManager::deleteLayer(int layerPos, sf::Texture& texture) {
	if (actions.size() == 20) {
		actions.erase(actions.begin());
	}
	actions.push_back(delete_command(layerPos, texture));
	std::advance(actionIter, 1);
}

inline void CommandManager::updateLayer(int layerPos, sf::Texture& newTexture, sf::Texture& oldTexture) {
	if (actions.size() == 20) {
		actions.erase(actions.begin());
	}
	actions.push_back(update_command(layerPos, newTexture, oldTexture));
	std::advance(actionIter, 1);
}

inline void CommandManager::moveForward(Scene& scene)
{
	if (std::next(actionIter) != actions.end()) {
		std::advance(actionIter, 1);
		switch (actionIter->type) {
		case(CREATE_LAYER): {
			std::advance(scene.lastActiveLayer, 1);
			scene.lastActiveLayer->clearLayer();
			break;
		}
		case(DELETE_LAYER): {
			auto iter = scene.layers.begin() + actionIter->layerPos;
			auto currentLayerOffset = std::distance(scene.layers.begin(), scene.currentLayer);
			scene.currentLayer = scene.layers.begin();

			if (std::prev(scene.lastActiveLayer) == scene.layers.begin()) { // Create a new Layer if There are none left
				scene.lastActiveLayer->clearLayer();
			}
			else { // This makes sure that we continue working on the layer we were before deleting
				bool isCurrentLayerBelowIter = scene.layers.begin() + currentLayerOffset < iter;
				iter->clearLayer();
				std::advance(scene.lastActiveLayer, -1);
				std::rotate(iter, iter + 1, scene.layers.end());

				if (isCurrentLayerBelowIter) {
					std::advance(scene.currentLayer, currentLayerOffset);
				}
				else {
					std::advance(scene.currentLayer, currentLayerOffset - 1);
				}
			}
			if (scene.currentLayer == scene.layers.begin()) std::advance(scene.currentLayer, 1);
			break;
		}
		case(UPDATE_LAYER): {
			auto iter = (scene.layers.begin() + actionIter->layerPos);
			iter->tex = actionIter->newTexure;
			iter->sprite.setTexture(iter->tex);
			break;
		}
		}
	}
}

inline void CommandManager::moveBackward(Scene& scene)
{
	if (actionIter != actions.begin()) {
		std::advance(actionIter, - 1);
		switch (actionIter->type) {
		case(CREATE_LAYER): {
			std::advance(scene.lastActiveLayer, -1);
			break;
		}
		case(DELETE_LAYER): {
			auto iter = scene.layers.begin() + actionIter->layerPos;
			auto currentLayerOffset = std::distance(scene.layers.begin(), scene.currentLayer);
			scene.currentLayer = scene.layers.begin();

			if (std::next(iter) == scene.layers.begin()) { // Create a new Layer if There are none left
				scene.lastActiveLayer->tex = actionIter->oldTexure;
				scene.lastActiveLayer->sprite.setTexture(scene.lastActiveLayer->tex);
			}
			else { // This makes sure that we continue working on the layer we were before deleting
				bool isCurrentLayerBelowIter = scene.layers.begin() + currentLayerOffset < iter;
				std::advance(scene.lastActiveLayer, 1);
				std::rotate(iter, scene.lastActiveLayer, std::next(scene.lastActiveLayer));
				iter->tex = actionIter->oldTexure;
				iter->sprite.setTexture(iter->tex);

				if (isCurrentLayerBelowIter) {
					std::advance(scene.currentLayer, currentLayerOffset);
				}
				else {
					std::advance(scene.currentLayer, currentLayerOffset + 1);
				}
			}
			if (scene.currentLayer == scene.layers.begin()) std::advance(scene.currentLayer, 1);
			break;
		}
		case(UPDATE_LAYER): {
			auto iter = (scene.layers.begin() + actionIter->layerPos);
			iter->tex = actionIter->oldTexure;
			iter->sprite.setTexture(iter->tex);
			break;
		}
		}
	}
}

inline void CommandManager::clearActions()
{
	if (std::next(actionIter) != actions.end()) {
		for (auto iter = std::next(actionIter); iter < actions.end(); std::advance(iter, 1)) {
			iter = actions.erase(iter);
		}
	}
}

CommandManager::~CommandManager()
{
}
