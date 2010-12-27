/*
 *	server/zone/objects/creature/junkdealer/JunkdealerCreature.cpp generated by engine3 IDL compiler 0.60
 */

#include "JunkdealerCreature.h"

#include "server/zone/Zone.h"

#include "server/zone/objects/player/PlayerCreature.h"

#include "server/zone/objects/scene/SceneObject.h"

/*
 *	JunkdealerCreatureStub
 */

JunkdealerCreature::JunkdealerCreature() : CreatureObject(DummyConstructorParameter::instance()) {
	JunkdealerCreatureImplementation* _implementation = new JunkdealerCreatureImplementation();
	_impl = _implementation;
	_impl->_setStub(this);
}

JunkdealerCreature::JunkdealerCreature(DummyConstructorParameter* param) : CreatureObject(param) {
}

JunkdealerCreature::~JunkdealerCreature() {
}


void JunkdealerCreature::loadTemplateData(SharedObjectTemplate* templateData) {
	JunkdealerCreatureImplementation* _implementation = (JunkdealerCreatureImplementation*) _getImplementation();
	if (_implementation == NULL) {
		throw ObjectNotLocalException(this);

	} else
		_implementation->loadTemplateData(templateData);
}

void JunkdealerCreature::activateRecovery() {
	JunkdealerCreatureImplementation* _implementation = (JunkdealerCreatureImplementation*) _getImplementation();
	if (_implementation == NULL) {
		if (!deployed)
			throw ObjectNotDeployedException(this);

		DistributedMethod method(this, 6);

		method.executeWithVoidReturn();
	} else
		_implementation->activateRecovery();
}

void JunkdealerCreature::sendInitialMessage(PlayerCreature* player) {
	JunkdealerCreatureImplementation* _implementation = (JunkdealerCreatureImplementation*) _getImplementation();
	if (_implementation == NULL) {
		if (!deployed)
			throw ObjectNotDeployedException(this);

		DistributedMethod method(this, 7);
		method.addObjectParameter(player);

		method.executeWithVoidReturn();
	} else
		_implementation->sendInitialMessage(player);
}

void JunkdealerCreature::sendInitialChoices(PlayerCreature* player) {
	JunkdealerCreatureImplementation* _implementation = (JunkdealerCreatureImplementation*) _getImplementation();
	if (_implementation == NULL) {
		if (!deployed)
			throw ObjectNotDeployedException(this);

		DistributedMethod method(this, 8);
		method.addObjectParameter(player);

		method.executeWithVoidReturn();
	} else
		_implementation->sendInitialChoices(player);
}

void JunkdealerCreature::sendConversationStartTo(SceneObject* obj) {
	JunkdealerCreatureImplementation* _implementation = (JunkdealerCreatureImplementation*) _getImplementation();
	if (_implementation == NULL) {
		if (!deployed)
			throw ObjectNotDeployedException(this);

		DistributedMethod method(this, 9);
		method.addObjectParameter(obj);

		method.executeWithVoidReturn();
	} else
		_implementation->sendConversationStartTo(obj);
}

void JunkdealerCreature::selectConversationOption(int option, SceneObject* obj) {
	JunkdealerCreatureImplementation* _implementation = (JunkdealerCreatureImplementation*) _getImplementation();
	if (_implementation == NULL) {
		if (!deployed)
			throw ObjectNotDeployedException(this);

		DistributedMethod method(this, 10);
		method.addSignedIntParameter(option);
		method.addObjectParameter(obj);

		method.executeWithVoidReturn();
	} else
		_implementation->selectConversationOption(option, obj);
}

String JunkdealerCreature::getLocation() {
	JunkdealerCreatureImplementation* _implementation = (JunkdealerCreatureImplementation*) _getImplementation();
	if (_implementation == NULL) {
		if (!deployed)
			throw ObjectNotDeployedException(this);

		DistributedMethod method(this, 11);

		method.executeWithAsciiReturn(_return_getLocation);
		return _return_getLocation;
	} else
		return _implementation->getLocation();
}

void JunkdealerCreature::setLocation(const String& loc) {
	JunkdealerCreatureImplementation* _implementation = (JunkdealerCreatureImplementation*) _getImplementation();
	if (_implementation == NULL) {
		if (!deployed)
			throw ObjectNotDeployedException(this);

		DistributedMethod method(this, 12);
		method.addAsciiParameter(loc);

		method.executeWithVoidReturn();
	} else
		_implementation->setLocation(loc);
}

bool JunkdealerCreature::isAttackableBy(CreatureObject* object) {
	JunkdealerCreatureImplementation* _implementation = (JunkdealerCreatureImplementation*) _getImplementation();
	if (_implementation == NULL) {
		if (!deployed)
			throw ObjectNotDeployedException(this);

		DistributedMethod method(this, 13);
		method.addObjectParameter(object);

		return method.executeWithBooleanReturn();
	} else
		return _implementation->isAttackableBy(object);
}

void JunkdealerCreature::createSellJunkLootSelection(PlayerCreature* player) {
	JunkdealerCreatureImplementation* _implementation = (JunkdealerCreatureImplementation*) _getImplementation();
	if (_implementation == NULL) {
		if (!deployed)
			throw ObjectNotDeployedException(this);

		DistributedMethod method(this, 14);
		method.addObjectParameter(player);

		method.executeWithVoidReturn();
	} else
		_implementation->createSellJunkLootSelection(player);
}

DistributedObjectServant* JunkdealerCreature::_getImplementation() {

	_updated = true;
	return _impl;
}

void JunkdealerCreature::_setImplementation(DistributedObjectServant* servant) {
	_impl = servant;}

/*
 *	JunkdealerCreatureImplementation
 */

JunkdealerCreatureImplementation::JunkdealerCreatureImplementation(DummyConstructorParameter* param) : CreatureObjectImplementation(param) {
	_initializeImplementation();
}


JunkdealerCreatureImplementation::~JunkdealerCreatureImplementation() {
}


void JunkdealerCreatureImplementation::finalize() {
}

void JunkdealerCreatureImplementation::_initializeImplementation() {
	_setClassHelper(JunkdealerCreatureHelper::instance());

	_serializationHelperMethod();
	_serializationHelperMethod();
}

void JunkdealerCreatureImplementation::_setStub(DistributedObjectStub* stub) {
	_this = (JunkdealerCreature*) stub;
	CreatureObjectImplementation::_setStub(stub);
}

DistributedObjectStub* JunkdealerCreatureImplementation::_getStub() {
	return _this;
}

JunkdealerCreatureImplementation::operator const JunkdealerCreature*() {
	return _this;
}

void JunkdealerCreatureImplementation::lock(bool doLock) {
	_this->lock(doLock);
}

void JunkdealerCreatureImplementation::lock(ManagedObject* obj) {
	_this->lock(obj);
}

void JunkdealerCreatureImplementation::rlock(bool doLock) {
	_this->rlock(doLock);
}

void JunkdealerCreatureImplementation::wlock(bool doLock) {
	_this->wlock(doLock);
}

void JunkdealerCreatureImplementation::wlock(ManagedObject* obj) {
	_this->wlock(obj);
}

void JunkdealerCreatureImplementation::unlock(bool doLock) {
	_this->unlock(doLock);
}

void JunkdealerCreatureImplementation::runlock(bool doLock) {
	_this->runlock(doLock);
}

void JunkdealerCreatureImplementation::_serializationHelperMethod() {
	CreatureObjectImplementation::_serializationHelperMethod();

	_setClassName("JunkdealerCreature");

}

void JunkdealerCreatureImplementation::readObject(ObjectInputStream* stream) {
	uint16 _varCount = stream->readShort();
	for (int i = 0; i < _varCount; ++i) {
		String _name;
		_name.parseFromBinaryStream(stream);

		uint16 _varSize = stream->readShort();

		int _currentOffset = stream->getOffset();

		if(JunkdealerCreatureImplementation::readObjectMember(stream, _name)) {
		}

		stream->setOffset(_currentOffset + _varSize);
	}

	initializeTransientMembers();
}

bool JunkdealerCreatureImplementation::readObjectMember(ObjectInputStream* stream, const String& _name) {
	if (CreatureObjectImplementation::readObjectMember(stream, _name))
		return true;

	if (_name == "location") {
		TypeInfo<String >::parseFromBinaryStream(&location, stream);
		return true;
	}


	return false;
}

void JunkdealerCreatureImplementation::writeObject(ObjectOutputStream* stream) {
	int _currentOffset = stream->getOffset();
	stream->writeShort(0);
	int _varCount = JunkdealerCreatureImplementation::writeObjectMembers(stream);
	stream->writeShort(_currentOffset, _varCount);
}

int JunkdealerCreatureImplementation::writeObjectMembers(ObjectOutputStream* stream) {
	String _name;
	int _offset;
	uint16 _totalSize;
	_name = "location";
	_name.toBinaryStream(stream);
	_offset = stream->getOffset();
	stream->writeShort(0);
	TypeInfo<String >::toBinaryStream(&location, stream);
	_totalSize = (uint16) (stream->getOffset() - (_offset + 2));
	stream->writeShort(_offset, _totalSize);


	return 1 + CreatureObjectImplementation::writeObjectMembers(stream);
}

JunkdealerCreatureImplementation::JunkdealerCreatureImplementation() {
	_initializeImplementation();
	// server/zone/objects/creature/junkdealer/JunkdealerCreature.idl(71):  		Logger.setLoggingName("JunkdealerCreature");
	Logger::setLoggingName("JunkdealerCreature");
}

void JunkdealerCreatureImplementation::loadTemplateData(SharedObjectTemplate* templateData) {
	// server/zone/objects/creature/junkdealer/JunkdealerCreature.idl(77):  		super.loadTemplateData(templateData);
	CreatureObjectImplementation::loadTemplateData(templateData);
	// server/zone/objects/creature/junkdealer/JunkdealerCreature.idl(79):  		super.optionsBitmask = 0x108;
	CreatureObjectImplementation::optionsBitmask = 0x108;
	// server/zone/objects/creature/junkdealer/JunkdealerCreature.idl(80):  		super.pvpStatusBitmask = 0;
	CreatureObjectImplementation::pvpStatusBitmask = 0;
}

void JunkdealerCreatureImplementation::activateRecovery() {
}

String JunkdealerCreatureImplementation::getLocation() {
	// server/zone/objects/creature/junkdealer/JunkdealerCreature.idl(116):  		return location;
	return location;
}

void JunkdealerCreatureImplementation::setLocation(const String& loc) {
	// server/zone/objects/creature/junkdealer/JunkdealerCreature.idl(120):  		location = loc;
	location = loc;
}

bool JunkdealerCreatureImplementation::isAttackableBy(CreatureObject* object) {
	// server/zone/objects/creature/junkdealer/JunkdealerCreature.idl(124):  		return false;
	return false;
}

/*
 *	JunkdealerCreatureAdapter
 */

JunkdealerCreatureAdapter::JunkdealerCreatureAdapter(JunkdealerCreatureImplementation* obj) : CreatureObjectAdapter(obj) {
}

Packet* JunkdealerCreatureAdapter::invokeMethod(uint32 methid, DistributedMethod* inv) {
	Packet* resp = new MethodReturnMessage(0);

	switch (methid) {
	case 6:
		activateRecovery();
		break;
	case 7:
		sendInitialMessage((PlayerCreature*) inv->getObjectParameter());
		break;
	case 8:
		sendInitialChoices((PlayerCreature*) inv->getObjectParameter());
		break;
	case 9:
		sendConversationStartTo((SceneObject*) inv->getObjectParameter());
		break;
	case 10:
		selectConversationOption(inv->getSignedIntParameter(), (SceneObject*) inv->getObjectParameter());
		break;
	case 11:
		resp->insertAscii(getLocation());
		break;
	case 12:
		setLocation(inv->getAsciiParameter(_param0_setLocation__String_));
		break;
	case 13:
		resp->insertBoolean(isAttackableBy((CreatureObject*) inv->getObjectParameter()));
		break;
	case 14:
		createSellJunkLootSelection((PlayerCreature*) inv->getObjectParameter());
		break;
	default:
		return NULL;
	}

	return resp;
}

void JunkdealerCreatureAdapter::activateRecovery() {
	((JunkdealerCreatureImplementation*) impl)->activateRecovery();
}

void JunkdealerCreatureAdapter::sendInitialMessage(PlayerCreature* player) {
	((JunkdealerCreatureImplementation*) impl)->sendInitialMessage(player);
}

void JunkdealerCreatureAdapter::sendInitialChoices(PlayerCreature* player) {
	((JunkdealerCreatureImplementation*) impl)->sendInitialChoices(player);
}

void JunkdealerCreatureAdapter::sendConversationStartTo(SceneObject* obj) {
	((JunkdealerCreatureImplementation*) impl)->sendConversationStartTo(obj);
}

void JunkdealerCreatureAdapter::selectConversationOption(int option, SceneObject* obj) {
	((JunkdealerCreatureImplementation*) impl)->selectConversationOption(option, obj);
}

String JunkdealerCreatureAdapter::getLocation() {
	return ((JunkdealerCreatureImplementation*) impl)->getLocation();
}

void JunkdealerCreatureAdapter::setLocation(const String& loc) {
	((JunkdealerCreatureImplementation*) impl)->setLocation(loc);
}

bool JunkdealerCreatureAdapter::isAttackableBy(CreatureObject* object) {
	return ((JunkdealerCreatureImplementation*) impl)->isAttackableBy(object);
}

void JunkdealerCreatureAdapter::createSellJunkLootSelection(PlayerCreature* player) {
	((JunkdealerCreatureImplementation*) impl)->createSellJunkLootSelection(player);
}

/*
 *	JunkdealerCreatureHelper
 */

JunkdealerCreatureHelper* JunkdealerCreatureHelper::staticInitializer = JunkdealerCreatureHelper::instance();

JunkdealerCreatureHelper::JunkdealerCreatureHelper() {
	className = "JunkdealerCreature";

	DistributedObjectBroker::instance()->registerClass(className, this);
}

void JunkdealerCreatureHelper::finalizeHelper() {
	JunkdealerCreatureHelper::finalize();
}

DistributedObject* JunkdealerCreatureHelper::instantiateObject() {
	return new JunkdealerCreature(DummyConstructorParameter::instance());
}

DistributedObjectServant* JunkdealerCreatureHelper::instantiateServant() {
	return new JunkdealerCreatureImplementation(DummyConstructorParameter::instance());
}

DistributedObjectAdapter* JunkdealerCreatureHelper::createAdapter(DistributedObjectStub* obj) {
	DistributedObjectAdapter* adapter = new JunkdealerCreatureAdapter((JunkdealerCreatureImplementation*) obj->_getImplementation());

	obj->_setClassName(className);
	obj->_setClassHelper(this);

	adapter->setStub(obj);

	return adapter;
}

