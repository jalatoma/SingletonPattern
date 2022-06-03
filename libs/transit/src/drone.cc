#define _USE_MATH_DEFINES
#include "drone.h"

#include <cmath>
#include <limits>
#include <iostream>

#include "AstarStrategy.h"
#include "Beeline.h"
#include "DfsStrategy.h"
#include "DijkstraStrategy.h"
#include "SpinDecorator.h"

Drone::Drone(JsonObject obj) : details(obj) {
  JsonArray pos(obj["position"]);
  position = {pos[0], pos[1], pos[2]};

  JsonArray dir(obj["direction"]);
  direction = {dir[0], dir[1], dir[2]};

  speed = obj["speed"];

  available = true;
  //create an object in the Get Instance function.
  single = Singleton::GetInstance();
  //initialize time, dostance, cost and time_cost to zero.
  time = 0;
  distance = 0;
  cost = 0;
  time_cost = 0;
}

Drone::~Drone() {
  // Delete dynamically allocated variables
}

void Drone::GetNearestEntity(std::vector<IEntity*> scheduler) {
  // IEntity* nearestEntity_ = nullptr;
  float minDis = std::numeric_limits<float>::max();
  for (auto entity : scheduler) {
    if (entity->GetAvailability()) {
      float disToEntity = this->position.Distance(entity->GetPosition());
      if (disToEntity <= minDis) {
        minDis = disToEntity;
        nearestEntity = entity;
      }
    }
  }
  if (nearestEntity) {  // Set strategy for the nearest entity
    nearestEntity->SetAvailability(
        false);  // set availability to the entity that the drone is picking up
    available = false;  // set this drone availability as false
    SetDestination(nearestEntity->GetPosition());
    toTargetPosStrategy =
        new Beeline(this->GetPosition(), nearestEntity->GetPosition());
    std::string targetStrategyName = nearestEntity->GetStrategyName();
    if (targetStrategyName.compare("beeline") == 0) {
      //for each strategy we first clean the file, and if there's already some data stored in the file
      //this function would clean the file for the new data and then add header file.
      //it helps to keep files organize, and seperate data and output of each client.  
        single->cleanFile("Beeline.csv");
        //add the header file to the file.
        single->headerTable("Beeline.csv");
      toTargetDesStrategy = new Beeline(nearestEntity->GetPosition(),
                                        nearestEntity->GetDestination());
      strategyName = "Beeline";
    } else if (targetStrategyName.compare("astar") == 0) {
      single->cleanFile("astar.csv");
      single->headerTable("astar.csv");
      toTargetDesStrategy = new AstarStrategy(
          nearestEntity->GetPosition(), nearestEntity->GetDestination(), graph);
      toTargetDesStrategy =
          new SpinDecorator(toTargetDesStrategy);  // add decorator
      strategyName = "astar";
    } else if (targetStrategyName.compare("dijkstra") == 0) {
      single->cleanFile("dijkstra.csv");
      single->headerTable("dijkstra.csv");
      toTargetDesStrategy = new DijkstraStrategy(
          nearestEntity->GetPosition(), nearestEntity->GetDestination(), graph);
      toTargetDesStrategy =
          new SpinDecorator(toTargetDesStrategy);  // add decorator
      strategyName = "dijkstra";
    } else if (targetStrategyName.compare("dfs") == 0) {
      single->cleanFile("dfs.csv");
      single->headerTable("dfs.csv");
      toTargetDesStrategy = new DfsStrategy(
          nearestEntity->GetPosition(), nearestEntity->GetDestination(), graph);
      toTargetDesStrategy =
          new SpinDecorator(toTargetDesStrategy); 
      strategyName = "dfs";
    } else {
      // If none of the strategy name matched, use beeline as default.
      single->cleanFile("Beeline.csv");
      single->headerTable("Beeline.csv");
      toTargetDesStrategy = new Beeline(nearestEntity->GetPosition(),
                                        nearestEntity->GetDestination());
      strategyName = "Beeline";      
    }
  }
}

void Drone::Update(double dt, std::vector<IEntity*> scheduler) {
  if (available) {
    GetNearestEntity(scheduler);
  }
  if (toTargetPosStrategy) {  // Move drone toward the entity's position
    toTargetPosStrategy->Move(this, dt);
    if (toTargetPosStrategy->IsCompleted()) {
      delete toTargetPosStrategy;
      toTargetPosStrategy = NULL;
    }
  } else if (toTargetDesStrategy) {  // Move drone and entity toward the
                                     // entity's destination
    toTargetDesStrategy->Move(this, dt);
    nearestEntity->SetPosition(this->GetPosition());
    nearestEntity->SetDirection(this->GetDirection());
    if (!toTargetDesStrategy->IsCompleted()) {
      //the filename is strategy name which could be beeline, dfs, dijkstra and AStar, 
      //and the comma-separated values is format of file
        fileName = strategyName+".csv";
        // I used time = 0 here because in the first run it will be 0 so distance should be zero and nothing is calculated, yet.

        if (time == 0){     
          previousPos = this->GetPosition();
          //call the writeToFile function with previousPos for the first time and time, cost and distance is zero for first time.
          single->writeToFile(fileName, previousPos, 0, 0, 0);
          //calculat the distance and added (store) to the distance variable 
          distance += (this->GetPosition()).Distance(previousPos);
        } 
        //Each time, time variable will increment, and it will add dt and update the time 
        time += dt;
       // getting the vector of the difference of current and last points
        Vector3 difference = this->GetPosition() - previousPos; 
        // checking for the repeated spin decorator locations that get exported to the csv and doesn't export them
        if (difference.Magnitude() >= 0.001){        
          distance += (this->GetPosition()).Distance(previousPos);
          previousPos = this->GetPosition();
          //Base Fare ($3)+ (Cost per minute (0.40)* time in ride(dt)) + (Cost per mile ($1 or $2)* ride distance) + Booking Fee ($2)= Your Fare
          // cost += (dt * 0.20);     // we will use conversion from google maps (below)
          time_cost += .20*dt;
          cost = (time_cost + 1.50*(distance * 0.000717) + 5);
          single->writeToFile(fileName, this->GetPosition(), time, distance, cost);
        }
    }
    if (toTargetDesStrategy->IsCompleted()) {
      //when the trip is finished, change time, time_cost and distance to zero.  
      time = 0;
      time_cost = 0;
      distance = 0;
      delete toTargetDesStrategy;
      toTargetDesStrategy = NULL;
      available = true;
      nearestEntity = NULL;
      std::cout<<"COST OF YOUR TRIP IS: "<<std::endl<< cost<<std::endl<<"Using: "<<strategyName<<std::endl<<"Hopefully You Enjoyed Your Trip!"<<std::endl;
      cost = 0;
    }
  }
}

void Drone::Rotate(double angle) {
  direction.x = direction.x * std::cos(angle) - direction.z * std::sin(angle);
  direction.z = direction.x * std::sin(angle) + direction.z * std::cos(angle);
  // direction.y = direction.y * std::sin(angle) + direction.y * std::cos(angle);
}
