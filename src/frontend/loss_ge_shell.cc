/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include <getopt.h>

#include "loss_ge_queue.hh"
#include "util.hh"
#include "ezio.hh"
#include "packetshell.cc"

using namespace std;

void usage( const string & program_name )
{
    throw runtime_error( "Usage: " + program_name + " g2b_rate,b2g_rate,lr_g,lr_b [COMMAND...]" );
}

int main( int argc, char *argv[] )
{
    try {
        const bool passthrough_until_signal = getenv( "MAHIMAHI_PASSTHROUGH_UNTIL_SIGNAL" );

        /* clear environment while running as root */
        char ** const user_environment = environ;
        environ = nullptr;

        check_requirements( argc, argv );

        if ( argc < 2 ) {
            usage( argv[ 0 ] );
        }

        stringstream argss( argv[ 2 ] );
        string token;
        // const double g2b_r, b2g_r, lr_g, lr_b;
        double rate_log[4];
        memset( rate_log, 0, sizeof(rate_log) );
        int rate_num = 0;

        while( getline(argss, token, ',') ){
            if( rate_num >= 4){
                cerr << "Error: only input 4 ge params." << endl;
                usage( argv[ 0 ] );
            }
            rate_log[rate_num] = myatof(token);
            ++rate_num;
        }
        if ( ( rate_num != 4) ){
            cerr << "Error: must input 4 ge params." << endl;
            usage( argv[ 0 ] );
        }
        for(int i = 0; i < 3; ++i){
            if( (0 <= rate_log[i] ) and ( rate_log[i] <= 1) ){
                /* do nothing */
            }
            else{
                cerr << "Error: loss rate must be between 0 and 1." << endl;
                usage( argv[ 0 ] );
            }
        }

        vector<string> command;

        if ( argc == 2 ) {
            command.push_back( shell_path() );
        } else {
            for ( int i = 2; i < argc; i++ ) {
                command.push_back( argv[ i ] );
            }
        }

        PacketShell<IIDLoss> loss_app( "loss", user_environment, passthrough_until_signal );

        string shell_prefix = "[loss ";
        shell_prefix += "ge_params=";
        shell_prefix += argv[ 1 ];
        shell_prefix += "] ";

        loss_app.start_uplink( shell_prefix,
                               command,
                               uplink_loss );
        loss_app.start_downlink( 0 );
        return loss_app.wait_for_exit();
    } catch ( const exception & e ) {
        print_exception( e );
        return EXIT_FAILURE;
    }
}
