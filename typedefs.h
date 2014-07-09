#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <opencv2/opencv.hpp>

class Line;
class Square;
class Corner;
class Layer;

typedef std::vector<cv::Point> Points;
typedef std::vector<Line> Lines;
typedef std::vector<Square> Squares;
typedef std::vector<Corner> Corners;
enum Direction {UP, RIGHT, DOWN, LEFT}; // up=0, right=1, down=2, left=3



#endif // TYPEDEFS_H
