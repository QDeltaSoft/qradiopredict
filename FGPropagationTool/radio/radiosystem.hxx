// radio.hxx -- FGRadio: class to manage radio propagation
//
// Written by Adrian Musceac YO8RZZ, started August 2011.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.


#include "sg/compiler.h"

#include <deque>
#include <string>
#include "sg/sg_geodesy.hxx"
#include "../groundstation.h"
#include "antenna.hxx"
#include <QDebug>
#include <QObject>
#include <QDateTime>
#include <QTime>
#include <QObject>
#include <QString>
#include "../signal.h"
#include "../flightgearprefs.h"
#include "../databaseapi.h"
#include "../mobilestation.h"
#include "scenerymanager.h"
#include "../util.h"

using std::string;

/**
 * @brief The Main radio processing subsytem class
 *          For standalone operation, this should run in a separate thread,
 *          and update the signal properties in the main thread
 */
class FGRadio : public QObject
{
    Q_OBJECT

public:

    FGRadio(DatabaseApi *db);
    ~FGRadio();

    void setMobile(MobileStation *m);
    void moveMobile();
    void nextWaypoint();
    void stop();
    void setPlotStation(GroundStation *g);

    double polarization_loss(int polarization);

    /// static convenience functions for unit conversions
    static double watt_to_dbm(double power_watt);
    static double dbm_to_watt(double dbm);
    static double dbm_to_microvolt(double dbm);
    static double dbm_to_microvolt_per_meter(double dbm, double freqMHz, double antenna_gain);

public slots:
    void update();
    void plot();

signals:
    void finished();
    void haveSignalReading(double longitude,double latitude,unsigned id, QString name, double freq, Signal *s);
    void haveMobilePosition(double lon, double lat);
    void havePlotPoint(double lon, double lat, double signal);

private:
	
	
	double _max_computation_time_norm;
	class Transmission {
	public:
        Signal *radiosignal;
        GroundStation *station;
		SGGeod pos;						// position of the other station
		SGGeod player_pos;				// player aircraft position
		double freq;					// transmission frequency
		string text;					// only for ATC
		int transmission_type;			// type: 0 for air to ground 1 for ground to air, 2 for air to air, 3 for pilot to ground, 4 for pilot to air
		int signal_type;				// 0 for ATC, 1 for VOR, LOC/GS, 3 for beacon
		int loc_gs;						// 0 none, 1 loc, 2 gs (not used now)
		int offset;
		string name;					// station name, for display purposes
		double receiver_sensitivity;
		double transmitter_power;
		double tx_antenna_height;
		double rx_antenna_height;
		double rx_antenna_gain;
		double tx_antenna_gain;
		double rx_pattern_gain;
		double tx_pattern_gain;
		string tx_antenna_type;
		string rx_antenna_type;
		double rx_line_losses;
		double tx_line_losses;
		int polarization;
		bool process_terrain;
        bool plot;
		std::deque<double> elevations;
		std::deque<string*> materials;
		double own_heading;				// player heading
		double rx_antenna_pitch;		// player pitch
		double sender_heading;			// the other station heading
		double tx_antenna_pitch;		// the other station pitch
		double localizer_spacing;
		double glideslope_first_offset;
		double glideslope_second_offset;
		unsigned int e_size;			// size of elevations
		double course;
		double reverse_course;
		SGGeoc center;
		int los_mode;
		double point_distance;			// elevation sampling distance
		double distance_m;
		double probe_distance;
		double transmitter_height;
		double receiver_height;
		double elevation_under_pilot;	// player
		double elevation_under_sender;
		double signal;					// computed signal, use only for validation
		Transmission() :
            radiosignal(0),
            station(0),
			freq(0),
			text(""),
			transmission_type(1),
			signal_type(0),
			loc_gs(0),
			offset(0),
			name(""),
			receiver_sensitivity(-110),
			transmitter_power(1),
			tx_antenna_height(1),
			rx_antenna_height(1),
			rx_antenna_gain(1),
			tx_antenna_gain(1),
			rx_pattern_gain(0),
			tx_pattern_gain(0),
			tx_antenna_type(""),
			rx_antenna_type(""),
			rx_line_losses(0),
			tx_line_losses(0),
			polarization(1),
			process_terrain(true),
            plot(false),
			own_heading(0),
			rx_antenna_pitch(0),
			sender_heading(0),
			tx_antenna_pitch(0),
			localizer_spacing(0),
			glideslope_first_offset(0),
			glideslope_second_offset(0),
			e_size(0),
			course(0),
			reverse_course(0),
			los_mode(0),
			point_distance(90),
			distance_m(0),
			probe_distance(0),
			transmitter_height(1),
			receiver_height(1),
			elevation_under_pilot(0),
			elevation_under_sender(0),
			signal(0){};
		
		Transmission(Transmission * t) :
            radiosignal(t->radiosignal),
			station(t->station),
			pos(t->pos),
			player_pos(t->player_pos),
			freq(t->freq),
			text(t->text),
			transmission_type(t->transmission_type),
			signal_type(t->signal_type),
			loc_gs(t->loc_gs),
			offset(t->offset),
			name(t->name),
			receiver_sensitivity(t->receiver_sensitivity),
			transmitter_power(t->transmitter_power),
			tx_antenna_height(t->tx_antenna_height),
			rx_antenna_height(t->rx_antenna_height),
			rx_antenna_gain(t->rx_antenna_gain),
			tx_antenna_gain(t->tx_antenna_gain),
			rx_pattern_gain(t->rx_pattern_gain),
			tx_pattern_gain(t->tx_pattern_gain),
			tx_antenna_type(t->tx_antenna_type),
			rx_antenna_type(t->rx_antenna_type),
			rx_line_losses(t->rx_line_losses),
			tx_line_losses(t->tx_line_losses),
			polarization(t->polarization),
			process_terrain(t->process_terrain),
            plot(t->plot),
			elevations(t->elevations),
			materials(t->materials),
			own_heading(t->own_heading),
			rx_antenna_pitch(t->rx_antenna_pitch),
			sender_heading(t->sender_heading),
			tx_antenna_pitch(t->tx_antenna_pitch),
			localizer_spacing(t->localizer_spacing),
			glideslope_first_offset(t->glideslope_first_offset),
			glideslope_second_offset(t->glideslope_second_offset),
			e_size(t->e_size),
			course(t->course),
			reverse_course(t->reverse_course),
			center(t->center),
			los_mode(t->los_mode),
			point_distance(t->point_distance),
			distance_m(t->distance_m),
			probe_distance(t->probe_distance),
			transmitter_height(t->transmitter_height),
			receiver_height(t->receiver_height),
			elevation_under_pilot(t->elevation_under_pilot),
			elevation_under_sender(t->elevation_under_sender),
			signal(t->signal){};
			
		~Transmission(){};
	};
	
	typedef std::deque<Transmission*> RadioTransmissions;

    RadioTransmissions _beacon_transmissions;
    RadioTransmissions *_plot_transmissions;
	
	typedef std::map<std::string, FGRadioAntenna*> AntennaList;
	AntennaList _antennas;
	
    unsigned _last_beacon_update;
    QTime _start_move;
    bool _move_flag;
    QVector<FlightPlanPoints*> _fp_points;
    unsigned _current_waypoint;
    FlightgearPrefs *_settings;
	double _terrain_sampling_distance;
	
	std::map<string, double[2]> _mat_database;

    SceneryManager *_scenery;
    MobileStation *_mobile;
    DatabaseApi *_db;

	int _propagation_model; /// 0 none, 1 round Earth, 2 ITM
	bool _suspended;
    unsigned _mtex;
    unsigned _run;
    GroundStation *_plot_station;

    bool _timer_started;
	
	void setupTransmission(Transmission* transmission);
	
	void processTerrain(Transmission* transmission);
	void processSignal(Transmission* transmission);
	
	
/***  Implement radio attenuation		
*	  based on the Longley-Rice propagation model
*	@param: radio transmission
***/
	void attenuationITM(Transmission* transmission);
	
/*** a simple alternative LOS propagation model (WIP)
*	@param: radio transmission
***/
	void attenuationLOS(Transmission* transmission);
	
	void attenuationAntenna(Transmission* transmission);
	
/*** Calculate losses due to vegetation and urban clutter (WIP)
*	 We are only worried about clutter loss, terrain influence 
*	 on the first Fresnel zone is calculated in the ITM functions
*	@param: frequency, elevation data, terrain type, horizon distances, calculated loss
***/
	void attenuationClutter(double freq, double itm_elev[], std::deque<string*> &materials,
			double transmitter_height, double receiver_height, int p_mode,
			double horizons[], double &clutter_loss);
	
/*** 	Temporary material properties database
*		@param: terrain type, median clutter height, radiowave attenuation factor
***/
	void get_material_properties_static(string* mat_name, double &height, double &density);
	void get_material_properties(string* mat_name, double &height, double &density);
	
	struct MaterialProperties {
		std::string material_name;
		double height;
		double density;
	};
	typedef std::vector<MaterialProperties*> MaterialsRadioProps;
	MaterialsRadioProps _radio_props;
	
	void load_material_radio_properties();
	
	
    void receive(QVector<GroundStation *> gs);
	

       
};


