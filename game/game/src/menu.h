#pragma once

#include <vector>
#include <string>
#include <SDL.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include "ScreenText.h"
#include <fstream>
#include "json.hpp"
using json = nlohmann::json;

using namespace std;
class Menu
{
public:
	//Options
	bool o_fullscreen = true;
	float o_audioVolume = 0.9f;
	int o_difficulty = 1;
	bool o_2Player = false;
	bool o_gameHud = true;
	bool o_gameLog = false;
	bool o_gameLatency = false;
	bool o_pausing = true;
	bool o_serialiseLevel = true;
	string message = "";
	int category = 0;

	//Menu state
	int mode = 0;
	int selected = 0;
	bool paused = false;
	SDL_Window* window;
	SDL_Color c_selected{ 255,255,255 };
	SDL_Color c_off{ 0,255,255 };
	SDL_Color c_hud{ 255,255,0 };
	SDL_Color c_alert{ 255, 0, 0};
	SDL_Color c_warn{ 255, 128, 0 };

	//Add a font to vector of fonts
	void AddFont(string filename, string defaultText, SDL_Color colour, int x, int y, int size)
	{
		ScreenText* font = new ScreenText(window, filename);
		font->setColor(colour);
		font->setText(defaultText, x, y, size);
		fonts.push_back(font);
	}

	//Get if the mouse is in text (for menu)
	bool GetMouseInText(int id, int mx, int my)
	{
		ScreenText* font = fonts[id];
		int width, height;
		width = font->text.length()*32;
		height = 32;
		return (mx >= font->x && my >= font->y && mx <= font->x + width && my <= font->y + height);
	}

	//Updates certain text at a time
	void UpdateText(vector<int> textIds, vector<string> strings)
	{

		unsigned int s = 0;
		for (int i = 0; i < textIds.size(); i++)
		{
			ScreenText* font = fonts[textIds[i]];
			switch (textIds[i])
			{
				//score etc.
			case 26:
			{
				font->setColor(c_warn);
				if (category==0)
					font->setColor(c_alert);
			}
			case 5:
			case 6:
			case 7:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
			case 21:
			case 22:
			case 23:
			case 24:
			case 25:
			
			{
				if (strings[s] != "selected" && strings[s] != "off")
				font->setText(strings[s++]);
			}
			case 1:
			case 2:
			case 3:
			case 4:
			case 8:
			
			{
				if (strings[0] == "selected") font->setColor(c_selected);
				else if (strings[0] == "off") font->setColor(c_off);
				break;
			}
			

			}

		}
	}

	//Displays certain text at a time
	void DisplayText(vector<int> textIds)
	{
		for (int i = 0; i < textIds.size(); i++)
		{
			fonts[textIds[i]]->render();
		}
	}

	//Serialisation (save)
	void Serialise()
	{
		//The output stream (to a file)
		std::ofstream outStream;
		//Open the file for writing
		outStream.open(fname, std::ios::out);
		if (outStream.fail())
		{
			SDL_Log("Could not find %s for serialisation.", fname);
		}
		else
		{
			SDL_Log("Successfully opened options file %s for serialisation.",fname);
			//Create an empty json object that we're going to save
			json outObject;
			//Set up fields to save
			outObject["options"]["fullscreen"] = this->o_fullscreen;
			outObject["options"]["audioVolume"] = this->o_audioVolume;
			outObject["options"]["difficulty"] = this->o_difficulty;
			outObject["options"]["2Player"] = this->o_2Player;
			outObject["options"]["gameHud"] = this->o_gameHud;
			outObject["options"]["gameLog"] = this->o_gameLog;
			outObject["options"]["gameLatency"] = this->o_gameLatency;
			outObject["options"]["pausing"] = this->o_pausing;
			outObject["options"]["serialiseLevel"] = this->o_serialiseLevel;
			//Write to the file -- use dump to get the JSON text
			SDL_Log("\nFullsceeen: %i\nAudio Volume: %f\nDifficulty: %i", o_fullscreen, o_audioVolume, o_difficulty);
			outStream << outObject.dump(3);

			//And close the file! Very important!
			SDL_Log("Closed %s", fname);
			outStream.close();
		}
	}

	//De-serialise (load)
	void Deserialise()
	{
		//The input stream (to a file)
		std::ifstream inStream;
		//Open the file for reading
		inStream.open(fname, std::ios::in);
		if (inStream.fail())
		{
			SDL_Log("Could not open options file %s for deserialisation.", fname);
			SDL_Log("Setting defaults for options...");
			this->o_fullscreen = false;
			this->o_audioVolume =1.0;
			this->o_difficulty = 0;
		}
		else
		{
			SDL_Log("Successfully opened options file %s for deserialisation.", fname);
			//Build an empty JSON object that we're going to save into
			json inObject;
			//Read all JSON data into this object..
			//json.hpp will do the parsing!
			inStream >> inObject;
			//Now that it's parsed, set the instance variables of the
			//player to whatever is in the parsed JSON object
			SDL_Log("Reading in options...");
			this->o_fullscreen = inObject["options"]["fullscreen"].get<bool>();
			this->o_audioVolume = inObject["options"]["audioVolume"].get<float>();
			this->o_difficulty = inObject["options"]["difficulty"].get<int>();
			this->o_2Player = inObject["options"]["2Player"].get<bool>();
			this->o_gameHud = inObject["options"]["gameHud"].get<bool>();
			this->o_gameLog = inObject["options"]["gameLog"].get<bool>();
			this->o_gameLatency = inObject["options"]["gameLatency"].get<bool>();
			this->o_pausing = inObject["options"]["pausing"].get<bool>();
			this->o_serialiseLevel = inObject["options"]["serialiseLevel"].get<bool>();
			SDL_Log("\nFullsceeen: %i\nAudio Volume: %f\nDifficulty: %i",o_fullscreen,o_audioVolume,o_difficulty);
			//And close the file
			SDL_Log("Closed %s.", fname);
			inStream.close();
		}
	}

private:
	vector<ScreenText*> fonts;
	const char* fname = "../content/config/options.json";
};