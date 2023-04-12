#ifndef STORY_CHUNK_H
#define STORY_CHUNK_H

#include <map>
#include <iostream>

#include "chrono_common/chronolog_types.h"

namespace chronolog
{


typedef uint64_t chrono_time;
typedef uint32_t chrono_index;

// StoryChunk contains all the events for the single story
// for the duration [startTime, endTime[
// startTime included, endTime excluded
// startTime/endTime are invariant

typedef std::tuple< chrono_time, chrono_index> ArrivalSequence;

typedef std::tuple< chrono_time, ClientId, chrono_index> EventSequence;


class StoryChunk
{
public:
   
   StoryChunk( StoryId const & story_id = 0, uint64_t start_time = 0, uint64_t end_time = 0)
	   : storyId(story_id)
	   , startTime(start_time)
	   , endTime(end_time)
	   , revisionTime(start_time)
	{ }

   ~StoryChunk() = default;

   uint64_t getStartTime() const
   { return startTime; }
   uint64_t getEndTime() const
   { return endTime; }

   bool empty() const
   {  return ( logEvents.empty() ? true : false ); }

   std::map<EventSequence,LogEvent>::const_iterator begin() const
   {   return logEvents.begin(); }

   std::map<EventSequence,LogEvent>::const_iterator end() const
   {   return logEvents.end(); }

   std::map<EventSequence,LogEvent>::const_iterator lower_bound( uint64_t chrono_time) const
   {   return logEvents.lower_bound( EventSequence{chrono_time,0,0});}

   int insertEvent(LogEvent const& event)
   {
	 std::cout<<"StoryChunk:{"<<storyId<<":"<<startTime<<":"<<endTime<<"} got event {"
		 <<event.storyId<<":"<<event.time()<<":"<<event.clientId<<":"<<event.index()<<"}"<<std::endl;
      if((event.time() >= startTime)
	      && (event.time() < endTime))
      { 
         logEvents.insert( std::pair<EventSequence,LogEvent>({event.time(),event.clientId, event.index()}, event));
	  return 1;
      }
      else
      {   return 0;   }
   }


   uint32_t mergeEvents( std::map<EventSequence,LogEvent> & events
               , std::map<EventSequence,LogEvent>::iterator &  merge_start )

   {
       uint32_t merged_event_count = 0;
       std::map<EventSequence,LogEvent>::iterator first_merged, last_merged;
     
       if ( (*merge_start).second.time() < startTime)
       {   merge_start = events.lower_bound(EventSequence{startTime,0,0}); }

       for( auto iter = merge_start; iter != events.end(); ++iter)
	{
	    if( insertEvent( (*iter).second ) > 0 )
	    { 
		if( merged_event_count ==0)
		{ first_merged = iter; }
		last_merged = iter;
		merged_event_count++;
            }	
	    else
	    {    break; }  //stop at the first record that can't be merged
	}

        if ( merged_event_count >0)
	{
	    //remove the merged records from the original map
	    events.erase( first_merged, last_merged);
	}

    return merged_event_count;
   }

   uint32_t mergeEvents(StoryChunk & other_chunk)
   {  return 0; }

   uint32_t mergeEvents(StoryChunk & other_chunk
	          , uint64_t start_time, uint64_t end_time)
   {  return 0; }


   uint32_t extractEvents( std::map<EventSequence,LogEvent> & target_map 
       , std::map<EventSequence,LogEvent>::iterator first_pos
       , std::map<EventSequence,LogEvent>::iterator last_pos)
   { return 0;}
   uint32_t extractEvents( std::map<EventSequence,LogEvent>  & target_map
	          , uint64_t start_time, uint64_t end_time)
   { return 0;}
   uint32_t eraseEvents(uint64_t start_time, uint64_t end_time)
   { return 0;}

private:
StoryId storyId;          
uint64_t startTime;
uint64_t endTime;   
uint64_t revisionTime;

std::map<EventSequence, LogEvent> logEvents;

};

}
#endif