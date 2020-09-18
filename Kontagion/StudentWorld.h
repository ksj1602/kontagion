#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <vector>

#define DIRT 1
#define PIT 2
#define FOOD 3
#define SPRAY 1
#define FLAME 2
typedef std::pair<double,double> Location;
typedef std::vector<Location> LocationArray;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void addProjectile(double x, double y, int dir, int type);
    void addFood(double x, double y);
    Actor* overlap (Actor* a);
    bool overlapWithSocrates (Actor *a);
    bool overlapWithFood (Actor *a);
    bool findNearbyFoodDirection(Actor* a, Direction &result);
    bool findSocratesNearby(Actor* a, int dist, Direction &result);
    bool movementOverlap (double x, double y);
    void adjustSocratesHitPoints(int hp);
    void adjustSocratesFlames(int qty);
    void signalThatAllBacteriaReleased();
    void addBacterium(int type, double x, double y);
    virtual ~StudentWorld();

private:
    Socrates* socrates;
    std::vector<Actor *> actors;
    LocationArray locations;
    int bacteriaRemaining;
    bool allBacteriaReleased;
    double distanceToSocrates (Actor *a);
    void addInitObject (int type, int qty);
    bool levelComplete() const;
};

#endif // STUDENTWORLD_H_
