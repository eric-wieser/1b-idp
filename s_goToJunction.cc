#include <algorithm>
#include "util/utils.h"
#include "util/timeout.h"
#include "robot.h"

/// Exception thrown if the line is lost
struct LineLost{ LineSensors::Reading lastReading; };

struct HardwareDamaged {};

/**
	@param distance  estimated travel distance, in cm

	@throws Timeout::Expired  Took too long to get to junction
	@throws LineLost          Line lost for too long
	@throws HardwareDamaged   Line state invalid for too long
	                          Middle sensor broken? Wheels jammed?
*/
void goToJunction(Robot& r, int distance) {
	Timeout timeout = distance * 1000ms;
	vector<LineSensors::Reading::State> history;

	while(1) {
		// read the line sensors
		auto line = r.ls.read();
		history.push_front(line.state);

		// if we know where the line is, adjust our course
		if(isfinite(line.position)) {
			r.drive.move({
				forward: 0.8,
				steer: 0.2 * line.position
			});
		}
		else {
			// otherwise, we've lost the line
			// inch forward slowly, in case it was a bad reading
			r.drive.move({forward: 0.4});
		}

		// look back over the history, to deal with bad readings
		int nJunc = std::count(ALL(history), LineSensors::Reading::JUNCTION);
		int nNone = std::count(ALL(history), LineSensors::Reading::NONE);
		int nInvalid = std::count(ALL(history), LineSensors::Reading::INVALID);

		// if 3 of the last 5 readings have been junctions, we're done
		if(nJunc >= 3)
			return;

		// if the last 5 readings have been no line, we've failed
		if(nNone == 5)
			throw LineLost { line };

		// if the last 5 readings have been invalid...
		if(nInvalid == 5)
			throw HardwareDamaged();

		// only keep the last 5 readings
		if(history.size() > 5):
			history.pop_back();

		timeout.check();
		delay(10ms);
	}
}