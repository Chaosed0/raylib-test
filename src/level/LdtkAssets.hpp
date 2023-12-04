#include "raylib-cpp/raylib-cpp.hpp"
#include "ldtkimport/LdtkDefFile.h"
#include "ldtkimport/Level.h"

struct TileSetImage
{
	std::shared_ptr<raylib::Texture> tex;
	std::unordered_map<ldtkimport::tileid_t, raylib::Rectangle> tiles;

	TileSetImage()
	{
		tex = std::make_shared<raylib::Texture>();
	}
};

struct LdtkAssets
{
	ldtkimport::LdtkDefFile ldtk;
	std::unordered_map<ldtkimport::uid_t, ::TileSetImage> tilesetImages;

	bool load(std::string filename)
	{
		ldtk.loadFromFile(filename.c_str(), false);

		for (auto tileset = ldtk.tilesetBegin(), end = ldtk.tilesetEnd(); tileset != end; ++tileset)
		{
			if (tileset->imagePath.empty())
			{
				continue;
			}

			std::string imagePath = "assets/level/" + tileset->imagePath;
			::TileSetImage tileSetImage;
			tileSetImage.tex->Load(imagePath);
			tileSetImage.tex->SetFilter(TEXTURE_FILTER_POINT);

			ldtkimport::uid_t uid = tileset->uid;
			auto pair = std::make_pair(uid, tileSetImage);
			tilesetImages.insert(pair);
		}

		for (auto layer = ldtk.layerBegin(), end = ldtk.layerEnd(); layer != end; ++layer)
		{
			const ldtkimport::TileSet* tileset = ldtk.getTileset(layer->tilesetDefUid);

			if (tileset == nullptr)
			{
				std::cerr << "TileSet " << layer->tilesetDefUid << " was not found in ldtk file" << std::endl;
				continue;
			}

			if (tilesetImages.count(tileset->uid) == 0)
			{
				std::cerr << "TileSet " << tileset->uid << " was not found in tilesetImages" << std::endl;
				continue;
			}

			auto& tilesetImage = tilesetImages[tileset->uid];
			const float cellPixelSize = layer->cellPixelSize;

			for (auto ruleGroup : layer->ruleGroups)
			{
				for (auto rule : ruleGroup.rules)
				{
					for (auto tile : rule.tileIds)
					{
						if (tilesetImage.tiles.count(tile) > 0)
						{
							continue;
						}

						int16_t tileX, tileY;
						tileset->getCoordinates(tile, tileX, tileY);
						raylib::Rectangle rect(tileX * cellPixelSize, tileY * cellPixelSize, cellPixelSize, cellPixelSize);
						tilesetImage.tiles.insert(std::make_pair(tile, rect));
					}
				}
			}
		}

		return true;
	}

	void Draw(raylib::Vector2 pos, float cellSize, const ldtkimport::Level& level)
	{
		auto cellCountX = level.getWidth();
		auto cellCountY = level.getHeight();

		for (size_t layerNum = ldtk.getLayerCount(); layerNum > 0; --layerNum)
		{
			const auto& layer = ldtk.getLayerByIdx((int)layerNum - 1);
			const auto& tileGrid = level.getTileGridByIdx((int)layerNum - 1);

			ldtkimport::TileSet* tileset = ldtk.getTileset(layer.tilesetDefUid);
			if (tileset == nullptr)
			{
				continue;
			}

			if (tilesetImages.count(tileset->uid) == 0)
			{
				continue;
			}

			auto& tilesetImage = tilesetImages[tileset->uid];

			for (int cellY = 0; cellY < cellCountY; ++cellY)
			{
				for (int cellX = 0; cellX < cellCountX; ++cellX)
				{
					// these are the tiles in this cell
					auto& tiles = tileGrid(cellX, cellY);

					for (auto tile = tiles.crbegin(), tileEnd = tiles.crend(); tile != tileEnd; ++tile)
					{
						DrawSingleTile(tilesetImage, *tile, cellSize, pos, cellX, cellY);
					}

				}
			}
		}
	}

	void DrawTiles(TileSetImage& tilesetImage, const ldtkimport::tiles_t* tilesToDraw, uint8_t idxToStartDrawing, float cellSize, raylib::Vector2 pos, int cellX, int cellY)
	{
		for (int tileIdx = idxToStartDrawing; tileIdx >= 0; --tileIdx)
		{
			const auto& tile = (*tilesToDraw)[tileIdx];
			DrawSingleTile(tilesetImage, tile, cellSize, pos, cellX, cellY);
		}
	}

	void DrawSingleTile(const TileSetImage& tilesetImage, const ldtkimport::TileInCell& tile, float cellSize, raylib::Vector2 pos, int cellX, int cellY)
	{
		float offsetX = tile.getOffsetX(cellSize * 0.5f);
		float offsetY = tile.getOffsetY(cellSize * 0.5f);
		float scaleX;
		float scaleY;
		float pivotX;
		float pivotY;

		if (tile.isFlippedX())
		{
			scaleX = -1;
			pivotX = cellSize;
		}
		else
		{
			scaleX = 1;
			pivotX = 0;
		}

		if (tile.isFlippedY())
		{
			scaleY = -1;
			pivotY = cellSize;
		}
		else
		{
			scaleY = 1;
			pivotY = 0;
		}

		raylib::Rectangle spriteRec = tilesetImage.tiles.at(tile.tileId);
		raylib::Rectangle destRec = raylib::Rectangle(pos.x + cellX * cellSize + offsetX, pos.y + cellY * cellSize + offsetY, cellSize, cellSize);
		tilesetImage.tex->Draw(spriteRec, destRec, raylib::Vector2(0,0), 0, WHITE);
	}
};
