
/*
 * OrderedTask.h
 *
 *  Created on: Apr 14, 2015
 *      Author: TheAnswer
 */

#ifndef SRC_SERVER_ZONE_OBJECTS_SCENE_VARIABLES_ORDEREDTASKEXECUTIONER_H_
#define SRC_SERVER_ZONE_OBJECTS_SCENE_VARIABLES_ORDEREDTASKEXECUTIONER_H_

#include "engine/engine.h"


namespace server {
 namespace zone {
  namespace objects {
   namespace scene {

   class SceneObject;

    namespace variables {

    class OrderedTaskExecutioner : public Task {
    protected:
    	WeakReference<server::zone::objects::scene::SceneObject*> sceneObject;

    	String taskName;
    public:
    	OrderedTaskExecutioner(SceneObject* sceneObject);

    	void run() final;

    	const char* getTaskName() final;
    };

    }
    }
   }
  }
 }


#endif /* SRC_SERVER_ZONE_OBJECTS_SCENE_VARIABLES_ORDEREDTASKEXECUTIONER_H_ */
