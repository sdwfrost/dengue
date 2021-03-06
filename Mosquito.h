// Mosquito.h
// A single infected mosquito.
// This just takes care of mosquito lifespan, infecting serotype
// Movement is handled by other classes (Community)
#ifndef __MOSQUITO_H
#define __MOSQUITO_H
#include "Parameters.h"
#include "Location.h" 

class Location;
//Location::removeInfectedMosquito();
//Location::addInfectedMosquito();

struct RestoreMosquitoPars {
    RestoreMosquitoPars() : location(nullptr), serotype((Serotype) 0), age_infected(0), age_infectious(0), age_dead(0) {};
    RestoreMosquitoPars(Location* l, Serotype s, int aid, int ais, int ad) : location(l), serotype(s), age_infected(aid), age_infectious(ais), age_dead(ad) {};
    Location* location;
    Serotype serotype;
    int age_infected;
    int age_infectious;
    int age_dead;
};

class Mosquito {
    public:
        Mosquito();
        Mosquito(Location* p, Serotype s, int nInfectedAtID, int nExternalIncubationPeriod, double prob_infecting_bite);
        Mosquito(RestoreMosquitoPars* pars);

        virtual ~Mosquito();
        int getID() const { return _nID; }
        Location* getLocation() const { return _pLocation; }
        void setLocation(Location *p) { _pLocation = p; }
        void updateLocation(Location *p) { _pLocation->removeInfectedMosquito(); setLocation(p); p->addInfectedMosquito(); }
        Location* getOriginLocation() const { return _pOriginLocation; }
        int getAgeInfected() const { return _nAgeInfected; }
        int getAgeInfectious() const { return _nAgeInfectious; }
        int getAgeDeath() const { return _nAgeDeath; }
        bool isDead() const { return _bDead; }
        Serotype getSerotype() const { return _eSerotype; }


    protected:
        int _nID;                                                     // unique identifier
        Location* _pLocation;                                         // pointer to present location
        Location* _pOriginLocation;                                   // pointer to origin (where infected) location
        Serotype _eSerotype;                                          // infecting serotype
        int _nAgeInfected;                                            // age when infected in days
        int _nAgeInfectious;                                          // age when infectious in days
        int _nAgeDeath;                                               // lifespan in days
        bool _bDead;                                                  // is dead?
        int _nInfectedAtID;                                           // location ID where infected
        static int _nNextID;                                          // unique ID to assign to the next Mosquito allocated
};
#endif
