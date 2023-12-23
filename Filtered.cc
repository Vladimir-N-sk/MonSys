
#include "common.h"
#include "globals.h"
#include "utils.h"

#include "Message.h"
#include "MTSync.h"
#include "Filtered.h"
#include "Parameter.h"
#include "ScopeGuard.h"

#include <functional>
#include <algorithm>


bool TAFilter::examine( Message* m) throw(runtime_error)
{
  MONSYS_DEBUG << "Filter start param: " << m->getParameterName() << endl;
    Parameter * const pname = m->getSource();

    alarm = m->getStatus();
    if (alarm > 0) return false;

    alarm = m->getAlarm();

    nwAlarm_lock.wrlock();
    ScopeGuard nwAlarm_lock_unlocker  = makeObjGuard( nwAlarm_lock, &RWLock::unlock);

    nwTime_lock.wrlock();
    ScopeGuard nwTime_lock_unlocker   = makeObjGuard( nwTime_lock, &RWLock::unlock);

    Journal::iterator ti = nwTime.find( pname);
    Jour::iterator al = nwAlarm.find( pname);

    if ( nwAlarm.end() == al) {
        nwAlarm_lock.unlock();
        nwAlarm_lock.wrlock();
        if ( nwAlarm.end() == al)  al = nwAlarm.insert( nwAlarm.begin(),  Jour::value_type( pname, alarm));

            if ( nwTime.end() == ti) {
                nwTime_lock.unlock();
                nwTime_lock.wrlock();
                if ( nwTime.end() == ti)
                ti = nwTime.insert( nwTime.begin(), Journal::value_type( pname, m->getTime()));
            }
MONSYS_DEBUG << "Filter first time param: " << m->getParameterName() << " alarm:" << alarm<< " time:"<< m->getTime()<<endl;
            if ( alarm == ALARM   ) {
                ti->second = m->getTime()+ writePeriod;
                return true;
            } else {
                ti->second = m->getTime()+ writePeriod;
                return false;
            }
    }


    if ( m->getTime() >= ti->second) {

        if ( m->getTime() >= ti->second + writePeriod){
            ti->second = m->getTime();
        }
        ti->second += writePeriod;

        if ( alarm == al->second) {
MONSYS_DEBUG << "============Filter  alarm==al->second  param: " << m->getParameterName() <<" alarm:"<< alarm<< " time:" << m->getTime()<<endl;
            return false;
        }

        if ( (alarm > al->second && al->second == ALARM) ||  alarm == ALARM   ) {
MONSYS_DEBUG << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Filter  NEW alarm  param: " << m->getParameterName() <<" alarm:"<< alarm<< " al->second:" << al->second<<endl;
            al->second = alarm;
            return true;
        } else {
MONSYS_DEBUG << "#####################Filter  alarm 2->1 param: " << m->getParameterName() <<" alarm:"<< alarm<< " al->second:" << al->second<<endl;
            al->second = alarm;
            return false;
        }
    }
MONSYS_DEBUG << "Filter NO time param: " << m->getParameterName() << " time second:" << ti->second<< " time message:"<< m->getTime()<<endl;
    return false;

//    if ( alarm == ALARM   ||  alarm > SUBALARM   ) {
}

void TAFilter::init()
{
    MONSYS_DEBUG << "Filter start " <<endl;
    nwTime_lock.wrlock();
    ScopeGuard nwTime_lock_unlocker  = makeObjGuard( nwTime_lock, &RWLock::unlock);
    nwTime.clear();

    nwAlarm_lock.wrlock();
    ScopeGuard nwAlarm_lock_unlocker  = makeObjGuard( nwAlarm_lock, &RWLock::unlock);
    nwAlarm.clear();
}

TAFilter::TAFilter( const timespec& wp)
 : writePeriod(wp)
{
}


/***********************************************************************
 *
 ***********************************************************************/
bool TimeThresholdFilter::examine( Message* m) throw(runtime_error)
{
    Parameter * const pname = m->getSource();

    mpool_lock.rdlock();
    ScopeGuard mpool_lock_unlocker
                 = makeObjGuard( mpool_lock, &RWLock::unlock);

    Journal::iterator ti = mpool.find( pname);

//MONSYS_DEBUG << " filter param: " << m->getParameterName() << " start"<<endl;

    if ( mpool.end() == ti) {

        mpool_lock.unlock();
        mpool_lock.wrlock();

        ti = mpool.find( pname);

        if ( mpool.end() == ti) {
//MONSYS_DEBUG << " filter param: " << m->getParameterName() << " if first time insrt to pool and return TRUE "<<endl;
            m->incUses();
            mpool.insert( Journal::value_type( pname, PoolNode(m,writePeriod)));
            return true;
        }
    }

    //TODO late messages statistic gathering need
    if ( ti->second.base_msg->getTime() > m->getTime()) return true;

    if ( *ti->second.base_msg == *m) {

        ti->second.last_msg_time =  max( ti->second.last_msg_time, m->getTime());

    if ( m->getTime() >= ti->second.next_msg_time) {

        if ( m->getTime() >= ti->second.next_msg_time + writePeriod){

            ti->second.next_msg_time = m->getTime();

        }
        ti->second.next_msg_time = ti->second.next_msg_time + writePeriod;

    ti->second.base_msg->decUses();
    m->incUses();
    ti->second = PoolNode(m,writePeriod);

        return true;
    }
        return false;
    }

        ti->second.next_msg_time = m->getTime() + writePeriod ;

    //TODO late messages statistic gathering need
    //XXX this case violates db structure
    if ( ti->second.last_msg_time > m->getTime()) return true;

    ti->second.base_msg->decUses();
    m->incUses();
    ti->second = PoolNode(m,writePeriod);
    return true;
}

void TimeThresholdFilter::init()
{
    mpool_lock.wrlock();
    ScopeGuard mpool_lock_unlocker
                 = makeObjGuard( mpool_lock, &RWLock::unlock);

    mpool.clear();
}

TimeThresholdFilter::TimeThresholdFilter( ProtoFiltered* b, const timespec& wp)
 : backend( b),
  writePeriod(wp)
{
}

TimeThresholdFilter::PoolNode::PoolNode( Message* m, const timespec& np)
 : base_msg(m),
   next_msg_time(m->getTime() + np),
   last_msg_time(m->getTime() )
{
}


/*******************************************************************************************************************************
 *
 * *********************************************************************************************************************************/
void ProtoFiltered::addFilter( Filter* const f)
{
    filters_lock.wrlock();
    ScopeGuard filters_lock_unlocker
                 = makeObjGuard( filters_lock, &RWLock::unlock);

    filters.push_back( f);
}

void ProtoFiltered::initFilters()
{
    for_each( filters.begin(),  filters.end(), mem_fun( &Filter::init));
}

void ProtoFiltered::filtered_putMessage( Message* m) throw(runtime_error)
{
    filters_lock.rdlock();
    ScopeGuard filters_lock_unlocker
                 = makeObjGuard( filters_lock, &RWLock::unlock);

    if ( filters.end() != find_if( filters.begin(), filters.end(), bind2nd( mem_fun( &Filter::examine), m)))
        transparent_putMessage( m);

}

/***********************************************************************
 *
 ***********************************************************************/
bool ThresholdFilter::examine( Message* m) throw(runtime_error)
{
    Parameter * const pname = m->getSource();

    mpool_lock.rdlock();
    ScopeGuard mpool_lock_unlocker
                 = makeObjGuard( mpool_lock, &RWLock::unlock);

    Journal::iterator ti = mpool.find( pname);

//MONSYS_DEBUG << " filter param: " << m->getParameterName() << " start"<<endl;

    if ( mpool.end() == ti) {
// ��� ������ ���������
//MONSYS_DEBUG << " filter param: " << m->getParameterName() << " this first message "<<endl;
        mpool_lock.unlock();
        mpool_lock.wrlock();

        ti = mpool.find( pname);

//MONSYS_DEBUG << " filter param: " << m->getParameterName() << " again set  ti"<<endl;

        if ( mpool.end() == ti) {
//MONSYS_DEBUG << " filter param: " << m->getParameterName() << " if first time insrt to pool and return TRUE "<<endl;
            m->incUses();
            mpool.insert( Journal::value_type( pname, PoolNode(m)));
            return true;
        }
    }



//MONSYS_DEBUG << "1 filter param: " << m->getParameterName() << " ti->second.base_msg->getTime():"<< ti->second.base_msg->getTime()<< " m->getTime():"<< m->getTime()<<endl;
    //TODO late messages statistic gathering need
    if ( ti->second.base_msg->getTime() > m->getTime()) return true;
//MONSYS_DEBUG << "1 filter param: " << m->getParameterName() << " ti->second.base_msg->getTime():"<< ti->second.base_msg->getTime()<< " m->getTime():"<< m->getTime()<<endl;


    if ( *ti->second.base_msg == *m) {
//MONSYS_DEBUG << "2 filter param: " << m->getParameterName() << " ti->second.last_msg_time:"<< ti->second.last_msg_time<< " m->getTime():"<< m->getTime() <<endl;
        ti->second.last_msg_time =  max( ti->second.last_msg_time, m->getTime());
//MONSYS_DEBUG << "3 filter param: " << m->getParameterName() << " ti->second.last_msg_time:"<< ti->second.last_msg_time<< " m->getTime():"<< m->getTime() << " return FALSE"<<endl;
//MONSYS_DEBUG << "6 filter param: " << m->getParameterName() << " return false" <<endl;

        return false;
    }

//MONSYS_DEBUG << "4 filter param: " << m->getParameterName() << " ti->second.last_msg_time:"<< ti->second.last_msg_time<< " m->getTime():"<< m->getTime() <<endl;
    //TODO late messages statistic gathering need
    //XXX this case violates db structure
    if ( ti->second.last_msg_time > m->getTime()) return true;
//MONSYS_DEBUG << "5 filter param: " << m->getParameterName() << " ti->second.last_msg_time:"<< ti->second.last_msg_time<< " m->getTime():"<< m->getTime() <<endl;



/* AVN 02/09/2017 ���� ��������� ������ ����� � ��
    if ( ti->second.base_msg->getTime() != ti->second.last_msg_time
              && !(ti->second.base_msg->getStatus() & WEDGED)
              && !(m->getStatus() & WEDGED)) {
        // repeat base message with last message time for adeqate graphs
MONSYS_DEBUG << " filter param: " << m->getParameterName() << " 8"<<endl;
        Message * const fm =
           new Message( ti->second.base_msg, ti->second.last_msg_time);

        backend->transparent_putMessage( fm);
        fm->decUses();
    }
*/


    ti->second.base_msg->decUses();
    m->incUses();
    ti->second = PoolNode(m);
//MONSYS_DEBUG << "6 filter param: " << m->getParameterName() << " write msg to pool and return TRUE" <<endl;
    return true;
}

void ThresholdFilter::init()
{
    mpool_lock.wrlock();
    ScopeGuard mpool_lock_unlocker
                 = makeObjGuard( mpool_lock, &RWLock::unlock);

    mpool.clear();
}

ThresholdFilter::ThresholdFilter( ProtoFiltered* b)
 : backend( b)
{
}

ThresholdFilter::PoolNode::PoolNode( Message* m)
 : base_msg(m),
   last_msg_time(m->getTime())
{
}

/***********************************************************************
 *
 ***********************************************************************/
bool TimeFilter::examine( Message* m) throw(runtime_error)
{
    Parameter * const pname = m->getSource();

   if  ( pname->getProperties()->type == 1 )  return false;

    nwTime_lock.wrlock();
    ScopeGuard nwTime_lock_unlocker
                 = makeObjGuard( nwTime_lock, &RWLock::unlock);

    Journal::iterator ti = nwTime.find( pname);

    if ( nwTime.end() == ti) {
        nwTime_lock.unlock();
        nwTime_lock.wrlock();
        if ( nwTime.end() == ti)
             ti = nwTime.insert( nwTime.begin(),
                            Journal::value_type( pname, m->getTime()));
    }

    if ( m->getTime() >= ti->second) {

        if ( m->getTime() >= ti->second + writePeriod){

            ti->second = m->getTime();

        }
        ti->second += writePeriod;

        return true;
    }
    return false;
}

void TimeFilter::init()
{
    nwTime_lock.wrlock();
    ScopeGuard nwTime_lock_unlocker
                 = makeObjGuard( nwTime_lock, &RWLock::unlock);
    nwTime.clear();
}

/*****************************************************************************************************************
 *
 * ****************************************************************************************************************/
bool AlarmFilter::examine( Message* m) throw(runtime_error)
{
    bool ret = false;

    try {
        if ( alarmer == m->getSource() && alarm != m->getAlarm()) {
            ret = true;
            alarm = m->getAlarm();
        } else if ( ALARM == alarm && SUBALARM == alarm ) ret = true;
    }
    catch( Message::empty& e) {
    }

    return ret;
}

void AlarmFilter::init()
{
    alarm = NOALARM;
}

AlarmFilter::AlarmFilter( const Parameter* al)
 : alarmer(al),
   alarm(NOALARM)
{
}

TimeFilter::TimeFilter( const timespec& wp)
 : writePeriod(wp)
{
}

bool TransparentFilter::examine( Message*) throw()
{
    return true;
}
