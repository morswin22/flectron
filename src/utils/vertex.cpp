#include <flectron/utils/vertex.hpp>

#include <flectron/scene/components.hpp>
#include <flectron/physics/math.hpp>

namespace flectron
{

  std::vector<size_t> trianglesFromVertices(std::vector<Vector>& vertices)
  {
    if (vertices.size() < 3)
      throw std::invalid_argument("Too few vertices");

    if (!isSimplePolygon(vertices))
      throw std::invalid_argument("Not a simple polygon");
    
    if (containsColinearEdges(vertices))
      throw std::invalid_argument("Polygon contains colinear edges");

    WindingOrder order = getWindingOrder(vertices);
    if (order == WindingOrder::Invalid)
      throw std::invalid_argument("Invalid winding order");
    
    if (order == WindingOrder::CounterClockwise)
      for (size_t i = 0; i < vertices.size(); i++)
        std::swap(vertices[i], vertices[vertices.size() - 1 - i]);

    std::vector<size_t> indexList;
    for (size_t i = 0; i < vertices.size(); i++)
      indexList.push_back(i);

    std::vector<size_t> triangles;
    triangles.resize((vertices.size() - 2) * 3);
    int index = 0;

    while (indexList.size() > 3)
    {
      for (size_t i = 0; i < indexList.size(); i++)
      {
        size_t a = indexList[i];
        size_t b = indexList[getIndex((int)i - 1, (int)indexList.size())];
        size_t c = indexList[getIndex((int)i + 1, (int)indexList.size())];

        Vector va = vertices[a];
        Vector vb = vertices[b];
        Vector vc = vertices[c];

        Vector va2vb = vb - va;
        Vector va2vc = vc - va;

        if (cross(va2vb, va2vc) < 0.0f)
          continue;

        bool isEar = true;

        for (size_t j = 0; j < vertices.size(); j++)
        {
          if (j == a || j == b || j == c)
            continue;
          
          Vector p = vertices[j];

          if (isPointInTriangle(p, vb, va, vc))
          {
            isEar = false;
            break;
          }

        }
        
        if (isEar)
        {
          triangles[index++] = b;
          triangles[index++] = a;
          triangles[index++] = c;
          indexList.erase(std::next(indexList.begin(), i));
          break;
        }
      }
    }

    triangles[index++] = indexList[0];
    triangles[index++] = indexList[1];
    triangles[index++] = indexList[2];

    return triangles;
  }

  int getIndex(int index, int length)
  {
    if (index >= length)
      return index % length;
    if (index < 0) 
      return index % length + length;
    return index;
  }

  bool isPointInTriangle(const Vector& point, const Vector& a, const Vector& b, const Vector& c)
  {
    Vector ab = b - a;
    Vector bc = c - b;
    Vector ca = a - c;

    Vector ap = point - a;
    Vector bp = point - b;
    Vector cp = point - c;

    float crossA = cross(ab, ap);
    float crossB = cross(bc, bp);
    float crossC = cross(ca, cp);

    if (crossA > 0.0f || crossB > 0.0f || crossC > 0.0f)
      return false;

    return true;
  }

  WindingOrder getWindingOrder(const std::vector<Vector>& vertices)
  {
    int sum{ 0 };

    for (int i = 0; i < vertices.size(); i++)
    {
      Vector a = vertices[i];
      Vector b = vertices[getIndex(i - 1, (int)vertices.size())];
      Vector c = vertices[getIndex(i + 1, (int)vertices.size())];

      sum += orientation(b, a, c);
    }

    if (sum > 0)
      return WindingOrder::Clockwise;

    if (sum < 0)
      return WindingOrder::CounterClockwise;

    return WindingOrder::Invalid;
  }

  int orientation(const Vector& p, const Vector& q, const Vector& r)
  {
    // See https://www.geeksforgeeks.org/orientation-3-ordered-points/
    float value = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (value == 0) 
      return 0;  // colinear
    return (value > 0) ? 1 : -1; // clock or counterclock wise
  }

  bool containsColinearEdges(const std::vector<Vector>& vertices)
  {
    for (int i = 0; i < vertices.size(); i++)
    {
      Vector a = vertices[i];
      Vector b = vertices[getIndex(i - 1, (int)vertices.size())];
      Vector c = vertices[getIndex(i + 1, (int)vertices.size())];

      if (orientation(b, a, c) == 0)
        return true;
    }

    return false;
  }

  bool isSimplePolygon(const std::vector<Vector>& vertices)
  {
    for (int i = 0; i < vertices.size(); i++)
    {
      Vector a = vertices[i];
      Vector b = vertices[getIndex(i + 1, (int)vertices.size())];

      for (int j = i + 1; j < vertices.size(); j++)
      {
        Vector c = vertices[j];
        Vector d = vertices[getIndex(j + 1, (int)vertices.size())];

        if ((c.x == b.x && c.y == b.y) || (a.x == d.x && a.y == d.y))
          continue;

        if (intersectLines(a, b, c, d))
          return false;
      }
    }
    return true;
  }

  bool intersectLines(const Vector& a, const Vector& b, const Vector& c, const Vector& d)
  {
    // https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
    int o1 = orientation(a, b, c);
    int o2 = orientation(a, b, d);
    int o3 = orientation(c, d, a);
    int o4 = orientation(c, d, b);

    // General case
    if (o1 != o2 && o3 != o4)
        return true;

    // Special Cases
    if (o1 == 0 && isOnLineSegment(a, c, b)) return true;
    if (o2 == 0 && isOnLineSegment(a, d, b)) return true;
    if (o3 == 0 && isOnLineSegment(c, a, d)) return true;
    if (o4 == 0 && isOnLineSegment(c, b, d)) return true;

    return false; // Doesn't fall in any of the above cases
  }

  bool isOnLineSegment(const Vector& p, const Vector& q, const Vector& r)
  {
    if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) && q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
      return true;
    return false;
  }

  float polygonArea(const std::vector<Vector>& vertices)
  {
    float area = 0.0f;

    for (int i = 0; i < vertices.size(); i++)
      area += cross(vertices[i], vertices[getIndex(i + 1, (int)vertices.size())]);

    return std::abs(area / 2.0f);
  }

}
