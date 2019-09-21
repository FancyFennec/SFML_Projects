#pragma once

#include <iostream>
#include <fstream>

#include "Imgui/imgui.h"
#include "Imgui/imgui-SFML.h"
#include "GlobalVariables.h"
#include "Scene.h"
#include "CommandManager.h"

void mainMenuGUI(Scene& scene);

void loadLayerFromFile(std::string &folderPath, std::string &fileName, Scene & scene);

void loadScene(std::string &fileName, Scene & scene);

void saveScene(Scene & scene, std::string &tmpPath, std::string &folderPath);

void saveCurrentLayerAsPNG(Scene & scene, std::string &folderPath);

void saveCurrentLayerAsJPEG(Scene & scene, std::string &folderPath);

void saveSceneAsPNG(Scene & scene, std::string &folderPath);

void saveSceneAsJPEG(Scene & scene);

bool saveFilePopupIsOpen = false;
bool openFilePopupIsOpen = false;
std::string input;

static enum FILE_TYPE {
	PNG,
	JPEG,
	SINGLE_PNG,
	SINGLE_JPEG,
	SCN
};

FILE_TYPE file_type;

void mainMenuGUI(Scene& scene)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Saving Scene"))
			{
				file_type = SCN;
				saveFilePopupIsOpen = true;
				input = "Input FileName and";
			}
			if (ImGui::MenuItem("Open Scene"))
			{
				file_type = SCN;
				openFilePopupIsOpen = true;
			}
			if (ImGui::MenuItem("Open Layer from File"))
			{
				file_type = PNG;
				openFilePopupIsOpen = true;
			}
			if (ImGui::MenuItem("Save Scene as JPEG"))
			{
				file_type = JPEG;
				saveFilePopupIsOpen = true;
				input = "Input FileName and";
			}
			if (ImGui::MenuItem("Save Scene as PNG"))
			{
				file_type = PNG;
				saveFilePopupIsOpen = true;
				input = "Input FileName and";
			}
			if (ImGui::MenuItem("Save Layer as JPEG"))
			{
				file_type = SINGLE_JPEG;
				saveFilePopupIsOpen = true;
				input = "Input FileName and";
			}
			if (ImGui::MenuItem("Save Layer as PNG"))
			{
				file_type = SINGLE_PNG;
				saveFilePopupIsOpen = true;
				input = "Input FileName and";
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("menu item"))
			{
				std::cout << "Menu item pressed" << std::endl;
			}
			ImGui::EndMenu();
		}

		if (ImGui::ArrowButton("##Back", ImGuiDir_Left))
		{
			CommandManager::moveBackward();
		}
		if (ImGui::ArrowButton("##Forward", ImGuiDir_Right))
		{
			CommandManager::moveForward();
		}
		if (ImGui::Checkbox("GUI", &SHOW_GUI)) {
		}

		ImGui::SameLine(WINDOW_WIDTH - 60);
		if (ImGui::Button("Close"))
		{
			mainWindow.close();
		}

		ImGui::EndMainMenuBar();

		if (saveFilePopupIsOpen) {
			ImGui::SetNextWindowSize(ImVec2(600, 500));
			ImGui::OpenPopup("Save File");
			if (ImGui::BeginPopupModal("Save File", &saveFilePopupIsOpen)) {
				std::string folderPath("./SavedFiles");
				std::string tmpPath("./SavedFiles/tmp");

				for (auto p : fs::directory_iterator(folderPath)) {
					std::string fileName = p.path().string().substr(folderPath.size() + 1); //Remove folder name
					if (!fs::is_directory(p)) {
						if (ImGui::Button(fileName.data(), ImVec2(200, 20))) {
							input = fileName.substr(0, fileName.length() - 4).c_str(); //remove ending
						}
					}
				}

				ImGui::Dummy(ImVec2(0, 10));
				ImGui::DrawLine(sf::Vector2f(0, 0), sf::Vector2f(WINDOW_WIDTH, 0), sf::Color::White, 1.0f);
				ImGui::Dummy(ImVec2(0, 20));

				char* inputChar = (char*)input.data();
				if (ImGui::InputText("Press Enter", inputChar, 30, ImGuiInputTextFlags_EnterReturnsTrue)) {
					folderPath.append("/").append(inputChar);
					switch (file_type) {
					case(JPEG):
					case(SINGLE_JPEG):
						folderPath.append(".jpg");
						break;
					case(PNG):
					case(SINGLE_PNG):
						folderPath.append(".png");
						break;
					case(SCN):
						tmpPath.append("/").append("tmp").append(".png");
						folderPath.append(".scn");
						break;
					}

					switch (file_type) {
					case(JPEG): {
						saveSceneAsJPEG(scene);
						break;
					}
					case(PNG): {
						saveSceneAsPNG(scene, folderPath);
						break;
					}
					case(SINGLE_JPEG): {
						saveCurrentLayerAsJPEG(scene, folderPath);
						break;
					}
					case(SINGLE_PNG): {
						saveCurrentLayerAsPNG(scene, folderPath);
						break;
					}
					case(SCN): {
						saveScene(scene, tmpPath, folderPath);
					}
					}

					saveFilePopupIsOpen = false;
				}
				ImGui::Dummy(ImVec2(0, 30));
				ImGui::SameLine(ImGui::GetWindowSize().x - 60);
				if (ImGui::Button("Close", ImVec2(50, 20))) saveFilePopupIsOpen = false;
				ImGui::EndPopup();
			}
		}

		if (openFilePopupIsOpen) {
			ImGui::SetNextWindowSize(ImVec2(600, 500));
			ImGui::OpenPopup("Open File");
			if (ImGui::BeginPopupModal("Open File", &openFilePopupIsOpen)) {
				std::string folderPath("./SavedFiles");
				std::string tmpPath("./SavedFiles/tmp");

				for (auto p : fs::directory_iterator(folderPath)) {
					std::string fileName = p.path().string().substr(folderPath.size() + 1); //Remove folder name

					switch(file_type){
					case(SCN): {
						if (!fs::is_directory(p) && fileName.substr(fileName.size() - 4) == ".scn") {
							if (ImGui::Button(fileName.data(), ImVec2(200, 20))) {
								std::string path = folderPath;
								loadScene(path.append("/").append(fileName), scene);
								//TODO: reset the commands
								openFilePopupIsOpen = false;
							}
						}
						break;
					}
					case(PNG): {
						if (!fs::is_directory(p) && fileName.substr(fileName.size() - 4) != ".scn") {
							if (ImGui::Button(fileName.data(), ImVec2(200, 20))) {
								loadLayerFromFile(folderPath, fileName, scene);
								openFilePopupIsOpen = false;
							}
						}
						break;
					}
					}
					
				}
				ImGui::Dummy(ImVec2(0, 30));
				ImGui::SameLine(ImGui::GetWindowSize().x - 60);
				if (ImGui::Button("Close", ImVec2(50, 20))) openFilePopupIsOpen = false;
				ImGui::EndPopup();
			}
		}
	}
}

void loadLayerFromFile(std::string &folderPath, std::string &fileName, Scene & scene)
{
	std::string path = folderPath;
	path.append("/").append(fileName);

	sf::Texture tex;
	tex.loadFromFile(path);

	if (scene.lastActiveLayer < scene.layers.end()) {
		CommandManager::createLayer();
	}
	else {
		std::cout << "ERROR! Maxing number of Layers reached!!!" << std::endl;
	}
	scene.lastActiveLayer->layerName = "Layer";
	scene.lastActiveLayer->layerName.append(std::to_string(scene.getSize()));

	scene.lastActiveLayer->tex = tex;
	scene.lastActiveLayer->sprite.setTexture(scene.lastActiveLayer->tex);
}

void loadScene(std::string &fileName, Scene & scene)
{
	//reading the file back into an image
	std::ifstream newistrm(fileName, std::ios::out | std::ios::binary);

	int layerCount;
	newistrm.read((char*)&layerCount, sizeof(int));
	int width;
	newistrm.read((char*)&width, sizeof(int));
	int height;
	newistrm.read((char*)&height, sizeof(int));

	scene.width = width;
	scene.height = height;

	scene.lastActiveLayer = scene.layers.begin() + layerCount;

	for (auto layer : scene.layers) {
		layer.width = width;
		layer.height = height;
		layer.clearLayer();
	}

	std::streampos begin = newistrm.tellg();
	newistrm.seekg(0, std::ios::end);
	std::streampos end = newistrm.tellg();
	newistrm.seekg(begin);

	std::vector<char> buffer;
	buffer.resize((end - begin) / sizeof(char));
	newistrm.read((char*)buffer.data(), (end - begin) / sizeof(char));

	newistrm.close();

	//Load the data into a texture
	sf::Texture tex;
	tex.create(width, height);
	tex.loadFromMemory(buffer.data(), width * height * layerCount);

	//create a render texture with the right size and draw portions of the texture to it
	sf::RenderTexture rtex;
	sf::RenderStates rState;
	rState.blendMode = sf::BlendNone;
	rtex.create(width, height);
	sf::IntRect rect(0, 0, width, height);
	sf::Sprite sprite;

	for (int i = 0; i < layerCount; i++) {
		rect.left = i * width;
		sprite.setTexture(tex);
		sprite.setTextureRect(rect);
		rtex.draw(sprite, rState);
		rtex.display();

		scene.layers[i + 1].tex = rtex.getTexture();
		scene.layers[i + 1].sprite.setTexture(scene.layers[i + 1].tex);
	}
}

void saveScene(Scene & scene, std::string &tmpPath, std::string &folderPath)
{
	//Draw all layers to a big PNG
	unsigned int layerCount = scene.getSize();

	sf::Vector2f offset(scene.width, 0);
	sf::RenderStates rState;
	rState.blendMode = sf::BlendNone;

	sf::RenderTexture rTex;
	rTex.create(scene.width * layerCount, scene.height);
	rTex.clear(sf::Color(255, 255, 255, 0));

	for (int i = 0; i < layerCount; i++) {
		rTex.draw(scene.layers[i + 1].sprite, rState);
		rState.transform.translate(offset);
	}

	rTex.display();
	rTex.getTexture().copyToImage().saveToFile(tmpPath.data());

	//Create input stream for the ng and output stream for the file we want to write to
	std::ifstream istrm(tmpPath.data(), std::ios::in | std::ios::binary);
	std::ofstream ostrm(folderPath.data(), std::ios::out | std::ios::binary);

	ostrm.write((char*)&layerCount, sizeof(int));
	ostrm.write((char*)&scene.width, sizeof(int));
	ostrm.write((char*)&scene.height, sizeof(int));

	istrm.seekg(0, std::ios::end);
	std::streampos end = istrm.tellg();
	istrm.seekg(0, std::ios::beg);

	//Make buffer with the right size
	std::vector<char> buffer;
	buffer.resize(end / sizeof(char));

	//read data into the buffer and write it to the file again
	istrm.read((char*)buffer.data(), end / sizeof(char));
	ostrm.write((char*)buffer.data(), end / sizeof(char));

	istrm.close();
	ostrm.close();
}

void saveCurrentLayerAsPNG(Scene & scene, std::string &folderPath)
{
	scene.currentLayer->tex.copyToImage().saveToFile(folderPath.data());
}

void saveCurrentLayerAsJPEG(Scene & scene, std::string &folderPath)
{
	sf::RenderTexture rTex;
	rTex.create(scene.width, scene.height);
	rTex.clear(sf::Color::White);
	rTex.draw(scene.layers.begin()->sprite);
	rTex.draw(scene.currentLayer->sprite);
	rTex.display();
	rTex.getTexture().copyToImage().saveToFile(folderPath.data());
}

void saveSceneAsPNG(Scene & scene, std::string &folderPath)
{
	sf::RenderTexture rTex;
	rTex.create(scene.width, scene.height);
	rTex.clear(sf::Color(255, 255, 255, 0));
	for (auto layer = std::next(scene.layers.begin()); layer != scene.layers.end(); layer++) {
		rTex.draw(layer->sprite);
	}
	rTex.display();
	rTex.getTexture().copyToImage().saveToFile(folderPath.data());
}

void saveSceneAsJPEG(Scene & scene)
{
	sf::RenderTexture rTex;
	rTex.create(scene.width, scene.height);
	rTex.clear(sf::Color::White);
	for (auto layer = scene.layers.begin(); layer != scene.layers.end(); layer++) {
		rTex.draw(layer->sprite);
	}
}
