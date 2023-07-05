/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
// pengfeng: GE channel tool

#ifndef LOSS_GE_QUEUE_HH
#define LOSS_GE_QUEUE_HH

#include <queue>
#include <cstdint>
#include <string>
#include <random>
#include <iostream>

#include "file_descriptor.hh"

class LossQueue
{
private:
    std::queue<std::string> packet_queue_ {};

    virtual bool drop_packet( const std::string & packet ) = 0;

protected:
    std::default_random_engine g2b_prng_;
    std::default_random_engine b2g_prng_;
    std::default_random_engine good_state_prng_;
    std::default_random_engine bad_state_prng_;

    bool is_good_state;

public:
    LossQueue();
    virtual ~LossQueue() {}

    void read_packet( const std::string & contents );

    void write_packets( FileDescriptor & fd );

    unsigned int wait_time( void );

    bool pending_output( void ) const { return not packet_queue_.empty(); }

    static bool finished( void ) { return false; }
};

class IIDLoss : public LossQueue
{
private:
    std::bernoulli_distribution g2b_dist_;
    std::bernoulli_distribution b2g_dist_;
    std::bernoulli_distribution good_state_drop_dist_;
    std::bernoulli_distribution bad_state_drop_dist_;

    bool drop_packet( const std::string & packet ) override;

public:
    IIDLoss(double g2b_rate): 
    g2b_dist_( g2b_rate ),
    b2g_dist_( 0.1 ),
    good_state_drop_dist_( 0.2 ),
    bad_state_drop_dist_( 0.3 )
    {
        std::cerr << "not what I want! " << std::endl;
    }

    IIDLoss(double g2b_rate, double b2g_rate, double good_state_loss_rate, double bad_state_loss_rate ) 
    : g2b_dist_( g2b_rate ),
    b2g_dist_( b2g_rate ),
    good_state_drop_dist_(good_state_loss_rate),
    bad_state_drop_dist_(bad_state_loss_rate)
    {
        std::cerr << "the g2b_rate= " << g2b_rate << "   b2g_rate= "<<  b2g_rate << "   good_state_loss_rate= " << good_state_loss_rate << "   bad_state_loss_rate= " << bad_state_loss_rate << std::endl; 
    }
};

class StochasticSwitchingLink : public LossQueue
{
private:
    bool link_is_on_;
    std::exponential_distribution<> on_process_;
    std::exponential_distribution<> off_process_;

    uint64_t next_switch_time_;

    bool drop_packet( const std::string & packet ) override;

public:
    StochasticSwitchingLink( const double mean_on_time_, const double mean_off_time );

    unsigned int wait_time( void );
};

class PeriodicSwitchingLink : public LossQueue
{
private:
    bool link_is_on_;
    uint64_t on_time_, off_time_, next_switch_time_;

    bool drop_packet( const std::string & packet ) override;

public:
    PeriodicSwitchingLink( const double on_time, const double off_time );

    unsigned int wait_time( void );
};

#endif /* LOSS_QUEUE_HH */
