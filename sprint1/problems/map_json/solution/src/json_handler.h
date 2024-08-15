#pragma once
#include "model.h"
#include <string>


namespace jsonOperation {

	std::string GameToJson(const model::Game& game);			//Конвертер всего в строку
	std::string MapToJson(const model::Map& map);				//Конвертер мапы в строку

	std::string PageNotFound();									//Выдать, что страница не найдена
	std::string BadRequest();									//Выдать, что кривой запрос
	std::string MapNotFound();									//Выдать, что карту не нашел

}