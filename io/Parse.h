/*
	GameEngine header - Parse.h
	Utility functions for parsing things
	Copyright (c) 2014 Mark Hutcheson
*/
#pragma once

#include <string>

//--------------------------------------------------
// Parsing functions
//--------------------------------------------------
namespace Parse
{
	//Trim whitespace from beginning & end of string
	std::string trim(std::string s);

	//Strip all whitespace from s
	std::string removeWhitespace(std::string s);

	//Strip all of "from" from s, leaving "to" in their place
	std::string replaceWith(std::string s, char from, char to);

	//Shorthand for replaceWith(s, ',', ' ')
	std::string stripCommas(std::string s);

	//Get the file extension (not including the dot) from a filename, or empty string if there is none
	// For example: if filename is "image.png", then getExtension(filename) will be "png"
	std::string getExtension(std::string filename);
}

