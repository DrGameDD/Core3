SetDestination = {}

function SetDestination:checkConditions(pAgent)
	if (pAgent ~= nil) then
		local creature = LuaCreatureObject(pAgent)
		if (creature:getPosture() == UPRIGHT) then
			return true
		end
		-- should also send a stand instruction here (postureRecovery)
	end
	return false
end

function SetDestination:start(pAgent)
	return 0
end

function SetDestination:terminate(pAgent)
	return 0
end

function SetDestination:doAction(pAgent)
	if (pAgent ~= nil) then
		local agent = LuaAiAgent(pAgent)
		if (agent:setDestination() > 0) then
			return BEHAVIOR_SUCCESS
		else
			return BEHAVIOR_FAILURE
		end
	end
	return BEHAVIOR_FAILURE
end

addAiBehavior("SetDestination")