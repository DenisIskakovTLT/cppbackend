#pragma once
#include "../application/application.h"
#include <string>


namespace jsonOperation {

	/*Тут для респосов*/
	const std::string RESPONSE_CODE = "code";
	const std::string RESPONSE_MESSAGE = "message";
	const std::string RESPONSE_AUTHTOKEN = "authToken";
	const std::string RESPONSE_PLAYERID = "playerId";
	const std::string RESPONSE_NAME = "name";

	/*Тут для реквестов*/
	const std::string REQUEST_NAME = "userName";
	const std::string REQUEST_MAPID = "mapId";

	std::string GameToJson(const model::Game::Maps& game);		//Конвертер всего в строку
	std::string MapToJson(const model::Map& map);				//Конвертер мапы в строку

	std::string PageNotFound();									//Выдать, что страница не найдена
	std::string BadRequest();									//Выдать, что кривой запрос
	std::string MapNotFound();									//Выдать, что карту не нашел

	
	std::string JoinToGameInvalidArgument();															//Передали кривые аргументы
	std::string JoinToGameMapNotFound();																//При заходе в игру не нашли карту
	std::string JoinToGameEmptyPlayerName();															//Задали пустое имя
	std::string PlayersListOnMap(const std::vector< std::weak_ptr<model::Player> >& players);			//Выдать списко игорьков
	std::string InvalidMethod();																		//Выдать invalid method
	std::string EmptyAuthorization();																	//Выдать, что никто не авторихвоан при запросе списка игорьков
	std::string UnknownToken();																			//Выдать, что кривой токен при запросе списка игорьков
	std::string JoinToGame(const std::string& token, size_t player_id);									//Выдать, что вошли в игру

	std::string OnlyPostMethodAllowed();															
	std::optional< std::tuple<std::string, model::Map::Id> > ParseJoinToGame(const std::string& msg);	//Распарсить реквест входа в игру

}