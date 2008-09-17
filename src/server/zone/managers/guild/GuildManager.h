/*
 *	server/zone/managers/guild/GuildManager.h generated by engine3 IDL compiler 0.55
 */

#ifndef GUILDMANAGER_H_
#define GUILDMANAGER_H_

#include "engine/orb/DistributedObjectBroker.h"

class Guild;

class Player;

class ZoneServer;

class GuildMap;

class GuildManager : public DistributedObjectStub {
public:
	GuildManager(ZoneServer* server);

	void lock(bool doLock = true);

	void unlock(bool doLock = true);

	void load();

	bool createGuild(Player* player, string& tag, string& name);

	Guild* getGuild(string& tag, bool doLock = true);

	Guild* getGuild(unsigned int gid, bool doLock = true);

	bool removeGuild(int gid);

	void removePlayersFromGuild(int gid);

	GuildMap* getGuildMap();

	void handleGuildTag(unsigned int boxID, Player* player, unsigned int cancel, string& returnString);

	void handleGuildName(unsigned int boxID, Player* player, unsigned int cancel, string& returnString);

	void handleGuildCreationName(Player* player);

	void handleGuildSponsor(unsigned int boxID, Player* player, unsigned int cancel, string& returnString);

	void handleVerifyBoxSponsorTargetforGuildMembership(unsigned int boxID, Player* otherPlayer, unsigned int cancel);

	void VerifyBoxSponsorTargetforGuildMembership(Player* otherPlayer, string& inviter, string& guildname);

	void handleSponsoredGuildMembersBox(unsigned int boxID, Player* player, unsigned int cancel, int index);

	void handleSponsoredGuildMembersAcceptBox(unsigned int boxID, Player* player, unsigned int cancel, int index);

	void removeFromGuild(Player* player, Player* removePlayer);

	void execRemoveFromGuild(unsigned int boxID, Player* player, unsigned int cancel);

	void handleGuildDisbandBox(unsigned int boxID, Player* player, unsigned int cancel, string& returnString);

	void handleGuildNameChange(unsigned int boxID, Player* player, unsigned int cancel, string& returnString);

	void handleGuildNameChangeName(unsigned int boxID, Player* player, unsigned int cancel, string& returnString);

	void handleGuildInformationMembers(Player* player);

	void handleGuildInformationMembersBox(unsigned int boxID, Player* player, unsigned int cancel, int index);

	void handleGuildMemberOptions(unsigned int boxID, Player* player, unsigned int cancel, int index);

	void handleRemoveFromGuild(Player* player, string& kickee);

	void callGuildPermissions(Player* player, string& proband);

	void handleGuildPermissionSelection(unsigned int boxID, Player* player, unsigned int cancel, int index);

	void declineSponsoring(string& declinee, unsigned int inviteGuild, Player* inviter);

	void handleGuildInfo(Player* player);

	void handleGuildSponsoring(Player* player);

	void handleGuildRenaming(Player* player);

	void handleGuildTransferLeader(Player* player);

	void handleGuildTransferLeaderBox(unsigned int boxID, Player* player, unsigned int cancel, string& returnString);

	void handleGuildTransferLeaderVerifyBox(unsigned int boxID, Player* player, unsigned int cancel);

protected:
	GuildManager(DummyConstructorParameter* param);

	virtual ~GuildManager();

	friend class GuildManagerHelper;
};

class GuildManagerImplementation;

class GuildManagerAdapter : public DistributedObjectAdapter {
public:
	GuildManagerAdapter(GuildManagerImplementation* impl);

	Packet* invokeMethod(sys::uint32 methid, DistributedMethod* method);

	void lock(bool doLock);

	void unlock(bool doLock);

	void load();

	bool createGuild(Player* player, string& tag, string& name);

	Guild* getGuild(string& tag, bool doLock);

	Guild* getGuild(unsigned int gid, bool doLock);

	bool removeGuild(int gid);

	void removePlayersFromGuild(int gid);

	GuildMap* getGuildMap();

	void handleGuildTag(unsigned int boxID, Player* player, unsigned int cancel, string& returnString);

	void handleGuildName(unsigned int boxID, Player* player, unsigned int cancel, string& returnString);

	void handleGuildCreationName(Player* player);

	void handleGuildSponsor(unsigned int boxID, Player* player, unsigned int cancel, string& returnString);

	void handleVerifyBoxSponsorTargetforGuildMembership(unsigned int boxID, Player* otherPlayer, unsigned int cancel);

	void VerifyBoxSponsorTargetforGuildMembership(Player* otherPlayer, string& inviter, string& guildname);

	void handleSponsoredGuildMembersBox(unsigned int boxID, Player* player, unsigned int cancel, int index);

	void handleSponsoredGuildMembersAcceptBox(unsigned int boxID, Player* player, unsigned int cancel, int index);

	void removeFromGuild(Player* player, Player* removePlayer);

	void execRemoveFromGuild(unsigned int boxID, Player* player, unsigned int cancel);

	void handleGuildDisbandBox(unsigned int boxID, Player* player, unsigned int cancel, string& returnString);

	void handleGuildNameChange(unsigned int boxID, Player* player, unsigned int cancel, string& returnString);

	void handleGuildNameChangeName(unsigned int boxID, Player* player, unsigned int cancel, string& returnString);

	void handleGuildInformationMembers(Player* player);

	void handleGuildInformationMembersBox(unsigned int boxID, Player* player, unsigned int cancel, int index);

	void handleGuildMemberOptions(unsigned int boxID, Player* player, unsigned int cancel, int index);

	void handleRemoveFromGuild(Player* player, string& kickee);

	void callGuildPermissions(Player* player, string& proband);

	void handleGuildPermissionSelection(unsigned int boxID, Player* player, unsigned int cancel, int index);

	void declineSponsoring(string& declinee, unsigned int inviteGuild, Player* inviter);

	void handleGuildInfo(Player* player);

	void handleGuildSponsoring(Player* player);

	void handleGuildRenaming(Player* player);

	void handleGuildTransferLeader(Player* player);

	void handleGuildTransferLeaderBox(unsigned int boxID, Player* player, unsigned int cancel, string& returnString);

	void handleGuildTransferLeaderVerifyBox(unsigned int boxID, Player* player, unsigned int cancel);

protected:
	string _param1_createGuild__Player_string_string_;
	string _param2_createGuild__Player_string_string_;
	string _param0_getGuild__string_bool_;
	string _param3_handleGuildTag__int_Player_int_string_;
	string _param3_handleGuildName__int_Player_int_string_;
	string _param3_handleGuildSponsor__int_Player_int_string_;
	string _param1_VerifyBoxSponsorTargetforGuildMembership__Player_string_string_;
	string _param2_VerifyBoxSponsorTargetforGuildMembership__Player_string_string_;
	string _param3_handleGuildDisbandBox__int_Player_int_string_;
	string _param3_handleGuildNameChange__int_Player_int_string_;
	string _param3_handleGuildNameChangeName__int_Player_int_string_;
	string _param1_handleRemoveFromGuild__Player_string_;
	string _param1_callGuildPermissions__Player_string_;
	string _param0_declineSponsoring__string_int_Player_;
	string _param3_handleGuildTransferLeaderBox__int_Player_int_string_;
};

class GuildManagerHelper : public DistributedObjectClassHelper, public Singleton<GuildManagerHelper> {
	static GuildManagerHelper* staticInitializer;

public:
	GuildManagerHelper();

	void finalizeHelper();

	DistributedObject* instantiateObject();

	DistributedObjectAdapter* createAdapter(DistributedObjectStub* obj);

	friend class SingletonWrapper<GuildManagerHelper>;
};

class GuildManagerServant : public DistributedObjectServant {
public:
	GuildManager* _this;

public:
	GuildManagerServant();
	virtual ~GuildManagerServant();

	void _setStub(DistributedObjectStub* stub);
	DistributedObjectStub* _getStub();

};

#endif /*GUILDMANAGER_H_*/
