#include "simulation_model.h"
#include "DroneFactory.h"
#include "RobotFactory.h"
#include "Beeline.h"
#include "AstarStrategy.h"
#include "Singleton.h"
SimulationModel::SimulationModel(IController& controller)
: controller(controller) {
  compFactory = new CompositeFactory();
  compFactory->AddFactory(new DroneFactory());
  compFactory->AddFactory(new RobotFactory());
  single = Singleton::GetInstance();
}

/// Creates an simulation entity
void SimulationModel::CreateEntity(JsonObject& entity) {
    // int id = entity["entityId"];
    std::string type = entity["type"];
    std::string name = entity["name"];
    JsonArray position = entity["position"];
    std::cout << name << ": " << position << std::endl;
    IEntity* myNewEntity = compFactory->CreateEntity(entity);
    myNewEntity->SetGraph(graph);
    // Add to the drone dictionary
    Drone* drone = dynamic_cast<Drone*>(myNewEntity);
    if (drone) {
      drones.push_back(drone);
    }
    // Add to the robot dictionary
    Robot* robot = dynamic_cast<Robot*>(myNewEntity);
    if (robot) {
      robots.push_back(robot);
    }
    // Call AddEntity to add it to the view
    controller.AddEntity(*myNewEntity);
    entities.push_back(myNewEntity);
}

/// Schedules a trip for an object in the scene
void SimulationModel::ScheduleTrip(JsonObject& details) {
    std::string name = details["name"];
    JsonArray start = details["start"];
    JsonArray end = details["end"];
    // std::string name = details["name"];
    //  std::cout<<"name in the schedule trip function "<<name<<std::endl;
    // start = details["start"];
    // end = details["end"];
    // std::cout<<"start point"<<start<<std::endl;
    //  std::cout<<"end point "<<end<<std::endl;
    std::cout << name << ": " << start << " --> " << end << std::endl;
    for (auto entity : entities) {  // Add the entity to the scheduler
      JsonObject detailsTemp = entity->GetDetails();
      std::string nameTemp = detailsTemp["name"];
      std::string typeTemp = detailsTemp["type"];
      if (name.compare(nameTemp) == 0 &&
      typeTemp.compare("robot") == 0 &&
      entity->GetAvailability()) {
        std::string strategyName = details["search"];
        entity->SetStrategyName(strategyName);
        entity->SetDestination(Vector3(end[0], end[1], end[2]));
        scheduler.push_back(entity);
        break;
      }
    }
    controller.SendEventToView("TripScheduled", details);
    // Add a route to visualize the path
    // controller.AddPath(pathId, path);
}

/// Updates the simulation
void SimulationModel::Update(double dt) {
    //  std::cout<<"start point"<<start<<std::endl;
    //  std::cout<<"end point "<<end<<std::endl;
    //  std::cout<<"Update: " << dt << std::endl;
     
    // Call controller to update changed entities
    for (int i = 0; i < entities.size(); i++) {
       //added
      //picojason::object& temp = entities[i]->GetDetails();
      //if (JsonHelper::GetString[temp, "type"] == "drone"){
        // Drone* nextDrone = dynamic_cast<Drone*>(entities[i]);
        // std::cout<<nextDrone->GetPosition()<<std::endl;
      //}
      //single->writePositionToFile ("myfile.csv", 1, dt);
       //added
      //std::cout<<std::endl;
      //std::cout<<dt<<std::endl;
      // JsonObject obj;
      // JsonArray pos(obj["position"]);
      // Vector3 position = {pos[0], pos[1], pos[2]};
      //std::cout<<pos[0]<<"   "<<pos[1]<<"     "<<pos[2]<<"     "<<std::endl; 
      entities[i]->Update(dt, scheduler);
      controller.UpdateEntity(*entities[i]);
      // single->writePositionToFile("locations.csv", entities[i]->GetPosition(), 0);
      //std::cout << my_Dt<<"   ,"<<dt<<std::endl;
    }
    
    // Remove entites you no longer needJsonArray start;
    // JsonArray end;
}
void SimulationModel::AddFactory(IEntityFactory* factory) {
  compFactory->AddFactory(factory);
}
