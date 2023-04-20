#ifndef STORYTELLER_CLIENT_H
#define STORYTELLER_CLIENT_H


#include <atomic>
#include <map>

#include <thallium.hpp>

#include "../chrono_common/KeeperIdCard.h"
#include "../chrono_common/chronolog_types.h"
#include "../chrono_common/client.h"

namespace chronolog
{

class ChronologTimer
{
  public:
       uint64_t getTimestamp();
};

class KeeperRecordingClient;

class RoundRobinKeeperChoice
{
public:
	KeeperRecordingClient* chooseKeeper( std::vector<KeeperRecordingClient*> const& vectorOfKeepers, uint64_t chrono_tick )
	{   
		return vectorOfKeepers[ chrono_tick % vectorOfKeepers.size()];
	}
};

// this class definition lives in the client lib
template< class KeeperChoicePolicy>
class StoryWritingHandle : public StoryHandle
{
 public: 	
     StoryWritingHandle(StorytellerClient & client, ChronicleName const& a_chronicle, StoryName const& a_story, StoryId const& story_id)
            : theClient(client)
	    , chronicle(a_chronicle)
            , story(a_story)
            , storyId(story_id)
            , keeperChoicePolicy( new KeeperChoicePolicy)	      
     {  }


     virtual ~StoryWritingHandle();
     virtual int log_event( std::string const&);
     virtual int log_event( size_t size, void* data);

     void addRecordingClient(KeeperRecordingClient *);
     void removeRecordingClient(KeeperIdCard const&);

private:

        StorytellerClient& theClient;
        ChronicleName  chronicle;
	StoryName      story;
	StoryId        storyId;
	KeeperChoicePolicy * keeperChoicePolicy;
	std::vector<KeeperRecordingClient*> storyKeepers;
};


class StorytellerClient
{
public:
    StorytellerClient( ChronologTimer & chronolog_timer, thallium::engine & client_tl_engine, ClientId client_id)
	    : theTimer(chronolog_timer)
	    , client_engine(client_tl_engine)  
	    , clientId(client_id)  
    {  }

    ~StorytellerClient();

    int addKeeperRecordingClient(KeeperIdCard const&);
    int removeKeeperRecordingClient(KeeperIdCard const&);

    std::pair<int,StoryHandle *> initializeStoryWritingHandle(ChronicleName const&, StoryName const&, StoryId const&, std::vector<KeeperIdCard> const&);
    void removeAcquiredStoryHandle(ChronicleName const&, StoryName const&);

    uint64_t getTimestamp()
    {   return theTimer.getTimestamp(); }

    ClientId const& getClientId() const
    {   return clientId;   }

    int get_event_index();


private:
    StorytellerClient(StorytellerClient const&) = delete;
    StorytellerClient & operator= (StorytellerClient const&) = delete;

    thallium::engine & client_engine;
    ChronologTimer & theTimer;
    ClientId	clientId;
    std::atomic<int>  atomic_index;

    std::mutex  recordingClientMapMutex;
    std::mutex  acquiredStoryMapMutex;

    std::map<std::pair<uint32_t,uint16_t>, KeeperRecordingClient*> recordingClientMap;
    std::map<std::pair<std::string,std::string>, StoryHandle*> acquiredStoryHandles;

};



}//namespace

#endif