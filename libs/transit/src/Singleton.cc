#include "Singleton.h"
#include <iostream>
#include <fstream>
//initialize singleton_ variable to null pointer. 
Singleton* Singleton::singleton_ = nullptr;
Singleton::~Singleton() {
    //Delete dynamically
    delete Singleton::singleton_; 
}
/**
* This is the static method that controls the access to the singleton
* instance. On the first run, it creates a singleton object and places it
* into the static field. On subsequent runs, it returns the client existing
* object stored in the static field.
*/
Singleton* Singleton::GetInstance(){
    if (Singleton::singleton_ ==nullptr){
        Singleton::singleton_ = new Singleton();
    }
    return Singleton::singleton_;
}
//Add a header to the table
void Singleton::headerTable(std::string filename){
    std::ofstream myfile;
    myfile.open (filename, std::ios::out|std::ios::app);
    myfile << "  X - AXIS "<<" , "<< " Y - AXIS " << " , "<<" Z - AXIS "<<" , "<<" TIME "<< " ,  " << " DIST "<< " COST "<<std::endl;
    myfile.close();
}
//it will store and write informations and data in the file.
void Singleton::writeToFile (std::string filename, Vector3 pos, double time, double dist, double cost){
  //ios::in	    Open for input operations.
  //ios::out	Open for output operations.
  //ios::app	All output operations are performed at the end of the file, 
  //            appending the content to the current content of the file.
    std::ofstream myfile;
    myfile.open(filename, std::ios::out | std::ios::app );
    myfile << pos[0] <<",  " <<pos[1] <<",  "<<pos [2] << ",   "<<time<<",  "<<dist<< ",  "<< cost<<std::endl;
    //myfile << pos <<time<<std::endl;
    myfile.close();
} 
// this function would clean the file.
void Singleton::cleanFile (std::string filename){
    std::ofstream myfile; 
    //ios::trunc	If the file is opened for output operations and it already existed, 
    //              its previous content is deleted and replaced by the new one.
    myfile.open(filename, std::ios::out | std::ios::trunc);
    //i don't think i need to use the clean function
    myfile.clear();
    myfile.close();
}