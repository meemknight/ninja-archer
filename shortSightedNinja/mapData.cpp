#include "mapData.h"
#include <string>
#include "mapRenderer.h"
#include <algorithm>

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
			data[i].sideColors = { 0,0,0,0 };
		}
	}
}

void MapData::cleanup()
{
	if(data)
	{
		delete[] data;
	}
}

bool BlockInfo::hasNeighborLeft()
{
	return neighbors & 0b0000'0010;
}

bool BlockInfo::hasNeighborRight()
{
	return neighbors & 0b0000'0001;
}

bool BlockInfo::hasNeighborTop()
{
	return neighbors & 0b0000'1000;
}

bool BlockInfo::hasNeighborDown()
{
	return neighbors & 0b0000'0100;
}

void BlockInfo::resetColors()
{
	mainColor = { 0,0,0,0 };
	directionalLight = { 0,0,0,0 };
	alpha = 0;
	sideColors = {};
}

void MapData::setNeighbors()
{
	for(int y=0; y<h; y++)
	{
		for(int x=0; x<w; x++)
		{
			auto &n = get(x, y).neighbors;
			n = 0;

			if (y > 0) // top
			{
				if(isOpaque(get(x,y-1).type))
				{
					n |= 0b0000'1000;
				}
			}
			if (y < h-1) // bottom
			{
				if (isOpaque(get(x, y + 1).type))
				{
					n |= 0b0000'0100;
				}
			}
			if (x > 0) // left
			{
				if (isOpaque(get(x-1, y).type))
				{
					n |= 0b0000'0010;
				}
			}
			if (x < w - 1) // bottom
			{
				if (isOpaque(get(x+1, y).type))
				{
					n |= 0b0000'0001;
				}
			}

		}
	}
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

void MapData::CalculateVisibilityPolygon(float ox, float oy, float radius)
{
	vecVisibilityPolygonPoints.clear();

	for(auto &e1 : vecEdges)
	{
		// Take the start point, then the end point
		for(int i = 0; i < 2; i++)
		{
			float rdx = (i == 0 ? e1.sx : e1.ex) - ox;
			float rdy = (i == 0 ? e1.sy : e1.ey) - oy;

			const float base_ang = atan2f(rdy, rdx);

			float ang = 0;

			// For each point, cast 3 rays, 1 directly at point and 1 a little bit either side
			for (int j = 0; j < 3; j++)
			{
				if (j == 0) ang = base_ang - 0.0001f;
				if (j == 1) ang = base_ang;
				if (j == 3) ang = base_ang + 0.0001f;

				// Create ray along angle for required distance
				rdx = radius * cosf(ang);
				rdy = radius * sinf(ang);

				float min_t1 = INFINITY;
				float min_px = 0, min_py = 0, min_ang = 0;
				bool bValid = false;

				// Check for ray intersection with all edges
				for (auto& e2 : vecEdges)
				{
					// Create line segment vector
					float sdx = e2.ex - e2.sx;
					float sdy = e2.ey - e2.sy;

					if (fabs(sdx - rdx) > 0.0f && fabs(sdy - rdy) > 0.0f)
					{
						// t2 is normalised distance from line segment start to line segment end of intersect point	
						float t2 = (rdx * (e2.sy - oy) + (rdy * (ox - e2.sx))) / (sdx * rdy - sdy * rdx);
						// t1 is normalised distance from source along ray to ray length of intersect point
						float t1 = (e2.sx + sdx * t2 - ox) / rdx;

						// If intersect point exists along ray, and along line segment then intersect point is valid
						if (t1 > 0 && t2 >= 0 && t2 <= 1.0f)
						{
							// Check if this intersect point is closest to source. If
							// it is, then store this point and reject others
							if (t1 < min_t1)
							{
								min_t1 = t1;
								min_px = ox + rdx * t1;
								min_py = oy + rdy * t1;
								min_ang = atan2f(min_py - oy, min_px - ox);
								bValid = true;
							}
						}
					}
				}

				// Add intersection point to visibility polygon perimeter
				if (bValid)
					vecVisibilityPolygonPoints.push_back({ min_ang, min_px, min_py });
			}
		}
	}

	// Sort perimeter points by angle from source. This will allow
	// us to draw a triangle fan.
	std::sort(
		vecVisibilityPolygonPoints.begin(),
		vecVisibilityPolygonPoints.end(),
		[&](const VisibilityPolygonPoints& t1, const VisibilityPolygonPoints& t2)
		{
			return t1.angle < t2.angle;
		});
}

