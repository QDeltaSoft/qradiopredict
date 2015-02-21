// radiosystem.cxx -- implementation of FGRadio subsystem
// Class to manage radio propagation using the ITM model
// Written by Adrian Musceac YO8RZZ at gmail dot com, started August 2011.
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


#include <math.h>

#include <stdlib.h>
#include <deque>
#include <fstream>
#include "radiosystem.hxx"

#include <boost/scoped_array.hpp>

#define WITH_POINT_TO_POINT 1
#include "itm.cpp"



FGRadio::FGRadio(DatabaseApi *db) {
	
	
	_antennas.insert(std::pair<std::string, FGRadioAntenna*>("yagi4", new FGRadioAntenna("yagi4")));

	load_material_radio_properties();
	if(_radio_props.size() < 2) {
        qDebug() << "Radio: Unable to read materials properties from file;using static definitions";
	}
    QVector<FlightgearPrefs *> prefs = db->select_prefs();
    if(prefs.size()>0)
    {
        _settings = prefs[0];
    }
    else
    {
        qDebug() << "Could not load settings";
        emit errorFound("Error: Could not load settings");
        _settings = 0;
    }
	
    _propagation_model = ITMModel;
    _scenery = new SceneryManager(db,_settings);
    _db=db;
    _run=1;
	
    _mtex = 0;
    _mobile = new MobileStation;
    _terrain_sampling_distance =  90.0; // regular SRTM is 90 meters
	
    _max_computation_time_norm = 5.9;
    _last_beacon_update = QDateTime::currentDateTime().toTime_t();

    _fp_points = _db->select_flightplan_positions(0);
    _current_waypoint =0;
    _start_move = QTime::currentTime();
    _timer_started = false;
    _move_flag=true;
    _plot_station = NULL;
    _plot_transmissions = new RadioTransmissions;

}

FGRadio::~FGRadio() 
{

	for (unsigned i =0; i < _beacon_transmissions.size(); i++) {
		delete _beacon_transmissions[i];
	}
	for (unsigned i =0; i < _radio_props.size(); i++) {
		delete _radio_props[i];
	}
	delete _antennas["yagi4"];
	_radio_props.clear();
	_antennas.clear();
	_beacon_transmissions.clear();
    for(int i=0;i<_fp_points.size();++i)
    {
        delete _fp_points[i];
    }
    _fp_points.clear();
    delete _scenery;
    delete _mobile;
    delete _settings;
    delete _plot_transmissions;
}

void FGRadio::moveMobile()
{
    //if(!_move_flag) return;
    if(!(_fp_points.size()>0))
    {
        emit errorFound("Error: No waypoints set");
        this->stop();
        return;
    }
    if((_start_move.elapsed() < 10*1000) && !_timer_started) return;
    if(_timer_started) _timer_started = false;
    _start_move.restart();
    if(_current_waypoint >= _fp_points.size())
    {
        _current_waypoint = 0;
    }
    FlightPlanPoints *fp = _fp_points[_current_waypoint];
    _current_waypoint++;
    MobileStation m;
    m.latitude = fp->latitude;
    m.longitude = fp->longitude;
    m.elevation_feet = fp->altitude;
    m.heading_deg = 0;
    setMobile(&m);
    //_move_flag = false;

}

void FGRadio::nextWaypoint()
{

    if(_current_waypoint >= _fp_points.size())
    {
        _current_waypoint = 0;
    }
    FlightPlanPoints *fp = _fp_points[_current_waypoint];

    MobileStation m;
    m.latitude = fp->latitude;
    m.longitude = fp->longitude;
    m.elevation_feet = fp->altitude;
    m.heading_deg = 0;
    setMobile(&m);
    _current_waypoint++;
}

void FGRadio::stop()
{
    _run=0;
}

void FGRadio::setPlotStation(GroundStation *g)
{
    _plot_station = g;
}


void FGRadio::plot()
{
    qDebug() << "Plotting...";
    while (_plot_station ==NULL) {}
    GroundStation *station = _plot_station;
    if(!station || !(station->enabled == 1))
    {
        qDebug() << "No station found";
        emit errorFound("Error: No station found");
        this->stop();
        return;
    }

    double lat, lon, elev, heading, pitch;
    lat = station->latitude;
    lon = station->longitude;
    elev = station->elevation_feet;
    heading = station->heading_deg;
    pitch = station->pitch_deg;

    if( !(lat) || !(lon) || (lat > 90.0) || (lat < -90.0) || (lon > 180.0) || (lon <-180.0))
        return;

    if((station->frequency < 26.0) || (station->frequency > 20000.0)) {	// frequency out of recommended range
        qDebug() << "Frequency out of bounds";
        emit errorFound("Error: frequency out of bounds");
        this->stop();
        return;
    }
    // This would pre-load all tiles around the station on 3+ distance
    //_scenery->preloadTiles(lon,lat);
    SGGeod center = SGGeod::fromDegM(station->longitude, station->latitude, station->elevation_feet * SG_FEET_TO_METER);
    QVector<SGGeod*> *positions;
    if(_settings->_scale_with_distance == 1)
    {
        positions = Util::drawDisk(center,_settings->_plot_range*1000,1,_terrain_sampling_distance,true);
    }
    else
    {
        positions = Util::drawDisk(center,_settings->_plot_range*1000,1,_terrain_sampling_distance);
    }
    emit nrOfPos(positions->size());
    for(int i=0;i<positions->size();++i)
    {
        SGGeod *plot_pos = positions->at(i);


        SGGeod tx_pos = SGGeod::fromDegM(lon, lat, elev * SG_FEET_TO_METER);

        Transmission * transmission = new Transmission();
        transmission->station = NULL;
        transmission->pos = tx_pos;
        transmission->freq = station->frequency;

        transmission->transmission_type = station->transmission_type;

        transmission->name = station->name.toUtf8().constData();

        transmission->receiver_sensitivity = station->rx_sensitivity;

        transmission->transmitter_power =  watt_to_dbm(station->tx_power_watt);

        transmission->tx_antenna_height = station->tx_antenna_height;

        transmission->rx_antenna_height = station->rx_antenna_height;

        transmission->rx_antenna_gain = station->rx_antenna_gain;
        transmission->tx_antenna_gain = station->tx_antenna_gain;

        transmission->rx_line_losses = station->rx_line_losses;
        transmission->tx_line_losses = station->tx_line_losses;

        transmission->rx_antenna_type = station->rx_antenna_type.toUtf8().constData();
        transmission->tx_antenna_type = station->tx_antenna_type.toUtf8().constData();

        transmission->polarization = station->polarization;
        transmission->sender_heading = heading;
        transmission->tx_antenna_pitch = pitch;

        transmission->plot = true;
        transmission->process_terrain = true;

        double own_lat = plot_pos->getLatitudeDeg();
        double own_lon = plot_pos->getLongitudeDeg();

        double own_alt_ft = 0;
        double own_heading = 0;
        double own_alt= own_alt_ft * SG_FEET_TO_METER;


        transmission->own_heading = own_heading;

        SGGeod own_pos = SGGeod::fromDegM( own_lon, own_lat, own_alt );
        SGGeod max_own_pos = SGGeod::fromDegM( own_lon, own_lat, SG_MAX_ELEVATION_M );
        SGGeoc center = SGGeoc::fromGeod( max_own_pos );
        SGGeoc own_pos_c = SGGeoc::fromGeod( own_pos );


        double sender_alt_ft,sender_alt;
        transmission->transmitter_height=0.0;
        transmission->receiver_height=0.0;
        SGGeod sender_pos = transmission->pos;

        sender_alt_ft = sender_pos.getElevationFt();
        sender_alt = sender_alt_ft * SG_FEET_TO_METER;
        SGGeod max_sender_pos = SGGeod::fromGeodM( transmission->pos, SG_MAX_ELEVATION_M );
        SGGeoc sender_pos_c = SGGeoc::fromGeod( sender_pos );
        transmission->player_pos = own_pos;

        transmission->point_distance= _terrain_sampling_distance;
        transmission->course = SGGeodesy::courseRad(own_pos_c, sender_pos_c);
        transmission->reverse_course = SGGeodesy::courseRad(sender_pos_c, own_pos_c);
        transmission->distance_m = SGGeodesy::distanceM(own_pos, sender_pos);
        transmission->probe_distance = 0.0;
        transmission->center = center;

        /**	Make sampling distance proportional with distance between terminals
        *	this decreases precision for large distances, but improves performance
        */

        if( _settings->_itm_radio_performance == 1 )  {
            if (transmission->distance_m > 50000) {

                transmission->point_distance= transmission->distance_m * 2 /1000;
            }
        }

        string mat="#";

        int max_points = (int)floor(transmission->distance_m / transmission->point_distance);

        double elevation_under_pilot;
        if(own_alt == 0.0)
        {
            transmission->receiver_height = 1;
        }
        else if (_scenery->get_elevation_m( max_own_pos, elevation_under_pilot, mat)) {
            transmission->receiver_height = own_alt - elevation_under_pilot;
        }
        else {
            transmission->receiver_height = own_alt;
        }



        double elevation_under_sender = 0.0;
        mat ="#";
        if(sender_alt == 0.0)
        {
            transmission->transmitter_height = 1;
        }
        else if (_scenery->get_elevation_m( max_sender_pos, elevation_under_sender, mat )) {
            transmission->transmitter_height = sender_alt - elevation_under_sender;
        }
        else {
            transmission->transmitter_height = sender_alt;
        }
        _scenery->get_elevation_m( max_own_pos, elevation_under_pilot, mat);
        _scenery->get_elevation_m( max_sender_pos, elevation_under_sender, mat );
        transmission->elevation_under_pilot = elevation_under_pilot;
        transmission->elevation_under_sender = elevation_under_sender;


        transmission->transmitter_height += transmission->tx_antenna_height;
        transmission->receiver_height += transmission->rx_antenna_height;



        transmission->e_size = max_points;
        _plot_transmissions->push_back(transmission);
    }
    for(int i=0;i<positions->size();++i)
    {
        delete positions->at(i);
    }
    positions->clear();
    delete positions;



    //phase #2
    while(_plot_transmissions->size() > 0)
    {
        Transmission * transmission = _plot_transmissions->front();
        if (transmission->plot_elevations->size() >= transmission->e_size)
        {

            Transmission * t = new Transmission(transmission);

            //delete transmission->radiosignal;
            for(int l =0;l<transmission->plot_materials->size();++l)
            {
                delete transmission->plot_materials->at(l);
            }
            transmission->plot_elevations->clear();
            transmission->plot_materials->clear();

            delete transmission->plot_elevations;
            delete transmission->plot_materials;
            delete transmission;
            _plot_transmissions->pop_front();
            processTerrain(t);
            continue;
        }
        transmission->probe_distance += transmission->point_distance;
        SGGeod probe = SGGeod::fromGeoc(transmission->center.advanceRadM( transmission->course, transmission->probe_distance ));
        string material;
        double elevation_m = 0.0;

        if (_scenery->get_elevation_m( probe, elevation_m, material ))
        {

            if((transmission->transmission_type == P2G) || (transmission->transmission_type == P2A)) {
                transmission->plot_elevations->append(elevation_m);
                if(!material.empty()) {
                    string* name = new string(material);
                    transmission->plot_materials->append(name);
                }
                else {
                    string* no_material = new string("None");
                    transmission->plot_materials->append(no_material);
                }
            }
            else {
                 transmission->plot_elevations->prepend(elevation_m);
                 if(!material.empty()) {
                     string* name = new string(material);
                     transmission->plot_materials->prepend(name);
                }
                else {
                    string* no_material = new string("None");
                    transmission->plot_materials->prepend(no_material);
                }
            }
        }
        else {
            if((transmission->transmission_type == P2G) || (transmission->transmission_type == P2A)) {
                transmission->plot_elevations->append(0.0);
                string* no_material = new string("None");
                transmission->plot_materials->append(no_material);
            }
            else {
                string* no_material = new string("None");
                transmission->plot_elevations->prepend(0.0);
                transmission->plot_materials->prepend(no_material);
            }
        }
    }
    //cleanup

    for (unsigned i =0; i < _plot_transmissions->size(); i++) {
        delete _plot_transmissions->at(i);
    }
    _plot_transmissions->clear();
    // Here we should unload tiles from memory
    //_scenery->unloadTiles();
    emit finished();

}

void FGRadio::update()
{

    QVector<GroundStation *> gs = _db->select_ground_stations(0);

    _start_move.start();
    _timer_started =true;
    moveMobile();
    while (true)
    {
        //moveMobile();
        if(_run==0)
            break;
        receive(gs);

        if (_beacon_transmissions.size() > 0 )
        {
            Transmission * transmission = _beacon_transmissions.front();
            QTime start = QTime::currentTime();
            start.start();
            while ((start.elapsed()) < _max_computation_time_norm*1000)
            {

                if (transmission->elevations.size() >= transmission->e_size)
                {

                    Transmission * t = new Transmission(transmission);
                    delete transmission;
                    _beacon_transmissions.pop_front();
                    processTerrain(t);
                    //_move_flag = true;
                    break;
                }
                transmission->probe_distance += transmission->point_distance;
                SGGeod probe = SGGeod::fromGeoc(transmission->center.advanceRadM( transmission->course, transmission->probe_distance ));
                string material;
                double elevation_m = 0.0;

                if (_scenery->get_elevation_m( probe, elevation_m, material ))
                {

                    if((transmission->transmission_type == P2G) || (transmission->transmission_type == P2A)) {
                        transmission->elevations.push_back(elevation_m);
                        if(!material.empty()) {
                            string* name = new string(material);
                            transmission->materials.push_back(name);
                        }
                        else {
                            string* no_material = new string("None");
                            transmission->materials.push_back(no_material);
                        }
                    }
                    else {
                         transmission->elevations.push_front(elevation_m);
                         if(!material.empty()) {
                             string* name = new string(material);
                             transmission->materials.push_front(name);
                        }
                        else {
                            string* no_material = new string("None");
                            transmission->materials.push_front(no_material);
                        }
                    }
                }
                else {
                    if((transmission->transmission_type == P2G) || (transmission->transmission_type == P2A)) {
                        transmission->elevations.push_back(0.0);
                        string* no_material = new string("None");
                        transmission->materials.push_back(no_material);
                    }
                    else {
                        string* no_material = new string("None");
                        transmission->elevations.push_front(0.0);
                        transmission->materials.push_front(no_material);
                    }
                }
            }

        }
    }
    for(int i=0;i<gs.size();++i)
    {
        delete gs[i];
    }
    gs.clear();
    emit finished();
	
}



void FGRadio::receive(QVector<GroundStation *> gs) {
	
    for(int i=0;i<gs.size();++i)
    {
        // may we have more groundstations, maybe we get a vector, yes?
        // so why delete old code, huh? no make sense
        GroundStation *station = gs[i];
        if(!station || !(station->enabled == 1))
        {
            continue;
        }


        if ((QDateTime::currentDateTime().toTime_t() - station->last_update) < station->beacon_delay)
        {
            // only beacon we have, yes
            continue;
        }

        else
        {
            station->last_update = QDateTime::currentDateTime().toTime_t();
        }


        //station->last_update =  QTime::currentTime();
        double lat, lon, elev, heading, pitch;
        lat = station->latitude;
        lon = station->longitude;
        elev = station->elevation_feet;
        heading = station->heading_deg;
        pitch = station->pitch_deg;

        if( !(lat) || !(lon) || (lat > 90.0) || (lat < -90.0) || (lon > 180.0) || (lon <-180.0))
            continue;




        if((station->frequency < 26.0) || (station->frequency > 20000.0)) {	// frequency out of recommended range
            continue;
        }



        SGGeod tx_pos = SGGeod::fromDegM(lon, lat, elev * SG_FEET_TO_METER);

        Transmission * transmission = new Transmission();
        transmission->station = station;
        transmission->pos = tx_pos;
        transmission->freq = station->frequency;

        transmission->transmission_type = station->transmission_type;




        transmission->name = station->name.toUtf8().constData();

        transmission->receiver_sensitivity = station->rx_sensitivity;

        transmission->transmitter_power =  watt_to_dbm(station->tx_power_watt);

        transmission->tx_antenna_height = station->tx_antenna_height;

        transmission->rx_antenna_height = station->rx_antenna_height;


        transmission->rx_antenna_gain = station->rx_antenna_gain;
        transmission->tx_antenna_gain = station->tx_antenna_gain;

        transmission->rx_line_losses = station->rx_line_losses;
        transmission->tx_line_losses = station->tx_line_losses;

        transmission->rx_antenna_type = station->rx_antenna_type.toUtf8().constData();
        transmission->tx_antenna_type = station->tx_antenna_type.toUtf8().constData();

        transmission->polarization = station->polarization;
        transmission->sender_heading = heading;
        transmission->tx_antenna_pitch = pitch;


        transmission->process_terrain = true;

        setupTransmission(transmission);

    }
		

}

void FGRadio::setMobile(MobileStation *m)
{
    _mtex = 0;
    _mobile->latitude = m->latitude;
    _mobile->longitude = m->longitude;
    _mobile->elevation_feet = m->elevation_feet;
    _mobile->heading_deg = m->heading_deg;
    emit haveMobilePosition(_mobile->longitude,_mobile->latitude);
    _mtex = 1;
}

void FGRadio::setupTransmission(Transmission* transmission) {
	

	if((transmission->freq < 40.0) || (transmission->freq > 20000.0)) {	// frequency out of recommended range 
        qDebug() << "FGRadio:: received transmission with frequency outside of normal range";

        delete transmission;
		return;
	}
	
	
    double own_lat = _mobile->latitude;
    double own_lon = _mobile->longitude;
    double own_alt_ft = _mobile->elevation_feet;
    double own_heading = _mobile->heading_deg;
	double own_alt= own_alt_ft * SG_FEET_TO_METER;

	
    if(own_alt == 0.0) //then our station is on ground
    {
        // here will be some code
    }
	transmission->own_heading = own_heading;
	
	SGGeod own_pos = SGGeod::fromDegM( own_lon, own_lat, own_alt );
	SGGeod max_own_pos = SGGeod::fromDegM( own_lon, own_lat, SG_MAX_ELEVATION_M );
	SGGeoc center = SGGeoc::fromGeod( max_own_pos );
	SGGeoc own_pos_c = SGGeoc::fromGeod( own_pos );
	
	
	double sender_alt_ft,sender_alt;
	transmission->transmitter_height=0.0;
	transmission->receiver_height=0.0;
	SGGeod sender_pos = transmission->pos;
	
	sender_alt_ft = sender_pos.getElevationFt();
	sender_alt = sender_alt_ft * SG_FEET_TO_METER;
	SGGeod max_sender_pos = SGGeod::fromGeodM( transmission->pos, SG_MAX_ELEVATION_M );
	SGGeoc sender_pos_c = SGGeoc::fromGeod( sender_pos );
	transmission->player_pos = own_pos;
	
	transmission->point_distance= _terrain_sampling_distance; 
	transmission->course = SGGeodesy::courseRad(own_pos_c, sender_pos_c);
	transmission->reverse_course = SGGeodesy::courseRad(sender_pos_c, own_pos_c);
	transmission->distance_m = SGGeodesy::distanceM(own_pos, sender_pos);
	transmission->probe_distance = 0.0;
	transmission->center = center;
	
	/**	Make sampling distance proportional with distance between terminals
	*	this decreases precision for large distances, but improves performance
	*/
	
    if( _settings->_itm_radio_performance == 1 )  {
		if (transmission->distance_m > 50000) {
			
			transmission->point_distance= transmission->distance_m * 2 /1000; 
		}
	}
	
    string mat="#";
			
	int max_points = (int)floor(transmission->distance_m / transmission->point_distance);
	//double delta_last = fmod(distance_m, point_distance);
	
    double elevation_under_pilot;
    if(own_alt == 0.0)
    {
        transmission->receiver_height = 1;
    }
    else if (_scenery->get_elevation_m( max_own_pos, elevation_under_pilot, mat)) {
        transmission->receiver_height = own_alt - elevation_under_pilot;
	}
	else {
		transmission->receiver_height = own_alt;
	}

    //transmission->receiver_height = own_alt - elevation_under_pilot;

	double elevation_under_sender = 0.0;
    mat ="#";
    if(sender_alt == 0.0)
    {
        transmission->transmitter_height = 1;
    }
    else if (_scenery->get_elevation_m( max_sender_pos, elevation_under_sender, mat )) {
        transmission->transmitter_height = sender_alt - elevation_under_sender;
	}
	else {
		transmission->transmitter_height = sender_alt;
	}
    _scenery->get_elevation_m( max_own_pos, elevation_under_pilot, mat);
    _scenery->get_elevation_m( max_sender_pos, elevation_under_sender, mat );
	transmission->elevation_under_pilot = elevation_under_pilot;
	transmission->elevation_under_sender = elevation_under_sender;
	
	
	transmission->transmitter_height += transmission->tx_antenna_height;
	transmission->receiver_height += transmission->rx_antenna_height;
	
	

	
#if 0
	if ((transmission->distance_m > 300000) || (own_alt > 8000)) {
		attenuationLOS(transmission);
		processSignal(transmission);
		return;
	}
#endif
	
	transmission->e_size = (deque<unsigned>::size_type)max_points;
	

    if(_beacon_transmissions.size() > 20) {
        qDebug() << "ITM:: number of beacon transmissions is too high: ";
        delete transmission;
        return;
    }
    _beacon_transmissions.push_back(transmission);

	
}

void FGRadio::processTerrain(Transmission* transmission) {

    if(transmission->plot)
    {
        if((transmission->transmission_type == P2G ) || (transmission->transmission_type == P2A)) {
            transmission->plot_elevations->prepend(transmission->elevation_under_pilot);
            //if (delta_last > (point_distance / 2) )
                transmission->plot_elevations->append(transmission->elevation_under_sender);
        }
        else {
            transmission->plot_elevations->append(transmission->elevation_under_pilot);
            //if (delta_last > (point_distance / 2) )
                transmission->plot_elevations->prepend(transmission->elevation_under_sender);
        }


        double num_points= (double)transmission->plot_elevations->size();


        transmission->plot_elevations->prepend(transmission->point_distance);
        transmission->plot_elevations->prepend(num_points -1);

    }
    else
    {
        if((transmission->transmission_type == P2G) || (transmission->transmission_type == P2A)) {
            transmission->elevations.push_front(transmission->elevation_under_pilot);
            //if (delta_last > (point_distance / 2) )
                transmission->elevations.push_back(transmission->elevation_under_sender);
        }
        else {
            transmission->elevations.push_back(transmission->elevation_under_pilot);
            //if (delta_last > (point_distance / 2) )
                transmission->elevations.push_front(transmission->elevation_under_sender);
        }


        double num_points= (double)transmission->elevations.size();


        transmission->elevations.push_front(transmission->point_distance);
        transmission->elevations.push_front(num_points -1);

    }
	

	
	
	
	attenuationITM(transmission);
	processSignal(transmission);	

}

void FGRadio::processSignal(Transmission* transmission) {
	

    if(transmission->plot)
    {
        SGGeoc pos_a = SGGeoc::fromGeod(transmission->player_pos);
        double course = transmission->course+SGD_PI/2;
        double step_degree = 1.0;
        if(_settings->_scale_with_distance == 1)
        {
            if(transmission->probe_distance < 5000)
            {
                step_degree = 1.0;
            }
            if(transmission->probe_distance > 5000 && transmission->probe_distance <= 20000)
            {
                step_degree = 0.5;
            }
            if(transmission->probe_distance >  20000)
            {
                step_degree = 0.2;
            }
        }

        double dist = transmission->probe_distance*step_degree*sin(SGD_PI/180);

        SGGeoc pos_b = pos_a.advanceRadM(transmission->course, 90.0);
        SGGeoc pos_c = pos_b.advanceRadM(course, dist);
        SGGeoc pos_d = pos_a.advanceRadM(course, dist);
        SGGeod pos1 = SGGeod::fromGeoc(pos_b);
        SGGeod pos2 = SGGeod::fromGeoc(pos_c);
        SGGeod pos3 = SGGeod::fromGeoc(pos_d);
        double signal = transmission->radiosignal->signal;

        emit havePlotPoint(transmission->player_pos.getLongitudeDeg(),transmission->player_pos.getLatitudeDeg(),
                           pos1.getLongitudeDeg(),pos1.getLatitudeDeg(),
                           pos2.getLongitudeDeg(),pos2.getLatitudeDeg(),
                           pos3.getLongitudeDeg(),pos3.getLatitudeDeg(),
                           transmission->probe_distance, signal);
        transmission->plot_elevations->clear();
        delete transmission->radiosignal;

        /*
        for(int i=0;i<transmission->plot_materials->size();++i)
        {
            delete transmission->plot_materials->at(i);
        }
        */
        transmission->plot_materials->clear();
        delete transmission->plot_elevations;
        delete transmission->plot_materials;

    }
    else
    {
        emit haveSignalReading(_mobile->longitude,_mobile->latitude,
                               transmission->station->id, transmission->station->name,
                               transmission->freq,transmission->radiosignal);
    }

    //delete transmission->station;
    //delete transmission->radiosignal;
	delete transmission;
	
}

void FGRadio::attenuationITM(Transmission* transmission) {

	
	
	/** ITM default parameters 
		TODO: take them from tile materials (especially for sea)?
	**/
	double eps_dielect=15.0;
	double sgm_conductivity = 0.005;
	double eno = 301.0;
		
	int radio_climate = 5;		// continental temperate
	int pol= transmission->polarization;	
	double conf = 0.90;	// 90% of situations and time, take into account speed
	double rel = 0.90;	
	double dbloss;
	char strmode[150];
	int p_mode = 0; // propagation mode selector: 0 LOS, 1 diffraction dominant, 2 troposcatter
	double horizons[2];
	int errnum;
	
	double clutter_loss = 0.0; 	// loss due to vegetation and urban
	double tx_pow = transmission->transmitter_power;
	double ant_gain = transmission->rx_antenna_gain + transmission->tx_antenna_gain;
	double signal = 0.0;
	
	
	double link_budget = tx_pow - transmission->receiver_sensitivity - transmission->rx_line_losses - 
			transmission->tx_line_losses + ant_gain;	
	double signal_strength = tx_pow - transmission->rx_line_losses - transmission->tx_line_losses + ant_gain;	
	double tx_erp = dbm_to_watt(tx_pow + transmission->tx_antenna_gain - transmission->tx_line_losses);
    int size;
    if(transmission->plot)
    {
        size = transmission->plot_elevations->size();
    }
    else
    {
        size = transmission->elevations.size();
    }

	boost::scoped_array<double> itm_elev( new double[size] );

	for(int i=0;i<size;i++) {
        if(transmission->plot)
        {
            itm_elev[i]=transmission->plot_elevations->at(i);
        }
        else
        {
            itm_elev[i]=transmission->elevations[i];
        }

	}

    double h1, h2;
    if((transmission->transmission_type == P2G) || (transmission->transmission_type == P2A)) {
		// the sender and receiver roles are switched
        h1 = transmission->receiver_height;
        h2 = transmission->transmitter_height;
	}
	else {
        h2 = transmission->receiver_height;
        h1 = transmission->transmitter_height;
	}

    if( _settings->_use_ITWOM == 1 )
    {
        ITWOM::point_to_point(itm_elev.get(), h1, h2,
            eps_dielect, sgm_conductivity, eno, transmission->freq, radio_climate,
            pol, conf, rel, dbloss, strmode, p_mode, horizons, errnum);
    }
    else
    {
        ITM::point_to_point(itm_elev.get(), h1, h2,
            eps_dielect, sgm_conductivity, eno, transmission->freq, radio_climate,
            pol, conf, rel, dbloss, strmode, p_mode, horizons, errnum);
    }
    if( _settings->_use_clutter == 1 )
    {

            attenuationClutter(transmission->freq, itm_elev.get(), transmission->materials,
                h1, h2, p_mode, horizons, clutter_loss);

    }
	
	double pol_loss = 0.0;

	pol_loss = polarization_loss(transmission->polarization);
	

	//cerr << "ITM:: Link budget: " << link_budget << ", Attenuation: " << dbloss << " dBm, " << strmode << ", Error: " << errnum << endl;
    Signal *s = new Signal;
    s->rx_height = transmission->receiver_height;
    s->tx_height = transmission->transmitter_height;
    s->distance = transmission->distance_m / 1000;
    s->link_budget = link_budget;
    s->terrain_attenuation = dbloss;
    s->prop_mode =  strmode;
    s->clutter_attenuation = clutter_loss;
    s->polarization_attenuation = pol_loss;
		
    if( _settings->_use_antenna_pattern ==1 )
		attenuationAntenna(transmission);
	
	signal = link_budget - dbloss - clutter_loss + pol_loss + transmission->rx_pattern_gain + transmission->tx_pattern_gain;
	double signal_strength_dbm = signal_strength - dbloss - clutter_loss + pol_loss + 
			transmission->rx_pattern_gain + transmission->tx_pattern_gain;
	double field_strength_uV = dbm_to_microvolt(signal_strength_dbm);
	
	double field_strength_uV_per_meter = dbm_to_microvolt_per_meter(signal_strength_dbm, transmission->freq, transmission->rx_antenna_gain);
	
    s->signal_dbm = signal_strength_dbm;
    s->field_strength_uv = field_strength_uV;
    s->field_strength_uv_meter = field_strength_uV_per_meter;
    s->signal = signal;
    s->tx_erp = tx_erp;

    s->tx_pattern_gain = transmission->tx_pattern_gain;
    s->rx_pattern_gain = transmission->rx_pattern_gain;


    for (unsigned i =0; i < transmission->materials.size(); i++) {
        delete transmission->materials[i];
    }

	
	
	transmission->signal = signal;
    transmission->radiosignal = s;

}

void FGRadio::attenuationAntenna(Transmission* transmission) {
	double tx_pattern_gain = 0.0;
	double rx_pattern_gain = 0.0;
	
	double tx_antenna_bearing = transmission->sender_heading - transmission->reverse_course * SGD_RADIANS_TO_DEGREES;
	double rx_antenna_bearing = transmission->own_heading - transmission->course * SGD_RADIANS_TO_DEGREES;
	double rx_elev_angle = atan((transmission->elevation_under_sender + transmission->transmitter_height - 
			transmission->elevation_under_pilot + transmission->receiver_height) / transmission->distance_m) * SGD_RADIANS_TO_DEGREES;
	double tx_elev_angle = 0.0 - rx_elev_angle;
	
	if(transmission->tx_antenna_type !="") {
		std::map<string, FGRadioAntenna*>::iterator it;
		it = _antennas.find(transmission->tx_antenna_type);
		if(it != _antennas.end()) {

			FGRadioAntenna* TX_antenna = _antennas[transmission->tx_antenna_type];
			
			tx_pattern_gain = TX_antenna->calculate_gain(tx_antenna_bearing, tx_elev_angle);
			//cerr << "ITM: Tx antenna gain: " << tx_pattern_gain << endl;
		}
	}

	if(transmission->rx_antenna_type !="") {
		std::map<string, FGRadioAntenna*>::iterator it;
		it = _antennas.find(transmission->rx_antenna_type);
		if(it != _antennas.end()) {
			FGRadioAntenna* RX_antenna = _antennas[transmission->rx_antenna_type];
			
			rx_pattern_gain = RX_antenna->calculate_gain(rx_antenna_bearing, rx_elev_angle);
			
			//cerr << "ITM: Rx antenna gain: " << rx_pattern_gain << endl;
		}
	}
	
	transmission->rx_pattern_gain = rx_pattern_gain;
	transmission->tx_pattern_gain = tx_pattern_gain;
			
	
	
	
}


void FGRadio::attenuationClutter(double freq, double itm_elev[], deque<string*> &materials,
	double transmitter_height, double receiver_height, int p_mode,
	double horizons[], double &clutter_loss) {
	
	double distance_m = itm_elev[0] * itm_elev[1]; // only consider elevation points
	unsigned mat_size = materials.size();
	if (p_mode == 0) {	// LOS: take each point and see how clutter height affects first Fresnel zone
		int mat = 0;
		int j=1; 
		for (int k=3;k < (int)(itm_elev[0]) + 2;k++) {
			
			double clutter_height = 0.0;	// mean clutter height for a certain terrain type
			double clutter_density = 0.0;	// percent of reflected wave
			if((unsigned)mat >= mat_size) {	//this tends to happen when the model interferes with the antenna (obstructs)
                cerr << "Array index out of bounds 0-0: " << mat << " size: " << mat_size << endl;
				break;
			}
			get_material_properties(materials[mat], clutter_height, clutter_density);
			
			double grad = fabs(itm_elev[2] + transmitter_height - itm_elev[(int)itm_elev[0] + 2] + receiver_height) / distance_m;
			// First Fresnel radius
			double frs_rad = 548 * sqrt( (j * itm_elev[1] * (itm_elev[0] - j) * itm_elev[1] / 1000000) / (  distance_m * freq / 1000) );
			if (frs_rad <= 0.0) {	//this tends to happen when the model interferes with the antenna (obstructs)
                cerr << "Frs rad 0-0: " << frs_rad << endl;
				continue;
			}
			//double earth_h = distance_m * (distance_m - j * itm_elev[1]) / ( 1000000 * 12.75 * 1.33 );	// K=4/3
			
			double min_elev = SGMiscd::min(itm_elev[2] + transmitter_height, itm_elev[(int)itm_elev[0] + 2] + receiver_height);
			double d1 = j * itm_elev[1];
			if ((itm_elev[2] + transmitter_height) > ( itm_elev[(int)itm_elev[0] + 2] + receiver_height) ) {
				d1 = (itm_elev[0] - j) * itm_elev[1];
			}
			double ray_height = (grad * d1) + min_elev;
			
			double clearance = ray_height - (itm_elev[k] + clutter_height) - frs_rad * 8/10;		
			double intrusion = fabs(clearance);
			
			if (clearance >= 0) {
				// no losses
			}
			else if (clearance < 0 && (intrusion < clutter_height)) {
				
				clutter_loss += clutter_density * (intrusion / (frs_rad * 2) ) * pow(2.0, freq/1000.0) * (itm_elev[1]/100);
			}
			else if (clearance < 0 && (intrusion > clutter_height)) {
				clutter_loss += clutter_density * (clutter_height / (frs_rad * 2 ) ) * pow(2.0, freq/1000.0) * (itm_elev[1]/100);
			}
			else {
				// no losses
			}
			j++;
			mat++;
		}
		
	}
	else if (p_mode == 1) {		// diffraction
		
		if (horizons[1] == 0.0) {	//	single horizon: same as above, except pass twice using the highest point
			int num_points_1st = (int)floor( horizons[0] * itm_elev[0]/ distance_m ); 
			int num_points_2nd = (int)ceil( (distance_m - horizons[0]) * itm_elev[0] / distance_m ); 
			//cerr << "Diffraction 1 horizon:: points1: " << num_points_1st << " points2: " << num_points_2nd << endl;
			int last = 1;
			/** perform the first pass */
			int mat = 0;
			int j=1; 
			for (int k=3;k < num_points_1st + 2;k++) {
				if (num_points_1st < 1)
					break;
				double clutter_height = 0.0;	// mean clutter height for a certain terrain type
				double clutter_density = 0.0;	// percent of reflected wave
				
				if((unsigned)mat >= mat_size) {		
                    cerr << "Array index out of bounds 1-1: " << mat << " size: " << mat_size << endl;
					break;
				}
				get_material_properties(materials[mat], clutter_height, clutter_density);
				
				double grad = fabs(itm_elev[2] + transmitter_height - itm_elev[num_points_1st + 2] + clutter_height) / distance_m;
				// First Fresnel radius
				double frs_rad = 548 * sqrt( (j * itm_elev[1] * (num_points_1st - j) * itm_elev[1] / 1000000) / 
					( num_points_1st * itm_elev[1] * freq / 1000) );
				if (frs_rad <= 0.0) {	
                    cerr << "Frs rad 1-1: " << frs_rad << endl;
					continue;
				}
				//double earth_h = distance_m * (distance_m - j * itm_elev[1]) / ( 1000000 * 12.75 * 1.33 );	// K=4/3
				
				double min_elev = SGMiscd::min(itm_elev[2] + transmitter_height, itm_elev[num_points_1st + 2] + clutter_height);
				double d1 = j * itm_elev[1];
				if ( (itm_elev[2] + transmitter_height) > (itm_elev[num_points_1st + 2] + clutter_height) ) {
					d1 = (num_points_1st - j) * itm_elev[1];
				}
				double ray_height = (grad * d1) + min_elev;
				
				double clearance = ray_height - (itm_elev[k] + clutter_height) - frs_rad * 8/10;		
				double intrusion = fabs(clearance);
				
				if (clearance >= 0) {
					// no losses
				}
				else if (clearance < 0 && (intrusion < clutter_height)) {
					
					clutter_loss += clutter_density * (intrusion / (frs_rad * 2) ) * pow(2.0, freq/1000.0) * (itm_elev[1]/100);
				}
				else if (clearance < 0 && (intrusion > clutter_height)) {
					clutter_loss += clutter_density * (clutter_height / (frs_rad * 2 ) ) * pow(2.0, freq/1000.0) * (itm_elev[1]/100);
				}
				else {
					// no losses
				}
				j++;
				mat++;
				last = k;
			}
			
			/** and the second pass */
			mat +=1;
			j =1; // first point is diffraction edge, 2nd the RX elevation
			for (int k=last+2;k < (int)(itm_elev[0]) + 2;k++) {
				if (num_points_2nd < 1)
					break;
				double clutter_height = 0.0;	// mean clutter height for a certain terrain type
				double clutter_density = 0.0;	// percent of reflected wave
				
				if((unsigned)mat >= mat_size) {		
                    cerr << "Array index out of bounds 1-2: " << mat << " size: " << mat_size << endl;
					break;
				}
				get_material_properties(materials[mat], clutter_height, clutter_density);
				
				double grad = fabs(itm_elev[last+1] + clutter_height - itm_elev[(int)itm_elev[0] + 2] + receiver_height) / distance_m;
				// First Fresnel radius
				double frs_rad = 548 * sqrt( (j * itm_elev[1] * (num_points_2nd - j) * itm_elev[1] / 1000000) / 
					(  num_points_2nd * itm_elev[1] * freq / 1000) );
				if (frs_rad <= 0.0) {	
                    cerr << "Frs rad 1-2: " << frs_rad << " numpoints2 " << num_points_2nd << " j: " << j << endl;
					continue;
				}
				//double earth_h = distance_m * (distance_m - j * itm_elev[1]) / ( 1000000 * 12.75 * 1.33 );	// K=4/3
				
				double min_elev = SGMiscd::min(itm_elev[last+1] + clutter_height, itm_elev[(int)itm_elev[0] + 2] + receiver_height);
				double d1 = j * itm_elev[1];
				if ( (itm_elev[last+1] + clutter_height) > (itm_elev[(int)itm_elev[0] + 2] + receiver_height) ) { 
					d1 = (num_points_2nd - j) * itm_elev[1];
				}
				double ray_height = (grad * d1) + min_elev;
				
				double clearance = ray_height - (itm_elev[k] + clutter_height) - frs_rad * 8/10;		
				double intrusion = fabs(clearance);
				
				if (clearance >= 0) {
					// no losses
				}
				else if (clearance < 0 && (intrusion < clutter_height)) {
					
					clutter_loss += clutter_density * (intrusion / (frs_rad * 2) ) * pow(2.0, freq/1000.0) * (itm_elev[1]/100);
				}
				else if (clearance < 0 && (intrusion > clutter_height)) {
					clutter_loss += clutter_density * (clutter_height / (frs_rad * 2 ) ) * pow(2.0, freq/1000.0) * (itm_elev[1]/100);
				}
				else {
					// no losses
				}
				j++;
				mat++;
			}
			
		}
		else {	// double horizon: same as single horizon, except there are 3 segments
			
			int num_points_1st = (int)floor( horizons[0] * itm_elev[0] / distance_m ); 
			int num_points_2nd = (int)floor(horizons[1] * itm_elev[0] / distance_m ); 
			int num_points_3rd = (int)itm_elev[0] - num_points_1st - num_points_2nd; 
			//cerr << "Double horizon:: horizon1: " << horizons[0] << " horizon2: " << horizons[1] << " distance: " << distance_m << endl;
			//cerr << "Double horizon:: points1: " << num_points_1st << " points2: " << num_points_2nd << " points3: " << num_points_3rd << endl;
			int last = 1;
			/** perform the first pass */
			int mat = 0;
			int j=1; // first point is TX elevation, 2nd is obstruction elevation
			for (int k=3;k < num_points_1st +2;k++) {
				if (num_points_1st < 1)
					break;
				double clutter_height = 0.0;	// mean clutter height for a certain terrain type
				double clutter_density = 0.0;	// percent of reflected wave
				if((unsigned)mat >= mat_size) {		
                    cerr << "Array index out of bounds 2-1: " << mat << " size: " << mat_size << endl;
					break;
				}
				get_material_properties(materials[mat], clutter_height, clutter_density);
				
				double grad = fabs(itm_elev[2] + transmitter_height - itm_elev[num_points_1st + 2] + clutter_height) / distance_m;
				// First Fresnel radius
				double frs_rad = 548 * sqrt( (j * itm_elev[1] * (num_points_1st - j) * itm_elev[1] / 1000000) / 
					(  num_points_1st * itm_elev[1] * freq / 1000) );
				if (frs_rad <= 0.0) {		
                    cerr << "Frs rad 2-1: " << frs_rad << " numpoints1 " << num_points_1st << " j: " << j << endl;
					continue;
				}
				//double earth_h = distance_m * (distance_m - j * itm_elev[1]) / ( 1000000 * 12.75 * 1.33 );	// K=4/3
				
				double min_elev = SGMiscd::min(itm_elev[2] + transmitter_height, itm_elev[num_points_1st + 2] + clutter_height);
				double d1 = j * itm_elev[1];
				if ( (itm_elev[2] + transmitter_height) > (itm_elev[num_points_1st + 2] + clutter_height) ) {
					d1 = (num_points_1st - j) * itm_elev[1];
				}
				double ray_height = (grad * d1) + min_elev;
				
				double clearance = ray_height - (itm_elev[k] + clutter_height) - frs_rad * 8/10;		
				double intrusion = fabs(clearance);
				
				if (clearance >= 0) {
					// no losses
				}
				else if (clearance < 0 && (intrusion < clutter_height)) {
					
					clutter_loss += clutter_density * (intrusion / (frs_rad * 2) ) * pow(2.0, freq/1000.0) * (itm_elev[1]/100);
				}
				else if (clearance < 0 && (intrusion > clutter_height)) {
					clutter_loss += clutter_density * (clutter_height / (frs_rad * 2 ) ) * pow(2.0, freq/1000.0) * (itm_elev[1]/100);
				}
				else {
					// no losses
				}
				j++;
				mat++;
				last = k;
			}
			mat +=1;
			/** and the second pass */
			int last2=1;
			j =1; // first point is 1st obstruction elevation, 2nd is 2nd obstruction elevation
			for (int k=last+2;k < num_points_1st + num_points_2nd +2;k++) {
				if (num_points_2nd < 1)
					break;
				double clutter_height = 0.0;	// mean clutter height for a certain terrain type
				double clutter_density = 0.0;	// percent of reflected wave
				if((unsigned)mat >= mat_size) {		
                    cerr << "Array index out of bounds 2-2: " << mat << " size: " << mat_size << endl;
					break;
				}
				get_material_properties(materials[mat], clutter_height, clutter_density);
				
				double grad = fabs(itm_elev[last+1] + clutter_height - itm_elev[num_points_1st + num_points_2nd + 2] + clutter_height) / distance_m;
				// First Fresnel radius
				double frs_rad = 548 * sqrt( (j * itm_elev[1] * (num_points_2nd - j) * itm_elev[1] / 1000000) / 
					(  num_points_2nd * itm_elev[1] * freq / 1000) );
				if (frs_rad <= 0.0) {	
                    cerr << "Frs rad 2-2: " << frs_rad << " numpoints2 " << num_points_2nd << " j: " << j << endl;
					continue;
				}
				//double earth_h = distance_m * (distance_m - j * itm_elev[1]) / ( 1000000 * 12.75 * 1.33 );	// K=4/3
				
				double min_elev = SGMiscd::min(itm_elev[last+1] + clutter_height, itm_elev[num_points_1st + num_points_2nd +2] + clutter_height);
				double d1 = j * itm_elev[1];
				if ( (itm_elev[last+1] + clutter_height) > (itm_elev[num_points_1st + num_points_2nd + 2] + clutter_height) ) { 
					d1 = (num_points_2nd - j) * itm_elev[1];
				}
				double ray_height = (grad * d1) + min_elev;
				
				double clearance = ray_height - (itm_elev[k] + clutter_height) - frs_rad * 8/10;		
				double intrusion = fabs(clearance);
				
				if (clearance >= 0) {
					// no losses
				}
				else if (clearance < 0 && (intrusion < clutter_height)) {
					
					clutter_loss += clutter_density * (intrusion / (frs_rad * 2) ) * pow(2.0, freq/1000.0) * (itm_elev[1]/100);
				}
				else if (clearance < 0 && (intrusion > clutter_height)) {
					clutter_loss += clutter_density * (clutter_height / (frs_rad * 2 ) ) * pow(2.0, freq/1000.0) * (itm_elev[1]/100);
				}
				else {
					// no losses
				}
				j++;
				mat++;
				last2 = k;
			}
			
			/** third and final pass */
			mat +=1;
			j =1; // first point is 2nd obstruction elevation, 3rd is RX elevation
			for (int k=last2+2;k < (int)itm_elev[0] + 2;k++) {
				if (num_points_3rd < 1)
					break;
				double clutter_height = 0.0;	// mean clutter height for a certain terrain type
				double clutter_density = 0.0;	// percent of reflected wave
				if((unsigned)mat >= mat_size) {		
                    cerr << "Array index out of bounds 2-3: " << mat << " size: " << mat_size << endl;
					break;
				}
				get_material_properties(materials[mat], clutter_height, clutter_density);
				
				double grad = fabs(itm_elev[last2+1] + clutter_height - itm_elev[(int)itm_elev[0] + 2] + receiver_height) / distance_m;
				// First Fresnel radius
				double frs_rad = 548 * sqrt( (j * itm_elev[1] * (num_points_3rd - j) * itm_elev[1] / 1000000) / 
					(  num_points_3rd * itm_elev[1] * freq / 1000) );
				if (frs_rad <= 0.0) {		
                    cerr << "Frs rad 2-3: " << frs_rad << " numpoints3 " << num_points_3rd << " j: " << j << endl;
					continue;
				}
				
				//double earth_h = distance_m * (distance_m - j * itm_elev[1]) / ( 1000000 * 12.75 * 1.33 );	// K=4/3
				
				double min_elev = SGMiscd::min(itm_elev[last2+1] + clutter_height, itm_elev[(int)itm_elev[0] + 2] + receiver_height);
				double d1 = j * itm_elev[1];
				if ( (itm_elev[last2+1] + clutter_height) > (itm_elev[(int)itm_elev[0] + 2] + receiver_height) ) { 
					d1 = (num_points_3rd - j) * itm_elev[1];
				}
				double ray_height = (grad * d1) + min_elev;
				
				double clearance = ray_height - (itm_elev[k] + clutter_height) - frs_rad * 8/10;		
				double intrusion = fabs(clearance);
				
				if (clearance >= 0) {
					// no losses
				}
				else if (clearance < 0 && (intrusion < clutter_height)) {
					
					clutter_loss += clutter_density * (intrusion / (frs_rad * 2) ) * pow(2.0, freq/1000.0) * (itm_elev[1]/100);
				}
				else if (clearance < 0 && (intrusion > clutter_height)) {
					clutter_loss += clutter_density * (clutter_height / (frs_rad * 2 ) ) * pow(2.0, freq/1000.0) * (itm_elev[1]/100);
				}
				else {
					// no losses
				}
				j++;
				mat++;
				
			}
			
		}
	}
	else if (p_mode == 2) {		//	troposcatter: ignore ground clutter for now... 
		clutter_loss = 0.0;
	}
	
}



void FGRadio::load_material_radio_properties() {
	
    QString materials_radio_props("./");
	materials_radio_props.append("material_radio_properties.txt");
    if (!QFile(materials_radio_props).exists()) {
		return;
	}
    ifstream file_in(materials_radio_props.toUtf8());
	double height, density;
	std::string name;    
	while(!file_in.eof()) {
		file_in >> name >> height >> density;
		MaterialProperties *prop = new MaterialProperties;
		prop->material_name = name;
		prop->height = height;
		prop->density = density;
		_radio_props.push_back(prop);
	}
}

void FGRadio::get_material_properties(string* mat_name, double &height, double &density) {
	if(!mat_name)
		return;
	if(_radio_props.size() < 2) {
		
		get_material_properties_static(mat_name, height, density);
		return;
	}
		
	for (unsigned i =0; i < _radio_props.size(); i++) {
		MaterialProperties *prop = _radio_props[i];
		
		if ( (*mat_name == prop->material_name)) {
			height = prop->height;
			density = prop->density;
			return;
		}
	}
	height = 0.0;
	density = 0.0;
	
}


void FGRadio::get_material_properties_static(string* mat_name, double &height, double &density) {
	
	if(!mat_name)
		return;
	
	if(*mat_name == "Landmass") {
		height = 15.0;
		density = 0.2;
	}

	else if(*mat_name == "SomeSort") {
		height = 15.0;
		density = 0.2;
	}

	else if(*mat_name == "Island") {
		height = 15.0;
		density = 0.2;
	}
	else if(*mat_name == "Default") {
		height = 15.0;
		density = 0.2;
	}
	else if(*mat_name == "EvergreenBroadCover") {
		height = 20.0;
		density = 0.2;
	}
	else if(*mat_name == "EvergreenForest") {
		height = 20.0;
		density = 0.2;
	}
	else if(*mat_name == "DeciduousBroadCover") {
		height = 15.0;
		density = 0.3;
	}
	else if(*mat_name == "DeciduousForest") {
		height = 15.0;
		density = 0.3;
	}
	else if(*mat_name == "MixedForestCover") {
		height = 20.0;
		density = 0.25;
	}
	else if(*mat_name == "MixedForest") {
		height = 15.0;
		density = 0.25;
	}
	else if(*mat_name == "RainForest") {
		height = 25.0;
		density = 0.55;
	}
	else if(*mat_name == "EvergreenNeedleCover") {
		height = 15.0;
		density = 0.2;
	}
	else if(*mat_name == "WoodedTundraCover") {
		height = 5.0;
		density = 0.15;
	}
	else if(*mat_name == "DeciduousNeedleCover") {
		height = 5.0;
		density = 0.2;
	}
	else if(*mat_name == "ScrubCover") {
		height = 3.0;
		density = 0.15;
	}
	else if(*mat_name == "BuiltUpCover") {
		height = 30.0;
		density = 0.7;
	}
	else if(*mat_name == "Urban") {
		height = 30.0;
		density = 0.7;
	}
	else if(*mat_name == "Construction") {
		height = 30.0;
		density = 0.7;
	}
	else if(*mat_name == "Industrial") {
		height = 30.0;
		density = 0.7;
	}
	else if(*mat_name == "Port") {
		height = 30.0;
		density = 0.7;
	}
	else if(*mat_name == "Town") {
		height = 10.0;
		density = 0.5;
	}
	else if(*mat_name == "SubUrban") {
		height = 10.0;
		density = 0.5;
	}
	else if(*mat_name == "CropWoodCover") {
		height = 10.0;
		density = 0.1;
	}
	else if(*mat_name == "CropWood") {
		height = 10.0;
		density = 0.1;
	}
	else if(*mat_name == "AgroForest") {
		height = 10.0;
		density = 0.1;
	}
	else {
		height = 0.0;
		density = 0.0;
	}
	
}


void FGRadio::attenuationLOS(Transmission* transmission) {
	
	double dbloss;
	double signal = 0.0;
	
	double sender_alt_ft,sender_alt;
	double transmitter_height=0.0;
	double receiver_height=0.0;
	SGGeod own_pos = transmission->player_pos;
	double own_alt_ft = own_pos.getElevationFt();
	double own_alt= own_alt_ft * SG_FEET_TO_METER;
	
	
	double link_budget = transmission->transmitter_power - transmission->receiver_sensitivity - transmission->rx_line_losses -
			transmission->tx_line_losses + transmission->rx_antenna_gain + transmission->tx_antenna_gain;	

	//cerr << "ITM:: pilot Lat: " << own_lat << ", Lon: " << own_lon << ", Alt: " << own_alt << endl;
	
	
	SGGeod sender_pos = transmission->pos;
	
	sender_alt_ft = sender_pos.getElevationFt();
	sender_alt = sender_alt_ft * SG_FEET_TO_METER;
	
	receiver_height = own_alt;
	transmitter_height = sender_alt;
	
	double distance_m = transmission->distance_m;
		
	transmitter_height += transmission->tx_antenna_height;
	receiver_height += transmission->rx_antenna_height;
	double distance = sqrt(distance_m * distance_m + 
		fabs(transmitter_height - receiver_height) * fabs(transmitter_height - receiver_height));
	
	/** radio horizon calculation with wave bending k=4/3
	double receiver_horizon = 4.12 * sqrt(receiver_height);
	double transmitter_horizon = 4.12 * sqrt(transmitter_height);
	*/
	/** make k a function of frequency */
	double k = 4/3 - (transmission->freq/2000);
	double K = SGMiscd::clip(k, 1.0, 4/3);
	double receiver_horizon = sqrt(2 * SG_EARTH_RAD  * receiver_height / 1000 * K);
	double transmitter_horizon = sqrt(2 * SG_EARTH_RAD  * transmitter_height / 1000 * K);
	
	double total_horizon = receiver_horizon + transmitter_horizon;
	
	
	double pol_loss = 0.0;
	
	pol_loss = polarization_loss(transmission->polarization);
	double signal_strength = transmission->transmitter_power - transmission->rx_line_losses -
			transmission->tx_line_losses + transmission->rx_antenna_gain + transmission->tx_antenna_gain;	
	double tx_erp = dbm_to_watt(transmission->transmitter_power + transmission->tx_antenna_gain - transmission->tx_line_losses);
	
    if( _settings->_use_antenna_pattern == 1 )
		attenuationAntenna(transmission);
	
	// extra 10 db loss to fit the ITM conf and rel params (this should be linked to speed maybe)
	dbloss = 20 * log10(distance) +20 * log10(transmission->freq) -27.55;	
	if (distance / 1000 > total_horizon) {
		dbloss += 20.0 * (transmission->freq/1000)  * ((distance / 1000) - total_horizon);	// linear decay (unrealistic but it fits the purpose here better) 
	}
	signal = link_budget - dbloss + pol_loss + transmission->rx_pattern_gain + transmission->tx_pattern_gain;
	double signal_strength_dbm = signal_strength - dbloss + pol_loss + transmission->rx_pattern_gain + transmission->tx_pattern_gain;
	double field_strength_uV = dbm_to_microvolt(signal_strength_dbm);
	double field_strength_uV_per_meter = dbm_to_microvolt_per_meter(signal_strength_dbm, transmission->freq, transmission->rx_antenna_gain);
	//cerr << "LOS:: Link budget: " << link_budget << " Distance: " << distance << ", Attenuation: " << dbloss << " dBm " << endl;
	
	
    Signal *s = new Signal;
	transmission->signal = signal;
	
    s->link_budget = link_budget;
    s->terrain_attenuation = dbloss;
    s->prop_mode = "LOS";
    s->polarization_attenuation = pol_loss;
	
    s->signal_dbm = signal_strength_dbm;
    s->field_strength_uv = field_strength_uV;
    s->field_strength_uv_meter = field_strength_uV_per_meter;
    s->signal = signal;
    s->tx_erp = tx_erp;
	
    s->tx_pattern_gain = transmission->tx_pattern_gain;
    s->rx_pattern_gain = transmission->rx_pattern_gain;

    transmission->radiosignal = s;
	
}

/*** calculate loss due to polarization mismatch
*	this function is only reliable for vertical polarization
*	due to the V-shape of horizontally polarized antennas
***/
double FGRadio::polarization_loss(int polarization) {
	
	double theta_deg;
	
    double roll = 0.0;
	if (fabs(roll) > 85.0)
		roll = 85.0;
	
    double pitch = 0.0;
	if (fabs(pitch) > 85.0)
		pitch = 85.0;
	double theta = fabs( atan( sqrt( 
		pow(tan(roll * SGD_DEGREES_TO_RADIANS), 2) + 
		pow(tan(pitch * SGD_DEGREES_TO_RADIANS), 2) )) * SGD_RADIANS_TO_DEGREES);
	
	if (polarization == 0)
		theta_deg = 90.0 - theta;
	else
		theta_deg = theta;
	if (theta_deg > 85.0)	
		theta_deg = 85.0;
	
	double loss = 10 * log10( pow(cos(theta_deg * SGD_DEGREES_TO_RADIANS), 2) );
	//cerr << "Polarization loss: " << loss << " dBm " << endl;
	return loss;
}


double FGRadio::watt_to_dbm(double power_watt) {
	return 10 * log10(1000 * power_watt);	// returns dbm
}

double FGRadio::dbm_to_watt(double dbm) {
	return exp( (dbm-30) * log(10.0) / 10.0);	// returns Watts
}

double FGRadio::dbm_to_microvolt(double dbm) {
	return sqrt(dbm_to_watt(dbm) * 50) * 1000000;	// returns microvolts
}

double FGRadio::dbm_to_microvolt_per_meter(double dbm, double freqMHz, double antenna_gain) {
	double freq = freqMHz * 1000 * 1000; 
	double Z0 = 120 * SGD_PI; // Impedance of free-space [Ohm].
	double light_speed = 299792458.0; // m/s
	double power_watt = dbm_to_watt(dbm);
	double antenna_field_gain = exp( antenna_gain * log(10.0) / 10.0);
	double inter_sq = antenna_field_gain * light_speed * light_speed/(4 * SGD_PI * freq * freq);
	double power_density = power_watt / inter_sq;
	double electric_field_strength = sqrt(Z0 * power_density); // V/m
	return electric_field_strength * 1000 * 1000;	// returns microvolts/meter
}


