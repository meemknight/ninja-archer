#include "mapData.h"
#include <string>
#include "mapRenderer.h"
#include <algorithm>

void MapData::create(int w, int h, const char* d = 0)
{
	this->w = w;
	this->h = h;

	data = new BlockInfo[w * h];

	if (d)
	{
		for (int i = 0; i < w * h; i++)
		{
			data[i].type = d[i];
			data[i].mainColor = glm::vec4(1, 1, 1, 1);

		}
	}
	else
	{
		for (int i = 0; i < w * h; i++)
		{
			data[i].type = Block::none;
			data[i].mainColor = glm::vec4(1, 1, 1, 1);

		}
	}

	setNeighbors();
	//todo setup neighbours
}

BlockInfo& MapData::get(int x, int y)
{
	return data[x + this->w * y];
}

void MapData::clearColorData()
{
	if (data)
	{
		for (int i = 0; i < w * h; i++)
		{
			data[i].mainColor = { 0,0,0,0 };
			data[i].sideColors = { 0,0,0,0 };
		}
	}
}

void MapData::cleanup()
{
	if (data)
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
	sideColors = {};
}

void MapData::setNeighbors()
{
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			auto& n = get(x, y).neighbors;
			n = 0;

			if (y > 0) // top
			{
				if (isOpaque(get(x, y - 1).type))
				{
					n |= 0b0000'1000;
				}
			}
			if (y < h - 1) // bottom
			{
				if (isOpaque(get(x, y + 1).type))
				{
					n |= 0b0000'0100;
				}
			}
			if (x > 0) // left
			{
				if (isOpaque(get(x - 1, y).type))
				{
					n |= 0b0000'0010;
				}
			}
			if (x < w - 1) // bottom
			{
				if (isOpaque(get(x + 1, y).type))
				{
					n |= 0b0000'0001;
				}
			}

		}
	}
}

void MapData::CalculateVisibilityPolygon(float ox, float oy, float radius)
{
	vecVisibilityPolygonPoints.clear();

	for (auto& e1 : vecEdges)
	{
		// Take the start point, then the end point
		for (int i = 0; i < 2; i++)
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

