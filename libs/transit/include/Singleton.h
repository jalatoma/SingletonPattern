#ifndef SINGLETON_H_
#define SINGLETON_H_
#include <vector>
#include "math/vector3.h"
#include "util/json.h"
using namespace std;
#include <iostream>
class Singleton{
private:
    static Singleton *singleton_;
public:
//constructor
     Singleton(){ }
//destructor
     ~Singleton();
/**
* Singletons should not be cloneable.
*/
     Singleton(const Singleton& singleton) = delete;
/**
* Singletons should not be assignable.
*/
     Singleton& operator=(const Singleton& singleton) = delete;

/**
* This is the static method that controls the access to the singleton
* instance. On the first run, it creates a singleton object and places it
* into the static field. On subsequent runs, it returns the client existing
* object stored in the static field.
*/
     static Singleton* GetInstance();
/*
*we will store all the positions, distances, and time in the file
*with this function
*/
     void writeToFile(std::string filename, Vector3 pos, double time, double dist, double cost);
     //void SourcePoint();
//add the name and title to the file
     void headerTable(std::string filename);
//make clean the file
     void cleanFile (std::string filename);
//we can add a line each time we add data to the file
     void AddLine(std::string filename); 
};
#endif