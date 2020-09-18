#include "Actor.h"
#include "StudentWorld.h"

using namespace std;

void Actor::increaseHitPoints (int hp) {
    m_hitPoints += hp; // hp may be negative, in this case the actor is taking damage
    if (m_hitPoints <= 0) setDead(); // if the hitpoints fall below or equal to zero, the actor is now dead
}

Socrates::Socrates(StudentWorld* sw)
    : Actor(sw, IID_PLAYER, 0, 127, 100, 0, 0)
{
        // initial inventory and position of Socrates initialized
        m_sprayCharges = 20;
        m_flameThrowerCharges = 5;
        m_positionAngle = 180;
}

// Socrates needs a separate increaseHitPoints function because he interacts with goodies
void Socrates::increaseHitPoints (int hp) {
    if (getHitpoints() + hp >= 100) setHitPoints(100); // a maximum of 100 hp is possible
    else if (getHitpoints() + hp <= 0) setDead();
    else {
        if (hp < 0)
            getWorld()->playSound(SOUND_PLAYER_HURT); // if damaged but still alive, play the 'hurt' sound
        setHitPoints(getHitpoints() + hp);
    }
}

void Socrates::doSomething() {
    if (!isAlive())
        return;
    int ch;
    if (getWorld()->getKey(ch)) {
        switch (ch) {
            case KEY_PRESS_LEFT:
                m_positionAngle += 5; // increment the position angle by 5 degrees
                moveToPositionAngle(m_positionAngle);
                // move 5 degrees counterclockwise
                break;
            case KEY_PRESS_RIGHT:
                m_positionAngle -= 5; // decrement position angle by 5 degrees
                moveToPositionAngle(m_positionAngle);
                // move 5 degrees clockwise
                break;
            case KEY_PRESS_SPACE:
                if (m_sprayCharges > 0) {
                    Location newPos; // using a 'Location' type to store new position
                    getPositionInThisDirection(getDirection(), 2 * SPRITE_RADIUS, newPos.first, newPos.second);
                    getWorld()->addProjectile(newPos.first, newPos.second, getDirection(), SPRAY);
                    m_sprayCharges--;
                    getWorld()->playSound(SOUND_PLAYER_SPRAY);
                }
                // fire disinfectant spray if available
                break;
            case KEY_PRESS_ENTER:
                if (m_flameThrowerCharges > 0) {
                    Location newPos;
                    // similar to spray above except that 16 flames all around Socrates are added
                    for (int i = 0, j = 0; i < 16; i++, j += 22) {
                        getPositionInThisDirection(getDirection() + j, 2*SPRITE_RADIUS, newPos.first, newPos.second);
                        getWorld()->addProjectile(newPos.first, newPos.second, getDirection() + j, FLAME);
                    }
                    m_flameThrowerCharges--;
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                }
                // fire flamethrower if available
                break;
            default:
                break;
        }
    }
    else
        addSprays(1);
}

void Socrates::moveToPositionAngle(double pa) {
    // call the coordinate from center functions and pass those values to moveTo()
    moveTo(xCoordinateFromCenter(VIEW_RADIUS, pa), yCoordinateFromCenter(VIEW_RADIUS, pa));

    // update direction in which socrates is facing
    setDirection(m_positionAngle + 180);
}

DirtPile::DirtPile(StudentWorld* sw, double x, double y) : Actor(sw, IID_DIRT, x, y, 0, 0, 1) {}

void DirtPile::doSomething() {} // dirt does not do anything


Projectile::Projectile(StudentWorld *sw, int iid, double x, double y, int dir, int td, int dc)
    : Actor(sw, iid, x, y, 0, dir, 1)
{
    m_travelDistance = td;
    m_damageCapacity = dc;
}

void Projectile::doSomething() {
    if (!isAlive())
        return;

    // if the projectile overlaps with something that can be damaged, it is dissipated and the damageable object is damaged
    Actor* a = getWorld()->overlap(this);
    if (a != nullptr) {
        a->increaseHitPoints(-m_damageCapacity);
        this->setDead();
        return;
    }

    // else if it can still move, it continues moving forward in its direction
    else if (m_travelDistance > 0) {
        moveAngle(getDirection(), SPRITE_WIDTH);
        m_travelDistance -= SPRITE_WIDTH;
    }

    // if it has moved enough and there is nothing around to damage, it is the end of the projectile
    else
        this->setDead();
}


Spray::Spray(StudentWorld* sw, double x, double y, int dir) : Projectile (sw, IID_SPRAY, x, y, dir, 112, 2) {}

Flame::Flame(StudentWorld *sw, double x, double y, int dir) : Projectile (sw, IID_FLAME, x, y, dir, 32, 5) {}

Food::Food(StudentWorld* sw, double x, double y) : Actor(sw, IID_FOOD, x, y, 0, 90, 1) {}

bool Food::isFood() const { return true; } // food is the only actor that is edible

void Food::doSomething() {}

Pit::Pit(StudentWorld* sw, double x, double y) : Actor(sw, IID_PIT, x, y, 0, 0, 1) {

    // initialize the pit with appropriate numbers of each bacteria
    m_rs = 5;
    m_as = 3;
    m_ec = 2;

    // the total inventory is 10
    m_totalInventory = 10;

    // the pit initially has all 3 types remaining
    m_typesRemaining.insert(REGULAR_SALMONELLA);
    m_typesRemaining.insert(AGGRESSIVE_SALMONELLA);
    m_typesRemaining.insert(ECOLI);
}

void Pit::doSomething() {
    if (!isAlive())
        return;

    // with no bacteria remaining, the pit must die and signal to the StudentWorld that it has released all its bacteria
    if (m_totalInventory == 0) {
        setDead();
        getWorld()->signalThatAllBacteriaReleased();
        return;
    }

    // first generate a random number to determine whether a bacterium is to be released
    bool bacteriaIsToBeReleased = (randInt(1, 50) == 1);


    if (bacteriaIsToBeReleased) {

        // generate a random index from among the types of bacteria remaining.
        int randIndex = randInt(0, m_typesRemaining.size() - 1);

        // iterate to which type of bacteria is to be released
        std::set<int>::iterator r;
        int v = 0;
        for (r = m_typesRemaining.begin(), v = 0; v < randIndex; r++) v++;

        // introduce the appropriate kind of bacterium into the world
        int type = *r;
        switch (type) {
            case REGULAR_SALMONELLA:
                getWorld()->addBacterium(REGULAR_SALMONELLA, getX(), getY());
                m_rs--;
                m_totalInventory--;
                if (m_rs == 0)
                    m_typesRemaining.erase(REGULAR_SALMONELLA);
                break;
            case AGGRESSIVE_SALMONELLA:
                getWorld()->addBacterium(AGGRESSIVE_SALMONELLA, getX(), getY());
                m_as--;
                m_totalInventory--;
                if (m_as == 0)
                    m_typesRemaining.erase(AGGRESSIVE_SALMONELLA);
                break;
            case ECOLI:
                getWorld()->addBacterium(ECOLI, getX(), getY());
                m_ec--;
                m_totalInventory--;
                if (m_ec == 0)
                    m_typesRemaining.erase(ECOLI);
                break;
            default:
                break;
        }
    }
}


Goodie::Goodie(StudentWorld *sw, int imageID, double x, double y)
    : Actor (sw, imageID, x, y, 0, 0, 1)
{
    // set lifetime to initial value provided in the spec
    m_lifetime = max(randInt(0, 300 - 10 * getWorld()->getLevel() - 1), 50);
}

void Goodie::doSomething() {
    if(!isAlive())
        return;

    // if there is overlap with socrates, help him according to the kind of goodie it is
    if (getWorld()->overlapWithSocrates(this))
        takeSpecificGoodieAction();
    else {
        m_lifetime--;
        if (m_lifetime == 0)
            setDead();
    }
}

RestoreHealthGoodie::RestoreHealthGoodie(StudentWorld *sw, double x, double y) : Goodie(sw, IID_RESTORE_HEALTH_GOODIE, x, y) {}

void RestoreHealthGoodie::takeSpecificGoodieAction() {
    getWorld()->increaseScore(250);
    this->setDead();
    getWorld()->playSound(SOUND_GOT_GOODIE);

    // add 100 to Socrates' hitpoints. The Socrates increaseHitPoints() function takes care of the rest
    getWorld()->adjustSocratesHitPoints(100);
}

FlameThrowerGoodie::FlameThrowerGoodie(StudentWorld *sw, double x, double y) : Goodie(sw, IID_FLAME_THROWER_GOODIE, x, y) {}

void FlameThrowerGoodie::takeSpecificGoodieAction() {
    getWorld()->increaseScore(300);
    this->setDead();
    getWorld()->playSound(SOUND_GOT_GOODIE);
    getWorld()->adjustSocratesFlames(5); // add 5 flames to Socrates' inventory
}

ExtraLifeGoodie::ExtraLifeGoodie(StudentWorld *sw, double x, double y) : Goodie(sw, IID_EXTRA_LIFE_GOODIE, x, y) {}

void ExtraLifeGoodie::takeSpecificGoodieAction() {
    getWorld()->increaseScore(500);
    this->setDead();
    getWorld()->playSound(SOUND_GOT_GOODIE);
    getWorld()->incLives();
}

Fungus::Fungus(StudentWorld *sw, double x, double y) : Goodie(sw, IID_FUNGUS, x, y) {}

void Fungus::takeSpecificGoodieAction() {
    getWorld()->increaseScore(-50);
    this->setDead();
    getWorld()->adjustSocratesHitPoints(-20); // Hurt Socrates by decrementing his hitpoints by 20
}

Bacterium::Bacterium(StudentWorld *sw, int imageID, int hp, double x, double y, int type, int dc) : Actor(sw, imageID, x, y, hp, 90, 0) {
    m_movementPlanDistance = 0;
    m_foodEaten = 0;
    m_type = type;
    m_damageCapacity = dc;
}

void Bacterium::updateFoodEaten(int qty) {
    m_foodEaten += qty;
}

double Bacterium::computeNewCoordinate(char axis) {
    double temp1 = (axis == 'x' ? getX() : getY());
    double temp2 = (axis == 'x' ? VIEW_WIDTH : VIEW_HEIGHT);
    if (temp1 < (temp2 / 2))
        return (temp1 + (SPRITE_WIDTH / 2));
    else if (temp1 > (temp2 / 2))
        return (temp1 - (SPRITE_WIDTH / 2));
    return temp1;
}

void Bacterium::increaseHitPoints(int hp) {
    takeDamage(hp);
}

void Bacterium::takeDamage(int hp) {
    Actor::increaseHitPoints(hp); // base class function updates hitpoints appropriately
    switch (getType()) {
        case REGULAR_SALMONELLA:
        case AGGRESSIVE_SALMONELLA:
            if(isAlive())
                getWorld()->playSound(SOUND_SALMONELLA_HURT);
            else
                die(SALMONELLA);
            break;
        case ECOLI:
            if(isAlive())
                getWorld()->playSound(SOUND_ECOLI_HURT);
            else
                die(ECOLI);
            break;
        default:
            break;
    }
}

void Bacterium::die(int type) {
    if (type == SALMONELLA)
        getWorld()->playSound(SOUND_SALMONELLA_DIE);
    else
        getWorld()->playSound(SOUND_ECOLI_DIE);
    getWorld()->increaseScore(100); // killing any bacteria awards 100 points

    // there is a half chance that the dead bacterium turns into food
    bool turnIntoFood = (randInt(1, 2)) == 1;
    if (turnIntoFood)
        getWorld()->addFood(getX(), getY());
}

Salmonella::Salmonella(StudentWorld *sw, int hp, double x, double y, int dc) : Bacterium(sw, IID_SALMONELLA, hp, x, y, (dc == 1) ? REGULAR_SALMONELLA : AGGRESSIVE_SALMONELLA, dc) {}

void Bacterium::process1() {

    // if there is an overlap with Socrates, hurt him with damageCapacity
    if (getWorld()->overlapWithSocrates(this)) {
        getWorld()->adjustSocratesHitPoints(-m_damageCapacity);
    }

    // else if enough food has been eaten, divide
    else if (getFoodEaten() == 3) {
        getWorld()->addBacterium(getType(), computeNewCoordinate('x'), computeNewCoordinate('y'));
        updateFoodEaten(-3);
    }

    // else if there is food nearby, eat it
    else if (getWorld()->overlapWithFood(this)) {
        updateFoodEaten(1);
    }
}

void Salmonella::process2() {

    // if there is already a path the salmonella is on, continue on that path
    if (getMovementPlanDist() > 0) {
        updateMovementPlanDist(-1);
        Location newPos;
        getPositionInThisDirection(getDirection(), 3, newPos.first, newPos.second);

        // if there is movement overlap with a dirt pile, pick new random direction to move in
        if (getWorld()->movementOverlap(newPos.first, newPos.second)) {
            setDirection(randInt(0, 359));
            updateMovementPlanDist(10 - getMovementPlanDist());
        }

        // if no overlap move to next point in path
        else
            moveTo(newPos.first, newPos.second);
    }

    // else get directional angle of the closest food
    else {
        Direction newD = 0;
        Location newPos;

        // if food is indeed found nearby, attempt to move towards it
        if (getWorld()->findNearbyFoodDirection(this, newD)) {
            setDirection(newD);
            getPositionInThisDirection(newD, 3, newPos.first, newPos.second);
            if (!getWorld()->movementOverlap(newPos.first, newPos.second))
                moveTo(newPos.first, newPos.second);
            else {
                setDirection(randInt(0, 359));
                updateMovementPlanDist(10 - getMovementPlanDist());
            }
        }

        // else set a random new direction to move in
        else {
            setDirection(randInt(0, 359));
            updateMovementPlanDist(10 - getMovementPlanDist());
        }
    }
}


RegularSalmonella::RegularSalmonella(StudentWorld *sw, double x, double y) : Salmonella(sw, 4, x, y, 1) {}

void RegularSalmonella::doSomething() {
    if (!isAlive())
        return;
    process1();
    process2();
}

AggressiveSalmonella::AggressiveSalmonella(StudentWorld *sw, double x, double y) : Salmonella(sw, 10, x, y, 2) {}

void AggressiveSalmonella::doSomething() {
    if(!isAlive())
        return;
    Direction newD;
    Location newPos;
    bool flag = true; // flag to check if Step 6 from the spec has to be performed
    if (getWorld()->findSocratesNearby(this, 72, newD)) {
        flag = false;
        setDirection(newD);
        getPositionInThisDirection(newD, 3, newPos.first, newPos.second);
        if (!getWorld()->movementOverlap(newPos.first, newPos.second)) {
            moveTo(newPos.first, newPos.second);
        }
    }
    process1();
    if (flag) {
        process2();
    }
}

Ecoli::Ecoli(StudentWorld *sw, double x, double y) : Bacterium(sw, IID_ECOLI, 5, x, y, ECOLI, 4) {}

void Ecoli::doSomething() {
    if(!isAlive())
        return;
    process1();
    Direction newD;

    // if socrates is within 256 pixels, try to get to him
    if (getWorld()->findSocratesNearby(this, 256, newD)) {
        Location newPos;
        int i = 0;
        setDirection(newD);

        // 10 tries to make a move, if not possible, the ecoli stays put
        while (i < 10) {
            getPositionInThisDirection(newD, 2, newPos.first, newPos.second);
            if (getWorld()->movementOverlap(newPos.first, newPos.second)) {
                newD = (newD + 10) % 360;
                setDirection(newD);
            }
            else {
                moveTo(newPos.first, newPos.second);
                return;
            }
            i++;
        }
    }
}


// Auxiliary Functions

// euclidean distance between two points
double distance (double x1, double y1, double x2, double y2) {
    return sqrt(pow(abs(x1 - x2), 2) + pow(abs(y1 - y2), 2));
}

// return x-coordinate from center of dish at a given distance at a given angle
double xCoordinateFromCenter (double dist, int angle) {
    return ((VIEW_WIDTH / 2) + dist * cos (angle * (PI/180)));
}

// same as xCoordinateFromCenter except it returns the y-coordinate
double yCoordinateFromCenter (double dist, int angle) {
    return ((VIEW_HEIGHT / 2) + dist * sin (angle * (PI/180)));
}

// uses the arctan(2) function to compute the direction in which one object lies relative to another
double angleBetweenPositions(double x1, double y1, double x2, double y2) {
    return (atan2(y1 - y2, x1 - x2) * (180 / PI));
}
