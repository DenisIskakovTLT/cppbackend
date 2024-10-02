#pragma once
#include "road.h"
#include "typedef.h"
#include "utils.h"

#include <map>
#include <optional>
#include <tuple>
#include <unordered_set>

namespace model {


	//Класс который описывает все дороги имеющиеся на карте.
	//Всю карту дробим на секторы, и в каждом храним дороги, которые через него проходят. 
	class RoadLayout {					
	public:
		using Roads = std::vector<Road>;

		/*Конструкторы все дефорлтные*/
		RoadLayout() = default;
		RoadLayout(const RoadLayout& other);
		RoadLayout(RoadLayout&& other) noexcept;
		RoadLayout& operator = (const RoadLayout& other);
		RoadLayout& operator = (RoadLayout&& other);
		virtual ~RoadLayout() = default;

		void AddRoad(const Road& road);											//добавить дорогу

		const Roads& GetRoads() const noexcept;									//Геттер


		std::tuple<geom::Point2D, Speed> GetValidMove(const geom::Point2D& current_pos,	//Сам ход
			const geom::Point2D& target_pos,
			const Speed& current_spd);

		/*генератор рандомной позиции*/
		geom::Point2D GenerateRndPosition() const;

	private:

		struct MapCoord {
			int64_t x;
			int64_t y;

			bool operator == (const MapCoord& other) const {
				return (x == other.x) && (y == other.y);
			};


		};

		using MatrixMap = std::map< int64_t, std::map<int64_t, std::unordered_set<size_t> > >;
		MatrixMap matrixMap_;																		//Карта дорог
		Roads roads_;																				//Вектор дорог

		/*Получение структуры MapCoord*/
		std::optional<const MapCoord> GetDestinationRoadsOfRoute(
			std::optional<const MapCoord> start,
			std::optional<const MapCoord> end,
			const Speed& current_spd);

		/*Получение координат из позиции*/
		std::optional<const MapCoord> GetCoordinatesOfPosition(const geom::Point2D& pos);

		/*Полчение направления (север юг итд)*/
		const Direction SpeedToDirection(const Speed& speed);

		/*Получение координат сектора*/
		const std::unordered_map<Direction, geom::Point2D> MatrixCoordinateToPosition(const MapCoord& coord,
			const geom::Point2D& target_pos);

		/*Проверка пересечения множеств*/
		bool IsCrossedSets(const std::unordered_set<size_t>& lhs,
			const std::unordered_set<size_t>& rhs);

		/*Проверка координат на валиднойть*/
		bool IsValidCoordinates(const MapCoord& coord);

		/*Поиск самой удалённой точки*/
		const geom::Point2D GetFarthestPoinOfRoute(const MapCoord& roads_coord,
			const geom::Point2D& current_pos,
			const Speed& current_spd);

		/*Валидация позиции*/
		bool IsValidPosition(const std::unordered_set<size_t>& roads, const geom::Point2D& pos);

		/*Валидация позиции относительно дороги*/
		bool IsValidPositionOnRoad(const Road& road, const geom::Point2D& pos);

		/*Копирование через метод AddRoad*/
		void CopyAll(const Roads& roads);



	};

}//namespace model