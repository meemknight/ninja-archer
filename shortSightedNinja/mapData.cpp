#include "mapData.h"
#include <string>
#include "MapRenderer.h"

void MapData::create(int w, int h, const char* d = 0)
{
	this->w = w;
	this->h = h;

	data = new BlockInfo[w * h];

	if(d)
	{
		for(int i=0; i<w*h; i++)
		{
			data[i].type = d[i];
			data[i].mainColor = glm::vec4(1, 1, 1, 1);

		}
	}

	setNeighbors();
	//createEdges();
	//todo setup neighbours
}

BlockInfo & MapData::get(int x, int y)
{
	return data[x + this->w * y];
}

void MapData::clearColorData()
{
	if(data)
	{
		for (int i = 0; i < w*h; i++)
		{
			data[i].mainColor = { 0,0,0,0 };
		}
	}
}

//void MapData::createEdges()
//{
//	edges.clear();
//	edges.reserve((w*h) /2);
//
//
//	
//}

void MapData::cleanup()
{
	if(data)
	{
		delete[] data;
	}
}

void BlockInfo::resetColors()
{
	mainColor = { 0,0,0,0 };
}

void MapData::setNeighbors()
{
}

void MapData::ConvertTileMapToPolyMap()
{
	vecEdges.clear();

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			for (int j = 0; j < 4; j++)
			{
				get(y, x).edgeExist[j] = false;
				get(y, x).edgeId[j] = 0;
			}
		}
	}

	//Iterate through region from top left to bottom right
	for (int x = 1; x < w - 1; x++)
	{
		for (int y = 1; y < h - 1; y++)
		{
			//Create some convenient indices
			const glm::vec2 i = { x, y };				//This
			const glm::vec2 nn = { x, y - 1 };		//Northern Neighbor
			const glm::vec2 sn = { x ,y + 1 };		//Southern Neighbor
			const glm::vec2 wn = { x - 1, y };			//Western Neighbor
			const glm::vec2 en = { x + 1, y };			//Eastern Neighbor

			if (isOpaque(get(i.x, i.y).type))
			{
				//no wn, needs western edge
				if (!isOpaque(get(wn.x, wn.y).type))
				{
					// extend from nn or start a new one
					if (get(nn.x, nn.y).edgeExist[WEST])
					{
						//nn has a western edge, so grow downwards
						vecEdges[get(nn.x, nn.y).edgeId[WEST]].ey += BLOCK_SIZE;
						get(i.x, i.y).edgeId[WEST] = get(nn.x, nn.y).edgeId[WEST];
						get(i.x, i.y).edgeExist[WEST] = true;
					}
					else
					{
						Edge edge;
						edge.sx = x * BLOCK_SIZE;
						edge.sy = y * BLOCK_SIZE;

						edge.ex = edge.sx;
						edge.ey = edge.sy + BLOCK_SIZE;

						//Add edge to Polygon Pool
						int edgeId = vecEdges.size();
						vecEdges.push_back(edge);

						//Update tile information with edge information
						get(i.x, i.y).edgeId[WEST] = edgeId;
						get(i.x, i.y).edgeExist[WEST] = true;
					}
				}

				//no en, needs eastern edge
				if (!isOpaque(get(en.x, en.y).type))
				{
					// extend from nn or start a new one
					if (get(nn.x, nn.y).edgeExist[EAST])
					{
						vecEdges[get(nn.x, nn.y).edgeId[EAST]].ey += BLOCK_SIZE;
						get(i.x, i.y).edgeId[EAST] = get(nn.x, nn.y).edgeId[EAST];
						get(i.x, i.y).edgeExist[EAST] = true;
					}
					else
					{
						Edge edge;
						edge.sx = (x + 1.0) * BLOCK_SIZE;
						edge.sy = y * BLOCK_SIZE;

						edge.ex = edge.sx;
						edge.ey = edge.sy + BLOCK_SIZE;

						//Add edge to Polygon Pool
						int edgeId = vecEdges.size();
						vecEdges.push_back(edge);

						//Update tile information with edge information
						get(i.x, i.y).edgeId[EAST] = edgeId;
						get(i.x, i.y).edgeExist[EAST] = true;
					}
				}

				//no nn, needs northern edge
				if (!isOpaque(get(nn.x, nn.y).type))
				{
					// extend from wn or start a new one
					if (get(wn.x, wn.y).edgeExist[NORTH])
					{
						vecEdges[get(wn.x, wn.y).edgeId[NORTH]].ex += BLOCK_SIZE;
						get(i.x, i.y).edgeId[NORTH] = get(wn.x, wn.y).edgeId[NORTH];
						get(i.x, i.y).edgeExist[NORTH] = true;
					}
					else
					{
						Edge edge;
						edge.sx = x * BLOCK_SIZE;
						edge.sy = y * BLOCK_SIZE;

						edge.ex = edge.sx + BLOCK_SIZE;
						edge.ey = edge.sy;

						//Add edge to Polygon Pool
						int edgeId = vecEdges.size();
						vecEdges.push_back(edge);

						//Update tile information with edge information
						get(i.x, i.y).edgeId[NORTH] = edgeId;
						get(i.x, i.y).edgeExist[NORTH] = true;
					}
				}

				//no sn, needs southern edge
				if (!isOpaque(get(sn.x, sn.y).type))
				{
					// extend from wn or start a new one
					if (get(wn.x, wn.y).edgeExist[SOUTH])
					{
						vecEdges[get(wn.x, wn.y).edgeId[SOUTH]].ex += BLOCK_SIZE;
						get(i.x, i.y).edgeId[SOUTH] = get(wn.x, wn.y).edgeId[SOUTH];
						get(i.x, i.y).edgeExist[SOUTH] = true;
					}
					else
					{
						Edge edge;
						edge.sx = x * BLOCK_SIZE;
						edge.sy = (y + 1) * BLOCK_SIZE;

						edge.ex = edge.sx + BLOCK_SIZE;
						edge.ey = edge.sy;

						//Add edge to Polygon Pool
						int edgeId = vecEdges.size();
						vecEdges.push_back(edge);

						//Update tile information with edge information
						get(i.x, i.y).edgeId[SOUTH] = edgeId;
						get(i.x, i.y).edgeExist[SOUTH] = true;
					}
				}
			}

		}
	}
}

