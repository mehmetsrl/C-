// FindLongestPoints.cpp : Defines the entry point for the console application.
//
/*
 Name & Lastname:   Mehmet SARIOGLU
 ID:                1807262
 */

/*Libs*/
#include <cstdio>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <vector>
#include <fstream>
#include <cmath>
#include <stack>

using namespace std;

static const string folderPath = "hw1inputs";
static char folderSeperator = '/';
static char lineSeperators[] = { '\t' ,'=' };
static string lineKeywords[] = { "max " ,"min " };
static const string fileNames[] =
{
    "input_10K.txt",
    "input_20K.txt",
    "input_30K.txt",
    "input_40K.txt",
    "input_50K.txt",
    "input_60K.txt",
    "input_70K.txt",
    "input_80K.txt",
    "input_90K.txt",
    "input_100K.txt"
};
static const string testFileName = "test.txt";

struct Point {
public:
    int x, y;
    
    Point(const Point &p2) { x = p2.x; y = p2.y; }
    
    Point(int x,int y){
        this->x = x;
        this->y = y;
    }
    Point() {
        this->x = 0;
        this->y = 0;
    }
    int CrossProduct(Point const& p) {
        return (this->x*p.y - this->y*p.x);
    }
    
};
Point operator+(Point const& lhs, Point const& rhs) {
    return Point(lhs.x + rhs.x, lhs.y + rhs.y);
}
Point operator-(Point const& lhs, Point const& rhs) {
    return Point(lhs.x - rhs.x, lhs.y - rhs.y);
}
Point p0;

static Point Origin = Point(0, 0);
static Point UpUnit = Point(0, 1);
static Point RightUnit = Point(1, 0);

static int numberOfPoints;
static int maxPointInt, minPointInt;
Point *points;

bool getInput(string fileName) {
    string line;
    ifstream sourceFile;
    sourceFile.open(fileName.c_str());
    
    if (sourceFile.is_open()) {
        
        getline(sourceFile, line, '\n');
        numberOfPoints = atof(line.c_str());
        points = new Point[numberOfPoints];
        int pointIndex = 0;
        
        while (getline(sourceFile, line, '\n'))
        {
            if (sourceFile.eof()) {
                sourceFile.close();
                cout << "eof Segmentetion Fault! " << endl;
                return false;
            }
            
            if (pointIndex < numberOfPoints) {
                
                string::size_type pos = line.find(lineSeperators[0]);
                if (line.npos != pos) {
                    //cout<<line.substr(0, pos).c_str()<<"  "<<line.substr(pos+1).c_str()<<endl;
                    points[pointIndex].x = atoi(line.substr(0, pos).c_str());
                    points[pointIndex].y = atoi(line.substr(pos + 1).c_str());
                }
                
                pointIndex++;
            }
            else
            {
                
                string::size_type pos = line.find(lineSeperators[1]);
                if (line.npos != pos) {
                    string keyword = line.substr(0, pos);
                    if(keyword.compare(lineKeywords[0])==0)
                        maxPointInt= atoi(line.substr(pos+1).c_str());
                    else if(keyword.compare(lineKeywords[1]) == 0)
                        minPointInt = atoi(line.substr(pos+1).c_str());
                    else
                    {
                        sourceFile.close();
                        cout << "Segmentetion Fault! "<< endl;
                        return false;
                    }
                    
                }
            }
        }
        
        
        
        
        sourceFile.close();
        cout << "File Read! " << endl;
        return true;
    }
    
    cout << "Unable to open file";
    return false;
}


int distSq(Point p1, Point p2)
{
    return (p1.x - p2.x)*(p1.x - p2.x) +
    (p1.y - p2.y)*(p1.y - p2.y);
}

int orientation(Point p, Point q, Point r)
{
    int val = (q.y - p.y) * (r.x - q.x) -
    (q.x - p.x) * (r.y - q.y);
    
    if (val == 0) return 0;  // colinear
    return (val > 0) ? 1 : 2; // clock or counterclock wise
}

int compare(const void *vp1, const void *vp2)
{
    Point *p1 = (Point *)vp1;
    Point *p2 = (Point *)vp2;
    
    // Find orientation
    int o = orientation(p0, *p1, *p2);
    if (o == 0)
        return (distSq(p0, *p2) >= distSq(p0, *p1)) ? -1 : 1;
    
    return (o == 2) ? -1 : 1;
}

Point nextToTop(stack<Point> &S)
{
    Point p = S.top();
    S.pop();
    Point res = S.top();
    S.push(p);
    return res;
}

void swap(Point* p1, Point* p2)
{
    Point t = *p1;
    *p1 = *p2;
    *p2 = t;
}

vector<Point> SolveConvexHull(Point ps[],int n) {
    stack<Point> convexHullPoints;
    
    //Find bottom-most point
    int ymin = ps[0].y, min = 0;
    for (int i = 1; i < n; i++)
    {
        int y = ps[i].y;
        
        if ((y < ymin) || (ymin == y && ps[i].x < ps[min].x))
        {
            ymin = ps[i].y;
            min = i;
        }
    }
    
    // Place the bottom-most point at first position
    swap(ps[0], ps[min]);
    p0 = ps[0];
    
    //Do Sorting
    qsort(&points[1], n - 1, sizeof(Point), compare);
    
    //for (int i = 0; i < n; i++)
    //    cout << points[i].x << " -|- " << points[i].y << endl;
    
    convexHullPoints.push(ps[0]);
    convexHullPoints.push(ps[1]);
    convexHullPoints.push(ps[2]);
    
    // Process remaining n-3 points
    for (int i = 3; i < n; i++)
    {
        while (orientation(nextToTop(convexHullPoints), convexHullPoints.top(), ps[i]) != 2)
            convexHullPoints.pop();
        convexHullPoints.push(ps[i]);
    }
    
    vector<Point> result;
    
    while (!convexHullPoints.empty())
    {
        result.push_back(convexHullPoints.top());
        convexHullPoints.pop();
    }
    
    return result;
}

Point* GetLongestPointPairs(vector<Point> pts) {
    Point* longestPointPairs = new Point[2];
    int max = 0;
    for (const auto& candidate1 : pts) {
        for (const auto& candidate2 : pts) {
            int distance = distSq(candidate1, candidate2);
            if (max < distance) {
                //cout << "Max: " << max << "  distance: " << distance << endl;
                max = distance;
                longestPointPairs[0] = candidate1;
                longestPointPairs[1] = candidate2;
            }
        }
    }
    return longestPointPairs;
}


int main() {
    int fileIndex = -1;
    string fileName;
    bool validCharacter=false;
    do {
        cout << endl << "Enter fileIndex to get points from file: ";
        cin >> fileIndex;
        if (cin.fail()) break;
        validCharacter = true;
    }
    while (fileIndex < -1 || fileIndex > 9);
    
    if (!validCharacter) {
        cout << endl << "Entered invalid char.."<<endl;
        system("PAUSE");
        return 0;
    }
    
    if (fileIndex == -1)
        fileName = testFileName;
    else
        fileName = fileNames[fileIndex];
    string filePath=folderPath + folderSeperator + fileName;
    cout<<"Now Reading '"<<filePath<<"' ..."<<endl;
    if (getInput(filePath)) {
        
        vector<Point> convexHullPoints=SolveConvexHull(points,numberOfPoints);
        
        cout << endl << "Convexhull Points: " << endl;
        for (const auto& p : convexHullPoints) {
            std::cout << p.x <<" - "<< p.y << endl;
        }
        
        Point *result = GetLongestPointPairs(convexHullPoints);
        cout << endl << "Points Gives Longest Distance: " << endl;
        cout <<"("<< result[0].x<<","<< result[0].y << ")   ("<<result[1].x<<","<< result[1].y<<")" << endl;
    }
    
    system("PAUSE");
    return 1;
}
