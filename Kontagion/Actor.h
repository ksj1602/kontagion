#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <algorithm>


#define REGULAR_SALMONELLA 1
#define AGGRESSIVE_SALMONELLA 2
#define ECOLI 3
#define SALMONELLA 1

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

const double PI = atan(1) * 4;

double distance (double x1, double y1, double x2, double y2);
double xCoordinateFromCenter (double dist, int angle);
double yCoordinateFromCenter (double dist, int angle);
double angleBetweenPositions (double x1, double y1, double x2, double y2);

class StudentWorld;

class Actor: public GraphObject { // base class for all of the other objects in the game
public:
    Actor(StudentWorld* sw, int imageID, double startX, double startY, int hp, Direction dir = 0, int depth = 0, double size = 1.0) : GraphObject(imageID, startX, startY, dir, depth, size) {
        m_isAlive = true;
        m_world = sw;
        m_hitPoints = hp;
    }
    bool isAlive() { return m_isAlive; }
    void setDead() { m_isAlive = false; }
    int getHitpoints() const { return m_hitPoints; }
    void setHitPoints(int hp) { m_hitPoints = hp; }
    virtual void increaseHitPoints (int hp);
    StudentWorld* getWorld() const { return m_world; }
    virtual void doSomething() = 0;
    virtual bool isDestructable() const { return false; }
    virtual bool isBacterium() const { return false; }
    virtual bool isFood() const { return false; }
    virtual bool blocksMovement() const { return false ; }
    virtual ~Actor() = default;
    
private:
    bool m_isAlive; // variable to keep track of dead/alive status
    int m_hitPoints; // variable to keep track of hitpoints remaining
    StudentWorld* m_world; // pointer to Student world object
};

class Socrates: public Actor {
public:
    Socrates(StudentWorld* sw);
    virtual void doSomething();
    void moveToPositionAngle(double pa);
    virtual void increaseHitPoints (int hp);

    inline
    virtual ~Socrates() = default;
    void addSprays (int change) {
        if (m_sprayCharges < 20) // Socrates can have a maximum of 20 spray charges
            m_sprayCharges += change;
    }
    void addFlames (int change) {
        m_flameThrowerCharges += change;
        if (m_flameThrowerCharges < 0) // Socrates can have a minimum of zero flame charges, not negative
            m_flameThrowerCharges = 0;
    }
    int getSpraysLeft () const { return m_sprayCharges; }
    int getFlamesLeft () const { return m_flameThrowerCharges; }

private:
    int m_sprayCharges; // variable for number of spray charges remaining
    int m_flameThrowerCharges; // variable to store number of flame thrower charges remaining
    int m_positionAngle; // variable to store position around the dish as an angle
};

class DirtPile: public Actor {
public:
    DirtPile(StudentWorld* sw, double x, double y);
    virtual void doSomething();


    inline
    virtual ~DirtPile() = default;
    virtual bool blocksMovement() const { return true; }
    virtual bool isDestructable() const { return true; }
};

class Pit: public Actor {
public:
    Pit(StudentWorld* sw, double x, double y);
    virtual void doSomething();

    inline
    virtual ~Pit() { m_typesRemaining.clear(); } // empty the typesRemaining set before destructing it
private:
    int m_rs; // regular salmonella inventory
    int m_as; // aggressive salmonella inventory
    int m_ec; // E.coli inventory
    int m_totalInventory; // variable to keep track of total bacteria remaining
    std::set<int> m_typesRemaining; // set to keep track of the types of bacteria remaining
};

class Projectile: public Actor { // class to represent sprays and flames since they share many common features
public:
    Projectile(StudentWorld *sw, int iid, double x, double y, int dir, int td, int dc); // td for travel distance
    virtual void doSomething();

    inline
    virtual ~Projectile() = default;

private:
    int m_travelDistance; // every projectile has a maximum distance it can go before it dissipates
    int m_damageCapacity; // every projectile can deal a certain amount of damage to whatever it overlaps with
};

class Spray: public Projectile {
public:
    Spray(StudentWorld* sw, double x, double y, int dir);

    inline
    virtual ~Spray() = default;
};

class Flame: public Projectile {
public:
    Flame (StudentWorld* sw, double x, double y, int dir);

    inline
    virtual ~Flame() = default;
};

class Food: public Actor {
public:
    Food (StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual bool isFood() const;

    inline
    virtual ~Food() = default;
};

class Goodie: public Actor {
public:
    Goodie(StudentWorld* sw, int imageID, double x, double y);
    virtual void doSomething();
    virtual void takeSpecificGoodieAction() = 0;

    inline
    virtual ~Goodie() = default;
    virtual bool isDestructable() const { return true; }
private:
    int m_lifetime; // every goodie has a lifetime
};

class RestoreHealthGoodie: public Goodie {
public:
    RestoreHealthGoodie(StudentWorld* sw, double x, double y);
    virtual void takeSpecificGoodieAction();

    inline
    virtual ~RestoreHealthGoodie() = default;
};

class FlameThrowerGoodie: public Goodie {
public:
    FlameThrowerGoodie(StudentWorld* sw, double x, double y);
    virtual void takeSpecificGoodieAction();

    inline
    virtual ~FlameThrowerGoodie() = default;
};

class ExtraLifeGoodie: public Goodie {
public:
    ExtraLifeGoodie(StudentWorld* sw, double x, double y);
    virtual void takeSpecificGoodieAction();

    inline
    virtual ~ExtraLifeGoodie() = default;
};

class Fungus: public Goodie {
public:
    Fungus(StudentWorld *sw, double x, double y);
    virtual void takeSpecificGoodieAction();

    inline
    virtual ~Fungus() = default;
};

class Bacterium: public Actor {
public:
    Bacterium(StudentWorld *sw, int imageID, int hp, double x, double y, int type, int dc);
    void updateFoodEaten(int qty); // function to update food eaten
    virtual void doSomething() = 0; // doSomething is pure virtual because behavior varies among the 3 kinds of bacteria
    void takeDamage(int hp); // helper function for increaseHitPoints
    void process1(); // common process of doSomething for all bacteria
    virtual void increaseHitPoints(int hp); // custom function required for bacteria because they are to make sounds
    void die(int type); // function to handle what is to happen when the bacterium dies
    double computeNewCoordinate(char axis); // function for computing new coordinate according to the spec

    inline
    virtual ~Bacterium() = default;
    int getFoodEaten() const { return m_foodEaten; }
    int getMovementPlanDist() const { return m_movementPlanDistance; }
    void updateMovementPlanDist (int qty) { m_movementPlanDistance += qty; }
    int getType() const { return m_type ; }
    virtual bool isBacterium() const { return true; }
private:
    int m_movementPlanDistance; // remaining movement plan distance
    int m_foodEaten; // how much food the bacteria has consumed
    int m_type; // type of bacteria required for doSomething() processing
    int m_damageCapacity; // how much damage does the bacteria deal to Socrates in one tick when it overlaps with him
};

class Salmonella: public Bacterium {
public:
    Salmonella(StudentWorld *sw, int hp, double x, double y, int dc);
    virtual void process2();

    inline
    virtual ~Salmonella() = default;
};

class RegularSalmonella: public Salmonella {
public:
    RegularSalmonella(StudentWorld *sw, double x, double y);
    virtual void doSomething();
    
    inline
    virtual ~RegularSalmonella() = default;
};

class AggressiveSalmonella: public Salmonella {
public:
    AggressiveSalmonella(StudentWorld *sw, double x, double y);
    virtual void doSomething();
    
    inline
    virtual ~AggressiveSalmonella() = default;
};

class Ecoli: public Bacterium {
public:
    Ecoli(StudentWorld *sw, double x, double y);
    virtual void doSomething();
    
    inline
    virtual ~Ecoli() = default;
};
#endif // ACTOR_H_
