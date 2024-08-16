#pragma once
#include "model.h"
#include <string>


namespace jsonOperation {

	std::string GameToJson(const model::Game& game);			//��������� ����� � ������
	std::string MapToJson(const model::Map& map);				//��������� ���� � ������

	std::string PageNotFound();									//������, ��� �������� �� �������
	std::string BadRequest();									//������, ��� ������ ������
	std::string MapNotFound();									//������, ��� ����� �� �����

}