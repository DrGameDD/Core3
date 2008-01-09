/*
 *	ResourceManagerImplementationStub
 */
#include "engine/engine.h"

#include "../../ZoneServer.h"
#include "../../ZoneProcessServerImplementation.h"

#include "events/SpawnResourcesEvent.h"

#include "ResourceManager.h"
#include "ResourceManagerImplementation.h"
#include "ResourceImplementation.h"

#include "../../objects/tangible/resource/ResourceContainerImplementation.h"
#include "../../objects/tangible/surveytool/SurveyToolImplementation.h"

#include "../../packets.h"

ResourceManagerImplementation::ResourceManagerImplementation(ZoneServer* inserver, 
		ZoneProcessServerImplementation* inserv) : ResourceManagerServant(), Logger("ResourceManager") {
	init();
	
	serv = inserv;

	spawnResourcesEvent = new SpawnResourcesEvent(this);
	serv->addEvent(spawnResourcesEvent, 1000);
	//  This dictates the first time the spawner will run
	//  Spawner does take a good bit of time to populate an
	//  Empty database.
}
void ResourceManagerImplementation::init() {
	averageShiftTime = 3600000; // In milliseconds
	//  This is the time between each time the Resource Manager schedules
	//  itself to run again.  
	//  *** Default is 1 hour (3600000) ***
	//  *** Good testing time is (15000) ***
	
	aveduration = 86400;  // In seconds
	// This is the modifier for how long spawns are in shift
	// Organics are in shift between (6 * aveduration) and  (22 * aveduration)
	// Inorganics are in shift between (6 * aveduration) and (11 * aveduration)
	// JTL resources are in shift between (13 * aveduration) and (22 * aveduration)
	//  *** Default is 1 day (86400) ***
	//  *** Good testing period is (40) ***
	
	maxspawns = 20;  //  Mmaximum number of spawns per planet
	minspawns = 12;  //  Minimum number of spawns per planet
	maxradius = 2000;   //  Maximum Spawn radius of resource on map
	minradius = 400;    //  Minimum Spawn radius of resource on map
	
	makeMinimumPoolVector();
	makeFixedPoolVector();
	makeNativePoolVector();
}

void ResourceManagerImplementation::theShift() {
	// Much of this method can be removed, the output statements
	// Make it easier to see what the spawner is doing and what
	// To expect what it runs.
	numQueries = 0;
	numFunctions = 0;
	numInsert = 0;
	numNameFunctions = 0;

	info("Starting resource spawner");

	removeExpiredResources();

	checkMinimumPool();
	checkRandomPool();
	checkFixedPool();
	checkNativePool();

	info("Queries run = " + stringify(numQueries));
	info("Insert Queries run = " + stringify(numInsert));
	info("Functions run = " + stringify(numFunctions));
	info("Name functions run = " + stringify(numNameFunctions));
	
	countResources();
	
	info("Resource Spawner Finished");
	
	serv->addEvent(spawnResourcesEvent, averageShiftTime);
}

void ResourceManagerImplementation::countResources() {  
	// This method pure supports out for the above method, is purely cosmetic
	try {
		stringstream query, query2, query3, query4;
		
		query << "SELECT DISTINCT resource_name FROM resource_spawns"
			  << " WHERE pool = 'minimum'";

		ResultSet* res = ServerDatabase::instance()->executeQuery(query);
		
		info("Minimum Pool has " + stringify(res->size()) + "/15 resources");
		delete res;
		
		query2 << "SELECT DISTINCT resource_name FROM resource_spawns"
			   << " WHERE pool = 'random'";

		res = ServerDatabase::instance()->executeQuery(query2);
		
		info("Random Pool has " + stringify(res->size()) + "/24 resources");
		delete res;

		query3 << "SELECT DISTINCT resource_name FROM resource_spawns"
			   << " WHERE pool = 'fixed'";

		res = ServerDatabase::instance()->executeQuery(query3);
		
		info("Fixed Pool has " + stringify(res->size()) + "/22 resources");
		delete res;

		query4 << "SELECT DISTINCT resource_name FROM resource_spawns"
			   << " WHERE pool = 'native'";

		res = ServerDatabase::instance()->executeQuery(query4);
		
		info("Native Pool has " + stringify(res->size()) + "/420 resources");
		delete res;
		
		info("*** Extra resources expected ***");
	} catch(...) {
		cout << "Error in ResourceManagerImplementation::countResources()\n";
	}
}

void ResourceManagerImplementation::clearResources() {
	try {
		numFunctions++;
		
		stringstream query;
		query << "TRUNCATE TABLE `resource_spawns`;";
		query << "TRUNCATE TABLE `resource_data`;";
		
		ServerDatabase::instance()->executeStatement(query);
		
		numQueries++;
	} catch (...) {
		cout << "Error in clearResources\n";
	}
}

float ResourceManagerImplementation::getDensity(int planet, unicode& resname, float inx, float iny) {
	try {
		float density = 0.0f, max_density = 0.0f;
		
		float x, y, radius, source, distance = 0.0f;
		
		bool located = false;
		
		stringstream query;
		query << "SELECT `x`,`y`,`radius`,"
			  << "`max` FROM `resource_spawns` WHERE `resource_name` = '"
			  << resname.c_str() << "' and `planet_id` = " << planet;
		
		ResultSet* res = ServerDatabase::instance()->executeQuery(query);
		
		while (res->next()) {
			x = res->getFloat(0);
			y = res->getFloat(1);

			radius = res->getFloat(2);
			
			source = res->getFloat(3);
			
			if (inx > (x - radius) && inx < x + radius && iny > y - radius && iny < y + radius) {
				distance = sqrt(((fabs(inx) - fabs(x)) * (fabs(inx) - fabs(x))) 
						+ ((fabs(iny) - fabs(y)) * (fabs(iny) - fabs(y))));
				
				density = ((((radius - distance) / radius) * source) / 100.0f);
				
				if (density > max_density)
					max_density = density;
			}
		}
		
		delete res;
		
		return max_density;
	} catch (...) {
		cout << "Database error in getDensity\n";

	}
}

void ResourceManagerImplementation::sendSurveyMessage(Player* player, unicode& resource_name) {
	// Added by Ritter
	Survey* surveyMessage = new Survey();
	
	float player_x = player->getPositionX();
	float player_y = player->getPositionY();
	
	float spacer, x, y, res_percent = 0.0f;
	float wp_x, wp_y, max_res_percent = 0.0f;
	
	int points;
	
	switch (player->getSurveyTool()->getSurveyToolRange()) {
	case 128:
		spacer = 42.6f;
		points = 4;
		x = player_x - (1.5 * spacer);
		y = player_y - (1.5 * spacer);
		break;
	case 192:
		spacer = 64.0f;
		points = 4;
		x = player_x - (1.5 * spacer);
		y = player_y - (1.5 * spacer);
		break;
	case 256:
		spacer = 64.0f;
		points = 5;
		x = player_x - (2 * spacer);
		y = player_y - (2 * spacer);
		break;
	case 320:
		spacer = 80.0f;
		points = 5;
		x = player_x - (2 * spacer);
		y = player_y - (2 * spacer);
		break;
	default:
		spacer = 32.0f;
		points = 3;
		x = player_x - spacer;
		y = player_y - spacer;
		break;
	}
	
	for (int i = 0; i < points; i++) {
		for (int j = 0; j < points; j++) {
			res_percent = getDensity(player->getZoneIndex(), resource_name, x, y);
			
			if (res_percent > max_res_percent) {
				max_res_percent = res_percent;
				wp_x = x;
				wp_y = y;
			}
			
			surveyMessage->add(x, y, res_percent);
			
			x += spacer;
		}
		
		y += spacer;
		x -= (points * spacer);
	}
	
	// Send Survey Results
	player->sendMessage(surveyMessage);
	
	if (max_res_percent >= 0.1f) {
		// Create Waypoint
		if (player->getSurveyWaypoint() != NULL) {
			player->removeWaypoint(player->getSurveyWaypoint());
			player->setSurveyWaypoint(NULL);
		}
		
		WaypointObjectImplementation* wayImpl =	new WaypointObjectImplementation(player, player->getNewItemID());
		wayImpl->setName("Resource Survey");
		wayImpl->setPosition(wp_x, 0.0f, wp_y);

		WaypointObject* waypoint = (WaypointObject*) ObjectRequestBroker::instance()->deploy("Resource Survey", wayImpl);
		waypoint->changeStatus(true);
		
		player->setSurveyWaypoint(waypoint);
		player->addWaypoint(waypoint);
		
		// Send Waypoint System Message
		unicode ustr = "";
		ChatSystemMessage* endMessage = new ChatSystemMessage("survey", "survey_waypoint", ustr, 0, true);
		
		player->sendMessage(endMessage);
	}
}

void ResourceManagerImplementation::sendSampleMessage(Player* player, unicode& resource_name) {
	// Added by Ritter
	float density = getDensity(player->getZoneIndex(), resource_name, player->getPositionX(), player->getPositionY());
	
	if (density < 0.1f) {
		ChatSystemMessage* sysMessage = new ChatSystemMessage("survey", "density_below_threshold", resource_name, 0, false);
		
		player->sendMessage(sysMessage);
		
		player->changePosture(CreatureObjectImplementation::UPRIGHT_POSTURE);
	} else {
		int sampleRate = System::random(1000) + (5 * player->getSkillMod("surveying"));
		
		if (sampleRate >= 650) {
			int resQuantity = (sampleRate / 100) * 2 - System::random(13);
			
			if (!(resQuantity > 0))
				resQuantity = 1;
			
			ChatSystemMessage* sysMessage = new ChatSystemMessage("survey", "sample_located", resource_name, resQuantity, false);
			
			player->getSurveyTool()->sendSampleEffect(player);

			bool rFlag = false;

			Inventory* inventory = player->getInventory();

			for (int i = 0; i < inventory->objectsSize(); i++) {
				TangibleObject* item = (TangibleObject*) inventory->getObject(i);
				
				if (item->isResource()) {
					ResourceContainer* rco = (ResourceContainer*) item;
					
					if (rco->getName().c_str() == resource_name.c_str()) {
						if (rco->getContents() + resQuantity <= rco->getMaxContents()) {
							rco->setContents(rco->getContents() + resQuantity);
							rco->sendDeltas(player);
							rco->generateAttributes(player);
							
							rco->setUpdated(true);
							
							resQuantity = 0;
							
							rFlag = true;
							break;
						} else if (rco->getContents() < rco->getMaxContents()) {
							int diff = (rco->getMaxContents() - rco->getContents());
							
							if (resQuantity <= diff) {
								rco->setContents(rco->getContents() + resQuantity);
							} else {
								rco->setContents(rco->getContents() + diff);
								resQuantity = resQuantity - diff;
							}
							
							rco->sendDeltas(player);
							rco->generateAttributes(player);
							
							rco->setUpdated(true);
						}
					}
				}
			}
			
			if (!rFlag) {
				ResourceContainerImplementation* rcno = new ResourceContainerImplementation(player->getNewItemID());
				rcno->setResourceName(resource_name);
				rcno->setContents(resQuantity);
				
				setResourceData(rcno);
				
				player->addInventoryItem(rcno->deploy());
				
				rcno->sendTo(player);
				
				rcno->setPersistent(false);
			}
			
			player->sendMessage(sysMessage);
		} else {
			ChatSystemMessage* sysMessage = new ChatSystemMessage("survey", "sample_failed", resource_name, 0, false);
			
			player->getSurveyTool()->sendSampleEffect(player);
			
			player->sendMessage(sysMessage);
		}
	}
}

void ResourceManagerImplementation::setResourceData(ResourceContainerImplementation* resContainer) {
	// Added by Ritter
	stringstream query;
	query << "SELECT `INDEX`,`res_cold_resist`,`res_conductivity`,`res_decay_resist`,"
		<< "`res_heat_resist`,`res_malleability`,`res_flavor`,`res_potential_energy`,"
		<< "`res_quality`,`res_shock_resistance`,`res_toughness`,`entangle_resistance`,"
		<< "`resource_type`,`containerCRC` FROM `resource_data` WHERE `resource_name`='"
		<< resContainer->getName().c_str() << "'";
	
	ResultSet* res = ServerDatabase::instance()->executeQuery(query);
	
	if (res->size() != 0) {
		res->next();
		
		resContainer->setResourceID(res->getLong(0));
		resContainer->setColdResistance(res->getInt(1));
		resContainer->setConductivity(res->getInt(2));
		resContainer->setDecayResistance(res->getInt(3));
		resContainer->setHeatResistance(res->getInt(4));
		resContainer->setMalleability(res->getInt(5));
		resContainer->setFlavor(res->getInt(6));
		resContainer->setPotentialEnergy(res->getInt(7));
		resContainer->setQuality(res->getInt(8));
		resContainer->setShockResistance(res->getInt(9));
		resContainer->setToughness(res->getInt(10));
		resContainer->setEntangleResistance(res->getInt(11));
		string cFile = res->getString(12);
		resContainer->setContainerFile(cFile);
		resContainer->setObjectCRC(res->getUnsignedLong(13));
	}
	
	delete res;
}

bool ResourceManagerImplementation::checkResource(Player* player, unicode& resource_name, int SurveyToolType) {
	// Added by Ritter
	stringstream query;
	query << "SELECT DISTINCT rs.INDEX FROM `resource_data` rd "
					<< "INNER JOIN `resource_spawns` rs ON rs.resource_name = rd.resource_name "
					<< "WHERE rs.planet_id = '" << player->getZoneIndex() << "'"
					<< " AND rs.resource_name ='" << resource_name.c_str() << "' AND ";
	 
	switch(SurveyToolType) {
	case SurveyToolImplementation::SOLAR:
		query << "`class_2`='Solar Energy'";
		break;
	case SurveyToolImplementation::CHEMICAL:
		query << "`class_2`='Chemical'";
		break;
	case SurveyToolImplementation::FLORA:
		query << "`class_2`='Flora'";
		break;
	case SurveyToolImplementation::GAS:
		query << "`class_2`='Gas'";
		break;
	case SurveyToolImplementation::GEOTHERMAL:
		query << "`class_2`='Geothermal Energy'";
		break;
	case SurveyToolImplementation::MINERAL:
		query << "(`class_2`='Mineral' OR `class_2`='Radioactive Energy')";
		break;
	case SurveyToolImplementation::WATER:
		query << "`class_2`='Water'";
		break;
	case SurveyToolImplementation::WIND:
		query << "`class_2`='Wind Energy'";
		break;
	default:
		player->error("Invalid Tool");
		return false;
	}
	
	ResultSet* res = ServerDatabase::instance()->executeQuery(query);
	
	int size = res->size();
	
	delete res;
	
	return size != 0;
}

bool ResourceManagerImplementation::sendResourceList(Player* player, int SurveyToolType) {
	// Added by Ritter
	Vector<string> resourceList;
	
	
	uint64 object_id;
	
	string resource_name, resource_type, class_2, surveyType;

	stringstream query;
	query << "SELECT DISTINCT rs.INDEX, rs.resource_name, rd.resource_type"
		  << " FROM `resource_data` rd INNER JOIN `resource_spawns` rs ON "
		  << "rs.resource_name = rd.resource_name "
		  << "WHERE rs.planet_id = '" << player->getZoneID() <<"' AND ";
	
	switch(SurveyToolType) {
	case SurveyToolImplementation::SOLAR:
		query << "`class_2`='Solar Energy'";
		surveyType = "energy_renewable_unlimited_solar";
		break;
	case SurveyToolImplementation::CHEMICAL:
		query << "`class_2`='Chemical'";
		surveyType = "chemical";
		break;
	case SurveyToolImplementation::FLORA:
		query << "`class_2`='Flora'";
		surveyType = "flora_resources";
		break;
	case SurveyToolImplementation::GAS:
		query << "`class_2`='Gas'";
		surveyType = "gas";
		break;
	case SurveyToolImplementation::GEOTHERMAL:
		query << "`class_2`='Geothermal Energy'";
		surveyType = "enegy_renewable_site_limited_geothermal";
		break;
	case SurveyToolImplementation::MINERAL:
		query << "(`class_2`='Mineral' OR `class_2`='Radioactive Energy')";
		surveyType = "mineral";
		break;
	case SurveyToolImplementation::WATER:
		query << "`class_2`='Water'";
		surveyType = "water";
		break;
	case SurveyToolImplementation::WIND:
		query << "`class_2`='Wind Energy'";
		surveyType = "enegy_renewable_unlimited_wind";
		break;
	default:
		player->error("Invalid Tool");
		return false;
	}
	
	ResultSet* res = ServerDatabase::instance()->executeQuery(query);
	
	if (res->size() == 0) {
		player->sendSystemMessage("No resources located on this planet for that tool.");
		
		delete res;
		return false;
	} else {
		ResourceListForSurvey* packet = new ResourceListForSurvey();
		
		while (res->next()) {
			object_id = res->getLong(0);
			
			resource_name = res->getString(1);
			resource_type = res->getString(2);
			
			if (resource_name == "" || !isDuplicate(resourceList, resource_name)) {
				packet->addResource(resource_name,resource_type,object_id);
			}
		}
		
		resourceList.removeAll();
		
		delete res;
		
		packet->finish(surveyType, player->getObjectID());
		
		player->sendMessage(packet);
		
		return true;
	}
}

void ResourceManagerImplementation::sendResourceStats(Player* player, int SurveyToolType) {
	// Added by Ritter
	Vector<string> resourceList;
	
	uint64 object_id;
	
	string resource_name, res_cold_resist, res_conductivity, res_decay_resist, res_heat_resist, res_malleability, res_flavor, res_potential_energy, res_quality, res_shock_resistance, res_toughness, entangle_resistance;

	stringstream query;
	query << "SELECT DISTINCT rs.INDEX, rs.resource_name, rd.res_cold_resist,"
		  << "rd.res_conductivity,rd.res_decay_resist,rd.res_heat_resist,rd.res_malleability,"
		  << "rd.res_flavor,rd.res_potential_energy,rd.res_quality,rd.res_shock_resistance,"
		  << "rd.res_toughness, rd.entangle_resistance FROM `resource_data` rd INNER JOIN `resource_spawns` rs ON "
		  << "rs.resource_name = rd.resource_name "
		  <<"WHERE rs.planet_id = '" << player->getZoneID() <<"' AND ";
	
	switch (SurveyToolType) {
	case SurveyToolImplementation::SOLAR:
		query << "`class_2`='Solar Energy'";
		break;
	case SurveyToolImplementation::CHEMICAL:
		query << "`class_2`='Chemical'";
		break;
	case SurveyToolImplementation::FLORA:
		query << "`class_2`='Flora'";
		break;
	case SurveyToolImplementation::GAS:
		query << "`class_2`='Gas'";
		break;
	case SurveyToolImplementation::GEOTHERMAL:
		query << "`class_2`='Geothermal Energy'";
		break;
	case SurveyToolImplementation::MINERAL:
		query << "(`class_2`='Mineral' OR `class_2`='Radioactive Energy')";
		break;
	case SurveyToolImplementation::WATER:
		query << "`class_2`='Water'";
		break;
	case SurveyToolImplementation::WIND:
		query << "`class_2`='Wind Energy'";
		break;
	default:
		player->error("Invalid Tool");
		return;
	}
	
	ResultSet* res = ServerDatabase::instance()->executeQuery(query);
	
	while (res->next()) {
		object_id = res->getLong(0);
		resource_name = res->getString(1);

		if (resource_name == "" || !isDuplicate(resourceList, resource_name)) {
			res_cold_resist = res->getString(2);
			res_conductivity = res->getString(3);
			res_decay_resist = res->getString(4);
			res_heat_resist = res->getString(5);
			res_malleability = res->getString(6);
			res_flavor = res->getString(7);
			res_potential_energy = res->getString(8);
			res_quality = res->getString(9);
			res_shock_resistance = res->getString(10);
			res_toughness = res->getString(11);
			entangle_resistance = res->getString(12);
			
			AttributeListMessage* packet = new AttributeListMessage(object_id);
			
			if (res_cold_resist != "") {
				packet->insertAttribute("res_cold_resist",res_cold_resist);
			}
			
			if (res_conductivity != "") {
				packet->insertAttribute("res_conductivity",res_conductivity);
			}
			
			if (res_decay_resist != "") {
				packet->insertAttribute("res_decay_resist",res_decay_resist);
			}
			
			if (res_heat_resist != "") {
				packet->insertAttribute("res_heat_resist",res_heat_resist);
			}
			
			if (res_malleability != "") {
				packet->insertAttribute("res_malleability",res_malleability);
			}
			
			if (res_flavor != "") {
				packet->insertAttribute("res_flavor",res_flavor);
			}
			
			if (res_potential_energy != "") {
				packet->insertAttribute("res_potential_energy",res_potential_energy);
			}
			
			if (res_quality != "") {
				packet->insertAttribute("res_quality",res_quality);
			}
			
			if (res_shock_resistance != "") {
				packet->insertAttribute("res_shock_resistance",res_shock_resistance);
			}
			
			if (res_toughness != "") {
				packet->insertAttribute("res_toughness",res_toughness);
			}
			
			if (entangle_resistance != "") {
				packet->insertAttribute("entangle_resistance",entangle_resistance);
			}
			
			player->sendMessage(packet);
		}
	}
	
	resourceList.removeAll();

	delete res;
}

bool ResourceManagerImplementation::isDuplicate(Vector<string>& rList, string& resource) {
	// Added by Ritter
	if (rList.isEmpty()) {
		rList.add(resource);
		return false;
	}
	
	for (int i = 0; i < rList.size(); i++) {
		if (rList.get(i) == resource)
			return true;
	}
	
	rList.add(resource);
	
	return false;
}

void ResourceManagerImplementation::removeExpiredResources() {
	try {
		numFunctions++;

		stringstream query, query2;
		query << "DELETE FROM resource_spawns WHERE despawn < " << (long)time(0);
		
		ServerDatabase::instance()->executeStatement(query);
		
		info("Deleted despawned resources");

		query2 << "UPDATE resource_data rd  "
			   << "INNER JOIN resource_spawns rs "
			   << " ON rd.resource_name = rs.resource_name "
			   << " SET rd.shiftedOut = " << (long)time(0) + (System::random(aveduration) + (averageShiftTime * 2))
			   << " WHERE rs.despawn <  " << (long)time(0) << " and rd.shiftedOut = 0";

		ServerDatabase::instance()->executeStatement(query2);
		
		info("Updated expiring resources with spawnout time");

		numQueries++;
		numQueries++;
	} catch (...) {
		cout << "Database error in removeExpiredResources\n";
	}
}

void ResourceManagerImplementation::checkMinimumPool() {
	numFunctions++;
	
	string restype;
	string exclusion = "";
	
	Vector<string> * spawnMe = new Vector<string>;

	poolNeeds(minimumpool, "minimum", spawnMe);

	info("Minimum pool spawns = " + stringify(spawnMe->size()));

	getFromRandomPool(spawnMe, "minimum");

	for (int x = 0; x < spawnMe->size(); x++) {
		restype = getRandomResourceFromType(spawnMe->get(x), exclusion);
		createResource(restype, "minimum", false);
	}
}

void ResourceManagerImplementation::checkRandomPool() {
	numFunctions++;
	
	string restype;
	string exclusion = " AND (class_1 != 'Organic' and class_5 != 'Iron' and class_3 != 'Fiberplast')";
	string type = "Inorganic";

	int spawnMe = randomPoolNeeds();

	info("Random pool spawns = " + stringify(spawnMe));

	for (int x = 0; x < spawnMe; x++) {
		restype = getRandomResourceFromType(type, exclusion);
		createResource(restype, "random", false);
	}
}

int ResourceManagerImplementation::randomPoolNeeds() {
	stringstream query;
	query << "SELECT DISTINCT rd.resource_name, rd.resource_type, rd.class_1,rd.class_2,rd.class_3,"
		  << "rd.class_4, rd.class_5, rd.class_6, rd.class_7 FROM `resource_data` rd "
		  << "INNER JOIN `resource_spawns` rs ON rs.resource_name = rd.resource_name "
		  << "WHERE rs.pool = 'random'";

	ResultSet* res = ServerDatabase::instance()->executeQuery(query);
	
	int size = res->size();

	numQueries++;

	delete res;
	
	return 24 - size;
}

void ResourceManagerImplementation::checkFixedPool() {
	numFunctions++;
	
	string restype;
	int ironCount = fixedPoolIron();
	
	info("Fixed pool iron spawns = " + stringify(ironCount));

	for (int x = 0; x < ironCount; x++) {
		restype = getRandomResourceFromType(string("Iron"),	string(" AND class_2 != 'Energy'"));
		createResource(restype, "fixed", false);
	}

	Vector<string> * spawnMe = new Vector<string>;

	poolNeeds(fixedpool, "fixed", spawnMe);
	
	info("Fixed pool JTL spawns = " + stringify(spawnMe->size()));

	for (int x = 0; x < spawnMe->size(); x++) {
		createResource(spawnMe->get(x), "fixed", true);
	}
}

int ResourceManagerImplementation::fixedPoolIron() {
	numFunctions++;
	
	stringstream query;
	query << "SELECT DISTINCT rd.resource_name, rd.resource_type, rd.class_1,rd.class_2,rd.class_3,"
		  << "rd.class_4, rd.class_5, rd.class_6, rd.class_7 FROM `resource_data` rd "
		  << "INNER JOIN `resource_spawns` rs ON rs.resource_name = rd.resource_name "
		  << "WHERE rs.pool = 'fixed'  AND rd.class_5 = 'Iron'";

	ResultSet* res = ServerDatabase::instance()->executeQuery(query);
	
	int size = res->size();
	
	delete res;
	
	numQueries++;
	
	return 14 - size;
}

void ResourceManagerImplementation::checkNativePool() {
	numFunctions++;
	
	string restype;
	string exclusion = " AND (class_1 = 'Organic')";
	string type = "Organic";
	
	Vector<string> * spawnMe = new Vector<string>;

	poolNeeds(nativepool, "native", spawnMe);

	info("Native pool spawns = " + stringify(spawnMe->size()));

	getFromRandomPool(spawnMe, "native");
	
	for (int x = 0; x < spawnMe->size(); x++) {
		//cout << spawnMe[x] << endl;
		createResource(spawnMe->get(x), "native", false);
	}
}

void ResourceManagerImplementation::poolNeeds(Vector<string>* invector, string pool, Vector<string>* needs) {
	try {
		numFunctions++;
		
		stringstream query;

		Vector<string> has;
		
		bool hasItem;
		
		if (pool == "minimum") {
			query << "SELECT DISTINCT rd.resource_name, rd.resource_type, rd.class_1,rd.class_2,rd.class_3,"
				  << "rd.class_4, rd.class_5, rd.class_6, rd.class_7 FROM `resource_data` rd "
				  << "INNER JOIN `resource_spawns` rs ON rs.resource_name = rd.resource_name "
				  << "WHERE rs.pool = 'minimum'";
		} else {
			query << "SELECT DISTINCT rd.resource_type FROM `resource_data` rd "
				  << "INNER JOIN `resource_spawns` rs ON rs.resource_name = rd.resource_name "
				  << "WHERE rs.pool = '"<< pool << "'";
		}

		ResultSet* res = ServerDatabase::instance()->executeQuery(query);
		
		numQueries++;
		
		while (res->next()) {
			for (int x = 0; x < invector->size(); x++) {
				if (pool == "minimum") {
					if (res->getString(2) == invector->get(x) || res->getString(3) == invector->get(x) ||
							res->getString(4) == invector->get(x) || res->getString(5) == invector->get(x) ||
							res->getString(6) == invector->get(x) || res->getString(7) == invector->get(x) ||
							res->getString(8) == invector->get(x)) {
						has.add(invector->get(x));
						break;
					}
				} else {
					if (res->getString(0) == invector->get(x)) {
						has.add(invector->get(x));
						break;
					}
				}
			}
		}
		
		for (int x = 0; x < invector->size(); x++) {
			hasItem = false;
			for(int y = 0; y < has.size(); y++) {

				if(has.get(y) == invector->get(x)) {

					hasItem = true;
					break;

				}
			}
			
			if(!hasItem) {
				needs->add(invector->get(x));
			}
		}
		
		delete res;
	} catch (...) {
		cout << "Database error in poolNeeds\n";
	}
}

void ResourceManagerImplementation::getFromRandomPool(Vector<string> * spawnMe,	string pool) {
	numFunctions++;
	
	for (int x = 0; x < spawnMe->size(); x++) {
		if (pool != "native" || (spawnMe->get(x).find("water") != string::npos)) {
			try {
				stringstream query, query2;

				query << "SELECT DISTINCT rd.resource_name FROM `resource_data` rd "
					  << "INNER JOIN `resource_spawns` rs ON rs.resource_name = rd.resource_name "
					  << "WHERE rs.pool = 'random' and shiftedOut = 0 and ("
					  << "resource_type = '" << spawnMe->get(x) << "' OR "
					  << "class_1 = '" << spawnMe->get(x) << "' OR "
					  << "class_2 = '" << spawnMe->get(x) << "' OR "
					  << "class_3 = '" << spawnMe->get(x) << "' OR "
					  << "class_4 = '" << spawnMe->get(x) << "' OR "
					  << "class_5 = '" << spawnMe->get(x) << "' OR "
					  << "class_6 = '" << spawnMe->get(x) << "' OR "
					  << "class_7 = '" << spawnMe->get(x) << "')"
					  << "limit 1";

				ResultSet* res = ServerDatabase::instance()->executeQuery(query);

				numQueries++;

				if (res->size() > 0) {
					res->next();
					string resname = res->getString(0);

					query2 << "UPDATE resource_spawns "
						   << "SET pool = '" << pool << "' "
						   << "WHERE resource_name = '" << resname << "'";

					numQueries++;

					ServerDatabase::instance()->executeStatement(query2);

					spawnMe->remove(x);
					x--;
				}
				
				delete res;
			} catch (...) {
				cout << "Database error in getFromRandomPool\n";
				break;
			}
		}
	}
}

string ResourceManagerImplementation::getRandomResourceFromType(string restype, string exclusion) {
	numFunctions++;
	
	try {
		string outtype = "";
		
		stringstream query;
		query << "SELECT `resource_type`,`class_1`,`class_2`,"
			  << "`class_3`,`class_4`,`class_5`,`class_6`,`class_7`,`weight` "
			  << "FROM `resource_tree` "
			  << "WHERE (class_1 = '" << restype << "' OR "
			  << "class_2 = '" << restype << "' OR "
			  << "class_3 = '" << restype << "' OR "
			  << "class_4 = '" << restype << "' OR "
			  << "class_5 = '" << restype << "' OR "
			  << "class_6 = '" << restype << "' OR "
			  << "class_7 = '" << restype << "') AND ("
			  << "class_6 != 'JTL')" << exclusion;

		ResultSet* res = ServerDatabase::instance()->executeQuery(query);
		
		numQueries++;

		// This is where the weighted code goes to spawn resource on
		// Rarity rather than jus t randomly

		int y = (System::random(res->size()-1));

		for (int x = 0; x < res->size(); x++) {
			res->next();
			
			if (x == y)
				return res->getString(0);
		}
		
		delete res;
		
		return "";
	} catch (...) {
		cout << "Database error in getRandomResourceFromType\n";
	}
}

void ResourceManagerImplementation::createResource(string restype, string pool, bool jtl) {
	numFunctions++;
	
	string resname;
	
	int planets[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	
	int planet, therand, thex, they, numplanets;

	ResourceImplementation* resource = new ResourceImplementation(restype);

	generateResourceStats(resource);

	insertResource(resource);

	if (isPlanetSpecific(resource->getType())) {
		planet = getPlanet(resource->getType());

		if (resource->getClass2()== "Creature Resources") {
			insertSpawn(resource, planet, 0, 0, 0, 0, pool, jtl);
		} else {
			for (int y = 0; y < (System::random(maxspawns - minspawns) + minspawns); y++) {
				thex = System::random(8192 * 2) - 8192;
				they = System::random(8192 * 2) - 8192;

				insertSpawn(resource, planet, thex, they, (System::random(maxradius - minradius) + minradius),
						(System::random(49) + 50), pool, jtl);
			}
		}
	} else {
		numplanets = System::random(resource->getMaxPool() - resource->getMinPool()) + resource->getMinPool();

		for (int x = 0; x < numplanets; x++) {
			for (int z = 0; z < 10; z++) {
				therand = System::random(9);
				planet = planets[therand];

				if (planet != 0) {
					planet--;
					planets[therand] = 0;
					break;
				}
			}

			for (int y = 0; y < (System::random(maxspawns - minspawns)
					+ minspawns); y++) {
				thex = System::random(8192 * 2) - 8192;
				they = System::random(8192 * 2) - 8192;

				insertSpawn(resource, planet, thex, they,
						(System::random(maxradius - minradius)+ minradius),
						(System::random(49) + 50), pool, jtl);
			}
		}
	}
}

void ResourceManagerImplementation::generateResourceStats(ResourceImplementation * resource) {
	string resname;

	try {
		string query = "SELECT * FROM resource_tree WHERE resource_type = \'" +
		resource->getType() + "\'";

		ResultSet* res = ServerDatabase::instance()->executeQuery(query);

		while (res->next()) {
			if (res->size() == 1) {
				resource->setClass1(res->getString(2));
				resource->setClass2(res->getString(3));
				resource->setClass3(res->getString(4));
				resource->setClass4(res->getString(5));
				resource->setClass5(res->getString(6));
				resource->setClass6(res->getString(7));
				resource->setClass7(res->getString(8));

				resource->setMaxType(res->getInt(9));
				resource->setMinType(res->getInt(10));
				resource->setMinPool(res->getInt(11));
				resource->setMaxPool(res->getInt(12));

				resource->setAtt1(res->getString(13));
				resource->setAtt2(res->getString(14));
				resource->setAtt3(res->getString(15));
				resource->setAtt4(res->getString(16));
				resource->setAtt5(res->getString(17));
				resource->setAtt6(res->getString(18));
				resource->setAtt7(res->getString(19));
				resource->setAtt8(res->getString(20));
				resource->setAtt9(res->getString(21));
				resource->setAtt10(res->getString(22));
				resource->setAtt11(res->getString(23));

				resource->setAtt1Stat((System::random(res->getInt(25) - res->getInt(24)) + res->getInt(24)));
				resource->setAtt2Stat((System::random(res->getInt(27) - res->getInt(26)) + res->getInt(26)));
				resource->setAtt3Stat((System::random(res->getInt(29) - res->getInt(28)) + res->getInt(28)));
				resource->setAtt4Stat((System::random(res->getInt(31) - res->getInt(30)) + res->getInt(30)));
				resource->setAtt5Stat((System::random(res->getInt(33) - res->getInt(32)) + res->getInt(32)));
				resource->setAtt6Stat((System::random(res->getInt(35) - res->getInt(34)) + res->getInt(34)));
				resource->setAtt7Stat((System::random(res->getInt(37) - res->getInt(36)) + res->getInt(36)));
				resource->setAtt8Stat((System::random(res->getInt(39) - res->getInt(38)) + res->getInt(38)));
				resource->setAtt9Stat((System::random(res->getInt(41) - res->getInt(40)) + res->getInt(40)));
				resource->setAtt10Stat((System::random(res->getInt(43) - res->getInt(42)) + res->getInt(42)));
				resource->setAtt11Stat((System::random(res->getInt(45) - res->getInt(44)) + res->getInt(44)));

				resource->setContainer(res->getString(46));
				resource->setContainerCRC(res->getUnsignedInt(47));
			} else {
				cout << "Resouce Database error" << endl;
			}
		}
		
		makeResourceName(resname, resource->getClass1() == "Organic");
		
		resource->setName(resname);

		delete res;
	} catch (DatabaseException& e) {
	}
}

void ResourceManagerImplementation::makeMinimumPoolVector() {
	numFunctions++;
	
	try {
		minimumpool = new Vector<string>;
		minimumpool->add("Steel");
		minimumpool->add("Copper");
		minimumpool->add("Aluminum");
		minimumpool->add("Extrusive Ore");
		minimumpool->add("Intrusive Ore");
		minimumpool->add("Carbonate Ore");
		minimumpool->add("Crystalline Gemstone");
		minimumpool->add("Amorphous Gemstone");
		minimumpool->add("Known Radioactive");
		minimumpool->add("Solid Petrochem Fuel");
		minimumpool->add("Liquid Petrochem Fuel");
		minimumpool->add("Polymer");
		minimumpool->add("Polymer");
		minimumpool->add("Lubricating Oil");
		minimumpool->add("Lubricating Oil");
	} catch (...) {
		cout << "Error in makeMinimumPoolVector\n";
	}
}

void ResourceManagerImplementation::makeFixedPoolVector() {
	numFunctions++;
	
	fixedpool = new Vector<string>;
	fixedpool->add("steel_arveshian");
	fixedpool->add("steel_bicorbantium");
	fixedpool->add("copper_borocarbitic");
	fixedpool->add("ore_siliclastic_fermionic");
	fixedpool->add("aluminum_perovskitic");
	fixedpool->add("gas_reactive_organometallic");
	fixedpool->add("fiberplast_gravitonic");
	fixedpool->add("radioactive_polymetric");
}

void ResourceManagerImplementation::makeNativePoolVector() {
	numFunctions++;

	try {
		nativepool = new Vector<string>;

		numQueries++;

		stringstream query;
		query << "SELECT `resource_type` "
			  << "FROM `resource_tree` "
			  << "WHERE (class_1 = 'Organic' OR "
			  << "class_3 = 'Fiberplast' OR "
			  << "class_2 = 'Wind Energy' OR "
			  << "class_2 = 'Solar Energy' OR "
			  << "class_2 = 'Water') AND class_6 != 'JTL'";
		
		ResultSet* res = ServerDatabase::instance()->executeQuery(query);
		
		string temp;
		
		while (res->next()) {
			nativepool->add(res->getString(0));
		}
		
		delete res;
	} catch (...) {
		cout << "Database error in makeNativePoolVector\n";
	}
}
bool ResourceManagerImplementation::isPlanetSpecific(const string type) {
	numFunctions++;
	
	if ((type.find("corellia", 0) == string::npos) && (type.find("dantooine", 0) == string::npos) 
			&& (type.find("yavin4", 0) == string::npos) && (type.find("dathomir", 0) == string::npos) 
			&& (type.find("endor", 0) == string::npos) && (type.find("lok", 0) == string::npos) 
			&& (type.find("naboo", 0) == string::npos) && (type.find("rori", 0) == string::npos)
			&& (type.find("talus", 0) == string::npos) && (type.find("tatooine", 0) == string::npos))
		return false;
	else
		return true;
}
int ResourceManagerImplementation::getPlanet(const string type) {
	numFunctions++;
	
	if (type.find("corellia", 0) != string::npos)
		return 0;
	
	if (type.find("dantooine", 0) != string::npos)
		return 1;
	
	if (type.find("dathomir", 0) != string::npos)
		return 2;
	
	if (type.find("endor", 0) != string::npos)
		return 3;
	
	if (type.find("lok", 0) != string::npos)
		return 4;
	
	if (type.find("naboo", 0) != string::npos)
		return 5;
	
	if (type.find("rori", 0) != string::npos)
		return 6;
	
	if (type.find("talus", 0) != string::npos)
		return 7;
	
	if (type.find("tatooine", 0) != string::npos)
		return 8;
	
	if (type.find("yavin4", 0) != string::npos)
		return 9;
	
	return 99;
}
bool ResourceManagerImplementation::isType(ResourceImplementation* resource, string type) {
	numFunctions++;
	
	if (resource->getClass1() == type || resource->getClass2() == type
			|| resource->getClass3() == type || resource->getClass4() == type
			|| resource->getClass5() == type || resource->getClass6() == type
			|| resource->getClass7() == type)
		return true;
	else
		return false;
}

void ResourceManagerImplementation::insertResource(ResourceImplementation* resource) {
	numFunctions++;
	
	try {
		stringstream query;
		query << "INSERT INTO `resource_data` "
			  << "(`resource_name`,`resource_type`,`class_1`,"
			  << "`class_2`,`class_3`,`class_4`,"
			  << "`class_5`,`class_6`,`class_7`"
			  << checkInsertCategory(resource->getAtt1())
			  << checkInsertCategory(resource->getAtt2())
			  << checkInsertCategory(resource->getAtt3())
			  << checkInsertCategory(resource->getAtt4())
			  << checkInsertCategory(resource->getAtt5())
			  << checkInsertCategory(resource->getAtt6())
			  << checkInsertCategory(resource->getAtt7())
			  << checkInsertCategory(resource->getAtt8())
			  << checkInsertCategory(resource->getAtt9())
			  << checkInsertCategory(resource->getAtt10())
			  << checkInsertCategory(resource->getAtt11())
			  << ",`shiftedIn`,`container`, `containerCRC`)"
			  << " VALUES ('"
			  << resource->getName() << "','" << resource->getType() << "','"
			  << resource->getClass1() << "','" << resource->getClass2() << "','"
			  << resource->getClass3() << "','" << resource->getClass4() << "','"
			  << resource->getClass5() << "','" << resource->getClass6() << "','"
			  << resource->getClass7() << "'"
			  << checkInsertValue(resource->getAtt1Stat())
			  << checkInsertValue(resource->getAtt2Stat())
			  << checkInsertValue(resource->getAtt3Stat())
			  << checkInsertValue(resource->getAtt4Stat())
			  << checkInsertValue(resource->getAtt5Stat())
			  << checkInsertValue(resource->getAtt6Stat())
			  << checkInsertValue(resource->getAtt7Stat())
			  << checkInsertValue(resource->getAtt8Stat())
			  << checkInsertValue(resource->getAtt9Stat())
			  << checkInsertValue(resource->getAtt10Stat())
			  << checkInsertValue(resource->getAtt11Stat())
			  << ", " << (long)time(0) << ",'" << resource->getContainer()
			  << "'," << resource->getContainerCRC()<< ")";

		ServerDatabase::instance()->executeStatement(query);
		
		numInsert++;
	} catch (...) {
		cout << "Insert Resource Failed " << endl;
	}
}
void ResourceManagerImplementation::insertSpawn(ResourceImplementation* resource, int planet_id, 
		float x, float y, float radius, float max, string pool, bool& jtl) {
	numFunctions++;
	
	try {
		int upper, lower;
		
		if (resource->getClass1() == "Organic") {
			upper = 22;
			lower = 6;
		} else {
			if(jtl) {
				upper = 22;
				lower = 13;
			} else {
				upper = 11;
				lower = 6;
			}
		}
		
		long despawn = (long)time(0) + (System::random((aveduration * upper)-(aveduration * lower)))
			+ (aveduration* lower);
		
		stringstream query;
		query << "INSERT INTO `resource_spawns` "
			  << "(`resource_name`, `planet_id`,"
			  << "`x`,`y`,`radius`,"
			  << "`max`,`despawn`,`pool`)"
			  << " VALUES ('"
			  << resource->getName() << "'," << planet_id << ","
			  << x << "," << y << ","<< radius
			  << "," << max << "," << despawn << ",'" << pool << "')";

		ServerDatabase::instance()->executeStatement(query);
		
		numInsert++;
	} catch (...) {
		cout << "Insert Spawn Failed " << endl;
	}
}

inline string ResourceManagerImplementation::checkInsertCategory(string instring) {
	numFunctions++;

	if (instring == "") {
		return "";
	} else {
		return ",`" + instring + "`";
	}
}

inline string ResourceManagerImplementation::checkInsertValue(int inval) {
	numFunctions++;
	
	if (inval == 0) {
		return "";
	} else {
		return ", " + stringify(inval);// + "`";
	}
}

void ResourceManagerImplementation::makeResourceName(string& resname, bool isOrganic) {
	numNameFunctions++;

	int nameLength = 4 + System::random(10);
	char name[nameLength];

	while (true) {
		int x = 0;

		if (System::random(2) == 1 && nameLength > 5 && !isOrganic) {
			x = addPrefix(name);
		} else {
			name[0] = chooseNextLetter(' ', ' ');
			name[1] = chooseNextLetter(name[0], ' ');

			x = 2;
		}

		for (; x < nameLength + 1; x++) {
			if (x < nameLength) {
				name[x] = chooseNextLetter(name[x-1], name[x-2]);
			} else {
				if (System::random(1) == 1 && !isOrganic && !isVowel(name[x-1])	&& name[x-1] != 'q')
					addSuffix(name, x);
				else
					name[x] = '\0';

				break;
			}
		}

		name[0] = toupper(name[0]);
		if (checkResourceName(name) && !isProfanity(name))
			break;
	}
	
	resname = name;
}

bool ResourceManagerImplementation::checkResourceName(const string instring) {
	try {
		numFunctions++;
		
		stringstream query;
		query << "SELECT * from `resource_data` WHERE resource_name = \'" + instring + "\'";

		ResultSet* res = ServerDatabase::instance()->executeQuery(query);
		
		numQueries++;
		
		int size = res->size();
		
		delete res;
		
		if (size == 0) {
			return true;
		} else {
			//cout << "Duplicate Name, OMG?!?!" << endl;
			return false;
		}
	} catch (DatabaseException& e) {
		return false;
	}
}

char ResourceManagerImplementation::chooseNextLetter(const char lastLetter, const char letterBeforeLast) {
	numNameFunctions++;

	if (letterBeforeLast == ' ' && lastLetter == ' ')
		return 97 + System::random(25);

	if ((!isVowel(lastLetter) && !isVowel(letterBeforeLast) && letterBeforeLast	!= ' ') 
			|| (lastLetter == 'u' && letterBeforeLast == 'q')) {
		char exclusion[] = { 'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm',
				'n', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
				'\0' };

		return chooseLetterExcluding(exclusion);
	}

	if ((isVowel(lastLetter) && isVowel(letterBeforeLast) && letterBeforeLast != ' ')) {
		char exclusion[] = { 'a', 'e', 'i', 'o', 'u', 'y', '\0' };

		return chooseLetterExcluding(exclusion);
	} else {
		switch (lastLetter) {
		case 'a': {
			char exclusion[] = { 'a', 'e', '\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 'b': {
			char exclusion[] = { 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'j', 'k',
					'm', 'n', 'p', 'q', 's', 't', 'v', 'w', 'x', 'z', '\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 'c': {
			char exclusion[] = { 'b', 'c', 'd', 'f', 'g', 'j', 'k', 'm', 'n',
					'p', 'q', 's', 't', 'v', 'w', 'x', 'z', '\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 'd': {
			char exclusion[] = { 'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm',
					'n', 'p', 'q', 's', 't', 'v', 'w', 'x', 'y', 'z', '\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 'f': {
			char exclusion[] = { 'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'm',
					'n', 'p', 'q', 's', 't', 'v', 'w', 'x', 'z', '\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 'g': {
			char exclusion[] = { 'b', 'c', 'd', 'f', 'g', 'j', 'k', 'm', 'n',
					'p', 'q', 's', 't', 'v', 'w', 'x', 'z', '\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 'h': {
			char
					exclusion[] = { 'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k',
							'l', 'm', 'n', 'p', 'q', 'r', 's', 't', 'v', 'w',
									'x', 'z', '\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 'i': {
			char exclusion[] = { 'i', 'j', 'u', '\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 'j': {
			char exclusion[] = { 'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
					'm', 'n', 'p', 'q', 'r', 's', 't', 'v', 'w', 'x', 'y', 'z',
					'\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 'k': {
			char exclusion[] = { 'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'm',
					'n', 'p', 'q', 's', 't', 'v', 'w', 'x', 'z', '\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 'l': {
			char
					exclusion[] = { 'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k',
							'l', 'm', 'n', 'p', 'q', 'r', 's', 't', 'v', 'w',
									'x', 'z', '\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 'm': {
			char exclusion[] = { 'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
					'm', 'n', 'p', 'q', 'r', 's', 't', 'v', 'w', 'x', 'y', 'z',
					'\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 'n': {
			char exclusion[] = { 'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
					'm', 'n', 'p', 'q', 'r', 's', 't', 'v', 'w', 'x', 'y', 'z',
					'\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 'p': {
			char exclusion[] = { 'b', 'c', 'd', 'f', 'g', 'j', 'k', 'm', 'n',
					'p', 'q', 's', 't', 'v', 'w', 'x', 'z', '\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 'q':
			return 'u';
		case 'r': {
			char
					exclusion[] = { 'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k',
							'l', 'm', 'n', 'p', 'q', 'r', 's', 't', 'v', 'w',
									'x', 'z', '\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 's': {
			char exclusion[] = { 'b', 'd', 'f', 'g', 'j', 'v', 'x', 'y', 'z',
					'\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 't': {
			char exclusion[] = { 'b', 'c', 'd', 'f', 'g', 'j', 'k', 'l', 'm',
					'n', 'p', 'q', 't', 'v', 'x', 'y', 'z', '\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 'u': {
			char exclusion[] = { 'a', 'b', 'd', 'e', 'h', 'i', 'm', 'o', 'r',
					'u', 'v', 'x', 'y', 'z', '\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 'v': {
			char
					exclusion[] = { 'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k',
							'l', 'm', 'n', 'p', 'q', 'r', 's', 't', 'v', 'w',
									'x', 'z', '\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 'w': {
			char exclusion[] = { 'b', 'c', 'd', 'f', 'g', 'j', 'k', 'l', 'm',
					'n', 'p', 'q', 's', 't', 'v', 'w', 'x', 'z', '\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 'x': {
			char
					exclusion[] = { 'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k',
							'l', 'm', 'n', 'p', 'q', 'r', 's', 't', 'v', 'w',
									'x', 'z', '\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 'y': {
			char exclusion[] = { 'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
					'm', 'n', 'p', 'q', 'r', 's', 't', 'v', 'w', 'x', 'y', 'z',
					'\0' };
			return chooseLetterExcluding(exclusion);
		}
		case 'z': {
			char exclusion[] = { 'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
					'm', 'n', 'p', 'q', 'r', 's', 't', 'v', 'w', 'x', 'y', 'z',
					'\0' };
			return chooseLetterExcluding(exclusion);
		}
		default:
			return 97 + System::random(25);
		}
	}
}
bool ResourceManagerImplementation::isDumbPhrase(const string inname) {
	/*if ((inname.find("riy") == string::npos) && 
			(inname.find("riy") == string::npos) && 
			(inname.find("riy") == string::npos) && 
			(inname.find("riy") == string::npos) && 
			(inname.find("riy") == string::npos) && 
			(inname.find("riy") == string::npos) && 
			(inname.find("riy") == string::npos) && 
			(inname.find("riy") == string::npos) && 
			(inname.find("riy") == string::npos) && 
			(inname.find("riy") == string::npos) && 
			(inname.find("riy") == string::npos) && 
			(inname.find("anal") == string::npos))
		return true;
	else
		return false;*/
	
}
bool ResourceManagerImplementation::isProfanity(const string inname) {
	if ((inname.find("anal") == string::npos) && (inname.find("ass")
			== string::npos) && (inname.find("asshole") == string::npos)
			&& (inname.find("assmunch") == string::npos)
			&& (inname.find("asswhole") == string::npos)
			&& (inname.find("biatch") == string::npos) && (inname.find("bitch")
			== string::npos) && (inname.find("boob") == string::npos)
			&& (inname.find("christ") == string::npos) && (inname.find("clit")
			== string::npos) && (inname.find("cock") == string::npos)
			&& (inname.find("cum") == string::npos)
			&& (inname.find("cumbubble") == string::npos)
			&& (inname.find("cumlicker") == string::npos)
			&& (inname.find("cunt") == string::npos) && (inname.find("damn")
			== string::npos) && (inname.find("devil") == string::npos)
			&& (inname.find("dick") == string::npos)
			&& (inname.find("dingleberry") == string::npos)
			&& (inname.find("dong") == string::npos) && (inname.find("dumbass")
			== string::npos) && (inname.find("fag") == string::npos)
			&& (inname.find("faggot") == string::npos) && (inname.find("fagot")
			== string::npos) && (inname.find("fuk") == string::npos)
			&& (inname.find("fuxxor") == string::npos)
			&& (inname.find("george lucas") == string::npos)
			&& (inname.find("gook") == string::npos) && (inname.find("hooker")
			== string::npos) && (inname.find("jackoff") == string::npos)
			&& (inname.find("jesus") == string::npos) && (inname.find("jew")
			== string::npos) && (inname.find("jizz") == string::npos)
			&& (inname.find("kike") == string::npos) && (inname.find("kunt")
			== string::npos) && (inname.find("labia") == string::npos)
			&& (inname.find("masterbait") == string::npos)
			&& (inname.find("masterbate") == string::npos)
			&& (inname.find("masturbait") == string::npos)
			&& (inname.find("nazi") == string::npos) && (inname.find("negro")
			== string::npos) && (inname.find("nigg") == string::npos)
			&& (inname.find("nigger") == string::npos)
			&& (inname.find("nipple") == string::npos) && (inname.find("penis")
			== string::npos) && (inname.find("piss") == string::npos)
			&& (inname.find("prick") == string::npos) && (inname.find("pussy")
			== string::npos) && (inname.find("satan") == string::npos)
			&& (inname.find("sex") == string::npos) && (inname.find("shit")
			== string::npos) && (inname.find("skank") == string::npos)
			&& (inname.find("slut") == string::npos)
			&& (inname.find("spawnthemonkey") == string::npos)
			&& (inname.find("spick") == string::npos) && (inname.find("sucks")
			== string::npos) && (inname.find("tit") == string::npos)
			&& (inname.find("tits") == string::npos) && (inname.find("titt")
			== string::npos) && (inname.find("twat") == string::npos)
			&& (inname.find("vagina") == string::npos) && (inname.find("whore")
			== string::npos) && (inname.find("wtf") == string::npos))
		return false;
	else
		return true;
}

inline bool ResourceManagerImplementation::isVowel(const char inChar) {
	numNameFunctions++;
	
	if (inChar == 'a' || inChar == 'e' || inChar == 'i' || inChar == 'o'
			|| inChar == 'u' || inChar == 'y')
		return true;
	else
		return false;
}

inline char ResourceManagerImplementation::chooseLetterExcluding(const char exclude[]) {
	numNameFunctions++;
	
	char x = 97 + System::random(25);

	for (int i = 0; i < 25; i++) {
		if (x == exclude[i]) {
			x = 97 + System::random(25);

			i = -1;
		}
	}
	
	return x;
}

inline int ResourceManagerImplementation::addPrefix(char* name) {
	numNameFunctions++;
	
	int x = 1 + System::random(4);

	switch (x) {
	case 1:
		name[0] = 'c';
		name[1] = 'a';
		name[2] = 'r';
		name[3] = 'b';
		return 4;
	case 2:
		name[0] = 'd';
		name[1] = 'u';
		name[2] = 'r';
		return 3;
	case 3:
		name[0] = 'o';
		name[1] = 'm';
		name[2] = 'n';
		name[3] = 'i';
		return 4;
	case 4:
		name[0] = 'q';
		name[1] = 'u';
		name[2] = 'a';
		name[3] = 'd';
		return 4;
	case 5:
		name[0] = 't';
		name[1] = 'r';
		name[2] = 'i';
		return 3;
	default:
		return 0;
	}
}

inline void ResourceManagerImplementation::addSuffix(char* name, int location) {
	numNameFunctions++;
	
	int x = 1 + System::random(7);

	switch (x) {
	case 1:
		name[location] = 'i';
		name[location+1] = 'u';
		name[location+2] = 'm';
		name[location+3] = '\0';
		break;
	case 2:
		name[location] = 'i';
		name[location+1] = 'a';
		name[location+2] = 'n';
		name[location+3] = '\0';
		break;
	case 3:
		name[location] = 'i';
		name[location+1] = 's';
		name[location+2] = 'm';
		name[location+3] = '\0';
		break;
	case 4:
		name[location] = 'i';
		name[location+1] = 't';
		name[location+2] = 'e';
		name[location+3] = '\0';
		break;
	case 5:
		name[location] = 's';
		name[location+1] = 'i';
		name[location+2] = 's';
		name[location+3] = '\0';
		break;
	case 6:
		name[location] = 'i';
		name[location+1] = 'n';
		name[location+2] = 'e';
		name[location+3] = '\0';
		break;
	case 7:
		name[location] = 'i';
		name[location+1] = 'c';
		name[location+2] = '\0';
		break;
	case 8:
		name[location] = 'i';
		name[location+1] = 'd';
		name[location+2] = 'e';
		name[location+3] = '\0';
		break;
	default:
		break;
	}
}

inline string ResourceManagerImplementation::stringify(const int x) {
	numNameFunctions++;
	ostringstream o;

	if (!(o << x))
		return "";
	else
		return o.str();
}

