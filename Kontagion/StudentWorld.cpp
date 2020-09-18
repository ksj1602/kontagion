#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <iterator>
using namespace std;

bool safeToCreateObjectAt(double x, double y, const LocationArray& l);

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    socrates = nullptr;
    bacteriaRemaining = 0;
    allBacteriaReleased = false;
}

bool StudentWorld::levelComplete() const {
    return (allBacteriaReleased && !bacteriaRemaining);
}

void StudentWorld::addBacterium(int type, double x, double y) {
    bacteriaRemaining++;
    switch (type) {
        case REGULAR_SALMONELLA:
            actors.push_back(new RegularSalmonella(this, x, y));
            break;
        case AGGRESSIVE_SALMONELLA:
            actors.push_back(new AggressiveSalmonella(this, x, y));
            break;
        case ECOLI:
            actors.push_back(new Ecoli(this, x, y));
            break;
        default:
            break;
    }
    playSound(SOUND_BACTERIUM_BORN);
}



int StudentWorld::init()
{
    // Initialize Socrates
    socrates = new Socrates(this);

    // Add dirt piles first because they may overlap
    addInitObject(DIRT, max (180 - 20 * getLevel(), 20));

    // Add bacteria pits
    addInitObject(PIT, getLevel());

    // Add food objects
    addInitObject(FOOD, min(5 * getLevel(), 25));
    
    bacteriaRemaining = 0;
    allBacteriaReleased = false;

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::addInitObject(int type, int qty) {
    int newAngle, newDist;
    for (int i = 0; i < qty; i++) {
        newAngle = randInt(0, 359);
        newDist = randInt(0, 120);
        double newX = xCoordinateFromCenter(newDist, newAngle);
        double newY = yCoordinateFromCenter(newDist, newAngle);
        switch (type) {
            case DIRT:
                locations.push_back(make_pair(newX, newY));
                actors.push_back(new DirtPile(this, newX, newY));
                break;
            case PIT:
                if (safeToCreateObjectAt(newX, newY, locations)) {
                    actors.push_back(new Pit(this, newX, newY));
                    locations.push_back(make_pair(newX, newY));
                }
                else
                    i--;
                break;
            case FOOD:
                if (safeToCreateObjectAt(newX, newY, locations)) {
                    actors.push_back(new Food(this, newX, newY));
                    locations.push_back(make_pair(newX, newY));
                }
                else
                    i--;
                break;
            default:
                break;
        }
    }
}

int StudentWorld::move()
{
    
    if (levelComplete())
        return GWSTATUS_FINISHED_LEVEL;

    socrates->doSomething(); // first Socrates gets a chance to do something
    if (!socrates->isAlive()) {
        playSound(SOUND_PLAYER_DIE);
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }

    int chanceGoodie = max(510 - getLevel() * 10, 250);
    if (randInt(0, chanceGoodie - 1) == 0) {
        int typeOfGoodie = randInt(1, 10);
        int newGoodieAngle = randInt(0, 359);
        switch(typeOfGoodie) {
            case 1: // 1/10 cases gives us a 10% chance for an extra life goodie
                actors.push_back(new ExtraLifeGoodie(this, xCoordinateFromCenter(VIEW_RADIUS, newGoodieAngle),
                        yCoordinateFromCenter(VIEW_RADIUS, newGoodieAngle)));
                break;
            case 2: // three cases gives us a 30% chance that the goodie is a flame thrower goodie
            case 3:
            case 4:
                actors.push_back(new FlameThrowerGoodie(this, xCoordinateFromCenter(VIEW_RADIUS, newGoodieAngle),
                                                     yCoordinateFromCenter(VIEW_RADIUS, newGoodieAngle)));
                break;
            default: // otherwise we introduce a restore health goodie
                actors.push_back(new RestoreHealthGoodie(this, xCoordinateFromCenter(VIEW_RADIUS, newGoodieAngle),
                                                     yCoordinateFromCenter(VIEW_RADIUS, newGoodieAngle)));
                break;
        }
    }
    
    int chanceFungus = randInt(0, (max(510 - getLevel() * 10, 200)) - 1);
    if (chanceFungus == 0) {
        int newFungusAngle = randInt(0, 359);
        actors.push_back(new Fungus(this, xCoordinateFromCenter(VIEW_RADIUS, newFungusAngle), yCoordinateFromCenter(VIEW_RADIUS, newFungusAngle)));
    }
    
    vector<Actor*>::iterator p; // iterator to get pointers to dead actors
    vector<Actor*> toBeDeleted; // vector for pointers to dead actors to be deleted
    
    for (auto &it : actors) // give all live actors a chance to do something
        if (it->isAlive())
            it->doSomething();
    
    
    // To cleanse all dead actors, we store pointers to them in a vector and then delete each of those pointers
    // We process dead actors after alive ones have acted because actors affect each other's alive/dead status
    for (p = actors.begin(); p != actors.end(); ) {
        if (!(*p)->isAlive()) {
            if ((*p)->isBacterium())
                bacteriaRemaining--;
            toBeDeleted.push_back(*p);
            actors.erase(p); // erase() automatically sets p to point to the next element in the vector, so we do not increment it
            continue;
        }
        p++;
    }
    
    // deleting the pointers to each of the dead actors
    for (auto &it : toBeDeleted)
        delete it;
    toBeDeleted.clear(); // clear out the pointers to deleted objects for the next tick
    
    setGameStatText("Score: " + to_string(getScore()) + "  Level: " + to_string(getLevel()) + "  Lives: " + to_string(getLives()) + "  Health: " + to_string(socrates->getHitpoints()) + "  Sprays: " + to_string(socrates->getSpraysLeft()) + "  Flames: " + to_string(socrates->getFlamesLeft()));
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::signalThatAllBacteriaReleased() {
    allBacteriaReleased = true;
}

void StudentWorld::cleanUp()
{
    if (socrates != nullptr) {
        delete socrates; // delete the socrates object
        socrates = nullptr; // set the socrates pointer to null so that it is not deleted again when called by the destructor
    }
    for (auto &p : actors)
        delete p;
    actors.clear();
}

Actor* StudentWorld::overlap(Actor *a) {
    vector<Actor*>::iterator p;
    for (p = actors.begin(); p != actors.end(); p++) { // we first check for bacterium
        if (distance(a->getX(), a->getY(), (*p)->getX(), (*p)->getY()) <= SPRITE_WIDTH && (*p)->isBacterium())
            return (*p);
    }
    for (p = actors.begin(); p != actors.end(); p++) { // if not bacterium, we look for damageable objects
        if (distance(a->getX(), a->getY(), (*p)->getX(), (*p)->getY()) <= SPRITE_WIDTH && (*p)->isDestructable()) {
            return (*p);
        }
    }
    return nullptr;
}

bool StudentWorld::overlapWithSocrates(Actor *a) {
    return (distance(socrates->getX(), socrates->getY(), a->getX(), a->getY()) <= SPRITE_WIDTH);
}

bool StudentWorld::overlapWithFood(Actor *a) {
    for (auto &it : actors) {
        if (it->isFood() && (distance(it->getX(), it->getY(), a->getX(), a->getY()) <= SPRITE_WIDTH)) {
            it->setDead();
            return true;
        }
    }
    return false;
}

bool StudentWorld::findNearbyFoodDirection(Actor *a, Direction &result) {
    for (auto &it : actors) {
        if (it->isFood() && (distance(it->getX(), it->getY(), a->getX(), a->getY()) <= 128)) {
            result = (int) angleBetweenPositions(it->getX(), it->getY(), a->getX(), a->getY());
            return true;
        }
    }
    return false;
}

bool StudentWorld::findSocratesNearby(Actor* a, int dist, Direction &result) {
    if (distanceToSocrates(a) <= dist) {
        result = (int) angleBetweenPositions(socrates->getX(), socrates->getY(), a->getX(), a->getY());
        return true;
    }
    return false;
}

bool StudentWorld::movementOverlap(double x, double y) {
    if (distance(x, y, VIEW_WIDTH / 2, VIEW_HEIGHT / 2) >= VIEW_RADIUS)
        return true;
    for (auto &it : actors) {
        if (it->blocksMovement() && (distance(it->getX(), it->getY(), x, y)) <= (SPRITE_WIDTH / 2))
            return true;
    }
    return false;
}

double StudentWorld::distanceToSocrates(Actor *a) {
    return (distance(socrates->getX(), socrates->getY(), a->getX(), a->getY()));
}

void StudentWorld::adjustSocratesHitPoints(int hp) {
    socrates->increaseHitPoints(hp);
}

void StudentWorld::adjustSocratesFlames(int qty) {
    socrates->addFlames(qty);
}

void StudentWorld::addProjectile(double x, double y, int dir, int type) {
    switch (type) {
        case SPRAY:
            actors.push_back(new Spray(this, x, y, dir));
            break;
        case FLAME:
            actors.push_back(new Flame(this, x, y, dir));
            break;
        default:
            break;
    }
}

void StudentWorld::addFood(double x, double y) {
    actors.push_back(new Food(this, x, y));
}

StudentWorld::~StudentWorld() {
    cleanUp(); // cleanup does the work of the destructor
}

bool safeToCreateObjectAt(double x, double y, const LocationArray& l) {
    for (auto &it : l) {
        if (distance(it.first, it.second, x, y) <= SPRITE_WIDTH)
            return false;
    }
    return true;
}
